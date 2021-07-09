/*
 * Copyright (c) 2021, Wojciech Owczarek
 * All rights reserved.
 *
 * BSD 2-clause licence - see LICENSE in the top directory
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/select.h>
#include <time.h>
#include <errno.h>

#include "serport.h"
#include "tunif.h"

static char* def_sdev = "/dev/ttyUSB0";
static char* def_ipstr = "10.99.99.1";

struct pbnet_config {
    int baudrate;
    int bpslimit;
    int txqlen;
    size_t blocksize;
    char* sdev;
    char* ipstr;
    uint8_t netmask;
};

/* config defaults */
static struct pbnet_config _config = {
    .baudrate = 9600,
    .bpslimit = 0,
    .txqlen = 5,
    .blocksize = 190,
    .netmask = 24
};

#define PROGNAME "pbnet"
#define INIT_DELAY 10000 /* usleep during the initial ping */
#define MAX_BLOCKSIZE 256
#define MTU 1500
#define V4 4
#define V6 6

#define dprintf fprintf


/* simplified IP header, just to recognise v4/v6 */
struct iphdr {
    uint8_t ver_ihl;
};

/* state holder */
struct pbnet {

    /* file descriptors */
    int sp_fd;
    int tun_fd;

    uint8_t sp_state;                        /* current state */

    /* incoming packet over serial */
    size_t sp_rx_bcount, sp_rx_oh;           /* block count and total overhead for incoming packet */
    size_t sp_rx_plen;                       /* running length of packet being assembled */
    size_t sp_rx_blen;                       /* length of currently received block */
    unsigned char sp_rx_pbuf[MTU];           /* incoming packet buffer */
    unsigned char sp_rx_bbuf[MTU];          /* incoming block buffer  */

    /* outgoing packet over serial */
    size_t sp_tx_plen, sp_tx_left, sp_tx_oh; /* length of outgoing packet, bytes left to send, total overhead */
    size_t sp_tx_bcount;                     /* block counter for outgoing packet */
    unsigned char sp_tx_buf[MTU];            /* outgoing packet buffer */
    unsigned char *sp_tx_pos;                /* position in the outgoing packet buffer */

    /* delay measurement */
    struct timespec sp_tx_blockts;           /* data->ack measurement */
    struct timespec sp_tx_pktts;             /* pkt start->end measurement */
    struct timespec sp_rx_blockts;           /* rx block start->end measurement */
    struct timespec sp_rx_pktts;             /* rx packet start->end measurement */
    struct timespec sp_rx_ibts;              /* rx inter-block delay measurement */
    double ibdur;                            /* inter-block delta */

    /* packet counters */
    uint32_t sp_rx_count, sp_tx_count;

};

/* serial port state machine to handle flow control */
enum {
    PB_READY = 0x00, /* ready to receive / transmit */
    PB_WACK  = 0x01, /* waiting for ACK */
    PB_WRTX  = 0x02, /* waiting for RTX */
    PB_INIT  = 0x03, /* waiting for PB to respond */
    PB_DEAD  = 0x04  /* down / problem */
};

/* reserved control characters for flow control, ack, etc. */
enum {
    PB_SEP = 0x00,  /* packet separator */
    PB_ACK = 0x01,  /* block acknowledgment / keepalive */
    PB_STX = 0x02,  /* stop transmission (flow control) */
    PB_RTX = 0x03,  /* resume transmission (flow control) */
    PB_MCN          /* max control char = count of control chars */
};

/* character constants to send directly */
static const char PBC_ACK = PB_ACK; /* only this one for now */
/*
 * To send arbitrary data excluding reserved symbols, we use the escapeless encoding scheme:
 *
 * http://chilliant.blogspot.com/2020/01/escapeless-restartable-binary-encoding.html
 *
 * We rely on the fact that the excluded symbols are all in the 0...n range.
 * The encoding / decoding routines are slow, unoptimised and aren't very clever in general,
 * but they are much more than enough for the purpose of exchanging data with the PB-2000,
 * that can barely handle 300 Bps in and produce 500 Bps out (raw). They will need to be
 * optimised in Pascal code in the PB-2000 though.
 *
 */

/* encode a block of data (escapeless) */
static void enc_block(const void *ibuf, void* obuf, const size_t blocklen) {

    uint8_t n = 0;
    unsigned char chk[256], c, mic = 255, mac = 0;

    const unsigned char* buf = (const unsigned char*)ibuf;
    unsigned char *out = (unsigned char*) obuf;
    unsigned char *bout = out + PB_MCN;

    memset(chk, 0, 256);
    memset(out, 0, blocklen + PB_MCN);

    /* mark all present symbols */
    for (int i = 0; i < blocklen; i++) {
        c = buf[i];
        chk[c] = 1;
        /* track min and max byte */
        if (c < mic) mic = c;
        if (c > mac) mac = c; 
    }

    /* special case #1: all chars >= pb_mcn, no substitution needed */
    if (mic >= PB_MCN) {
       /* 
        * set at least two first substitutes to the same value,
        * the receiver detects this and also does no substitution
        */
        memset(out, 255, PB_MCN); 
        /* just copy the rest */
        memcpy(bout, buf, blocklen);
        return;
    }

    /* special case #2: all chars <= (255 - mcn): just use the end of range as substitutes */
    if (mac < (256 - PB_MCN)) {
        for(int i = 0; i < PB_MCN; i++) {
            out[i] = 256 - PB_MCN + i;
        }
        goto havesub;
    }

    /* pick first n unused, non-reserved symbols and place into first n bytes of output */
    for (int i = 0; i < 256 && n < PB_MCN; i++) if(chk[i]==0 && i >= PB_MCN) out[n++] = i;

havesub:

    /* swap reserved symbols for substitutes */
    for (int i = 0; i < blocklen; i++) {
        /* here is the reliance on control symbols all being 0...PB_MCN (excl) */
        if (buf[i] < PB_MCN) *bout++ = out[buf[i]]; else *bout++ = buf[i];
    }

}

/* decode a block of data (escapeless) */
static void dec_block(const void *ibuf, void *obuf, const size_t blocklen) {

    int i,n;
    unsigned char c;
    const unsigned char* buf = (const unsigned char*)ibuf;
    unsigned char *out = (unsigned char*) obuf;

    /* same byte in first two substitutes = no substitutions, just copy */
    if (buf[0] == buf[1]) {
        memcpy(obuf, ibuf + PB_MCN, blocklen - PB_MCN);
    } else for(i = PB_MCN; i < blocklen; i++) {
        c = buf[i];
        /* here is the reliance on control symbols all being 0...PB_MCN (excl) again. */
        for(n = 0; n < PB_MCN; n++) {
            if (c == buf[n]) c = n;
        }

        *out++ = c;
    }
}

/* send a block of data to the serial port, with an optional per-byte delay */
static int sp_xmit(int fd, void* buf, size_t len) {

    if(len == 1 || _config.bpslimit == 0) {
        return write(fd, buf, len);
    } else {

        for(int i = 0; i < len; i++) {
            int ret = write(fd,buf++, 1);
            if(ret < 0) return ret;
            int sld = 9E5 / _config.bpslimit;
            while (usleep(sld) == EINTR) usleep(sld);
        }

        return len;
    }

}

/* send an ACK and wait for response indefinitely, repeating every 256 INIT_DELAY */
static void pb_ping(int fd) {

    unsigned char c;
    uint8_t n = 0;

    for(;;) {
        if(n++ == 0) {
             write(fd,&PBC_ACK,1);
        }
        usleep(INIT_DELAY);
        if (sp_nread(fd) > 0 && read(fd,&c,1) == 1 && c == PB_ACK) break;
    };

}

/* set up serial port, wait for initial ACK, set up TUN interface */
struct pbnet pbnet_init(const char* dev, const sp_params *params, const char* addr, const uint8_t mask) {

    struct pbnet ret;
    char buf[1024];

    memset(&ret, 0, sizeof(ret));
    ret.sp_tx_pos = ret.sp_tx_buf;

    dprintf(stderr, "[       TUN] Setting up tun interface with %s/%d... ", addr, mask);

    ret.tun_fd = tunif_open(addr, mask, _config.txqlen);

    dprintf(stderr, "done.\n");

    ret.sp_fd = sp_open(dev, params, SP_NONE);

    if(ret.sp_fd != -1) {
        dprintf(stderr, "[       SER] Opened %s.\n",dev);
    }

    dprintf(stderr,"[       SER] Flushing buffer...");

    for(int i = 0; i<10; i++) {
        read(ret.sp_fd, buf, 1024);
    }

    dprintf(stderr, " done.\n");
    dprintf(stderr, "[PBNET->SER] PB-2000 are you there?\n");

    pb_ping(ret.sp_fd);

    dprintf(stderr, "[PBNET<-SER] They're here!\n");


    return ret;
}

/* shut down the operation, weld down the hatch, burn the evidence */
static void pbnet_close(struct pbnet *pb) {
    if(pb->sp_fd > -1) close(pb->sp_fd);
    if(pb->tun_fd > -1) close(pb->tun_fd);
}

/* encode and send a data block to PB */
static size_t pb_send_block(struct pbnet *pb) {

    unsigned char obuf[_config.blocksize];
    size_t len = (pb->sp_tx_left > (_config.blocksize - PB_MCN)) ? (_config.blocksize - PB_MCN) : pb->sp_tx_left;
    size_t ret;

    if(pb->sp_tx_left == 0) {
        return 0;
    }

    enc_block(pb->sp_tx_pos, obuf, len);
    pb->sp_tx_left -= len;

    if(pb->sp_tx_left == 0) {
        obuf[len+PB_MCN] = PB_SEP;
        pb->sp_tx_oh++;
        len++;
    }

    clock_gettime(CLOCK_MONOTONIC, &pb->sp_tx_blockts); 
    ret = sp_xmit(pb->sp_fd, obuf, len + PB_MCN);

    pb->sp_tx_bcount++;
    pb->sp_tx_oh += PB_MCN;
    pb->sp_tx_pos += len;

    /* end of packet */
    if(pb->sp_tx_left == 0) {
        pb->sp_tx_pos = pb->sp_tx_buf;
    }

    dprintf(stderr, "[PBNET->SER] TX pkt #%d block #%d (%d B), awaiting ACK\n", pb->sp_tx_count + 1, pb->sp_tx_bcount, len + PB_MCN);

    /* now we wait for ACK */
    pb->sp_state = PB_WACK;

    return ret;

}

/* handle a batch of data (a packet) from the host side */
static void handle_tun(struct pbnet *pb, unsigned char* buf, size_t len) {

    pb->sp_tx_pos = pb->sp_tx_buf;
    pb->sp_tx_left = len;
    pb->sp_tx_plen = len;
    pb->sp_tx_oh = 0;
    pb->sp_tx_bcount = 0;

    struct iphdr *iph = (struct iphdr*) buf;

    dprintf(stderr, "[PBNET<-TUN] Got %d bytes from host\n", len);

    if ((iph->ver_ihl >> 4) != V4) {
        dprintf(stderr,"[PBNET<-IP6] Got IPv6 packet from host, will not forward\n");
        return;
    } else {
        dprintf(stderr,"[PBNET<-IP4] Queued %d-byte IPv4 packet for PB\n", len);
    }
#ifdef PB_DUMP_BUF
    dprintf(stderr,"[DEBUG DUMP] HST PK RAW: ");
    for(int j = 0; j < len; j++) {
        dprintf(stderr, "%02x ",buf[j]);
    }
    dprintf(stderr, "\n");
#endif
    clock_gettime(CLOCK_MONOTONIC, &pb->sp_tx_pktts); 
    memcpy(pb->sp_tx_buf, buf, len);
    pb_send_block(pb);

}

/* timestamp delta */
static void ts_diff(struct timespec *a, struct timespec *b) {

    /* subtract */
    a->tv_sec -= b->tv_sec;
    a->tv_nsec -= b->tv_nsec;
    /* normalise */
    if(a->tv_nsec < 0) {
        a->tv_sec--;
        a->tv_nsec += 1E9;
    }

}
/* timespec to double ms */
static double ts_ms(struct timespec *ts) {
    return ((ts->tv_sec+0.0) * 1E9 + ts->tv_nsec + 0.0)/1000000.0;
}


/* handle a portion of incoming data from the PB */
static void handle_sp(struct pbnet *pb, unsigned char* buf, size_t len) {

    unsigned char c;
    struct timespec ack_ts, pkt_ts, block_ts, ib_ts;
    double pktdur, ackdur, bps, blockdur;

#ifdef PB_DUMP_BUF
    dprintf(stderr,"[DEBUG DUMP] SER BF RAW: ");
    for(int j = 0; j < len; j++) {
        dprintf(stderr, "%02x ",buf[j]);
    }
    dprintf(stderr, "\n");
#endif

    for(int i = 0; i < len; i++) {

        c = buf[i];

        /* received a control character */
        if ( c < PB_MCN) {
             switch(c) {
                case PB_SEP: /*i++; while(i<len && buf[i] == PB_SEP) i++;*/ goto blockdone; /* end of packet: complete current block and process it */
                case PB_ACK: if(pb->sp_state == PB_WACK) {
                                    clock_gettime(CLOCK_MONOTONIC, &ack_ts);
                                    pkt_ts = ack_ts;
                                    ts_diff(&ack_ts, &pb->sp_tx_blockts);
                                    ts_diff(&pkt_ts, &pb->sp_tx_pktts);
                                    if(pb->sp_tx_left == 0) {
                                        pb->sp_tx_count++;
                                        ackdur = ts_ms(&ack_ts);
                                        pktdur = ts_ms(&pkt_ts);
                                        bps = (pb->sp_tx_plen * 1000.0) / pktdur;
                                        dprintf(stderr, "[PBNET<-SER] Got ACK (%.03f ms)\n", ackdur);
                                        dprintf(stderr, "[PBNET->SER] TX pkt #%d done, total o/h %d/%d (%.03f%%), took %.03f ms, %.0f Bps mean\n",
                                                            pb->sp_tx_count, pb->sp_tx_oh, pb->sp_tx_plen, 100.0*((pb->sp_tx_oh + 0.0) / pb->sp_tx_plen),
                                                            pktdur, bps);
                                        /* IMPORTANT: automatically pause TX to allow PB to service the packet */
                                        pb->sp_state = PB_WRTX;
                                    } else {
                                        dprintf(stderr, "[PBNET<-SER] Got ACK (%.03f ms), next block\n", ack_ts.tv_nsec / 1000000.0);
                                        pb->sp_state = PB_READY;
                                        pb_send_block(pb);
                                    }

                             } else {
                                dprintf(stderr, "[PBNET<-SER] Got stray ACK, ignoring\n");
                             }
                             break;
                case PB_STX: if(pb->sp_state != PB_WRTX) {
                                dprintf(stderr, "[PBNET<-SER] Got STX, pausing TX\n");
                                pb->sp_state = PB_WRTX;
                             } else {
                                dprintf(stderr, "[PBNET<-SER] Got STX, already paused\n");
                             }
                             break;
                case PB_RTX: if(pb->sp_state == PB_WRTX) {
                                dprintf(stderr, "[PBNET<-SER] Got RTX, resuming TX\n");
                                pb->sp_state = PB_READY;
                             } else {
                                dprintf(stderr, "[PBNET<-SER] Got RTX, already transmitting, ignoring\n");
                             }
                             break;
            }
        /* received data */
        } else {
            if(pb->sp_rx_blen == 0) {
                clock_gettime(CLOCK_MONOTONIC, &pb->sp_rx_blockts); 
                if(pb->sp_rx_bcount == 0) {
                    pb->sp_rx_oh = 0;
                    pb->sp_rx_pktts = pb->sp_rx_blockts;
                } else {
                    ib_ts = pb->sp_rx_blockts;
                    ts_diff(&ib_ts, &pb->sp_rx_ibts);
                    pb->ibdur = ts_ms(&ib_ts);
                }
            }
            pb->sp_rx_bbuf[pb->sp_rx_blen++] = c;
            if(pb->sp_rx_blen == _config.blocksize) {
blockdone:
                clock_gettime(CLOCK_MONOTONIC, &block_ts);
                pb->sp_rx_ibts = block_ts;
                pkt_ts = block_ts;
                ts_diff(&block_ts, &pb->sp_rx_blockts);
                blockdur = ts_ms(&block_ts);
                /* we can have a zero-length block if plen == blocksize - pb_mcn */
                if(pb->sp_rx_blen > 0) {
                    pb->sp_rx_oh += PB_MCN;
                    pb->sp_rx_bcount++;

                    dprintf(stderr, "[PBNET<-SER] RX pkt #%d block #%d (%d B), took %.03f ms", pb->sp_rx_count + 1, pb->sp_rx_bcount, pb->sp_rx_blen, blockdur);
                    if(pb->sp_rx_bcount > 1) {
                        dprintf(stderr,", inter-block %.03f ms", pb->ibdur);
                    }
                    dprintf(stderr, "\n");

#ifdef PB_DUMP_BUF
                    dprintf(stderr,"[DEBUG DUMP] SER BL RAW: ");
                    for(int j = 0; j < pb->sp_rx_blen; j++) {
                        dprintf(stderr, "%02x ",pb->sp_rx_bbuf[j]);
                    }
                    dprintf(stderr, "\n");
#endif
                    dec_block(pb->sp_rx_bbuf, pb->sp_rx_pbuf + pb->sp_rx_plen, pb->sp_rx_blen);
                    pb->sp_rx_plen += (pb->sp_rx_blen - PB_MCN);
                }
                if(c == PB_SEP) {
                    pb->sp_rx_oh++;
                    ts_diff(&pkt_ts, &pb->sp_rx_pktts);
                    pktdur = ts_ms(&pkt_ts);
                    bps = (pb->sp_rx_plen * 1000.0) / pktdur;
                    dprintf(stderr, "[PBNET<-SER] Got SEP, end of packet\n");
                    dprintf(stderr, "[PBNET<-SER] Rx pkt #%d done, total o/h %d/%d (%.02f%%),took %.03f ms,  %.0f Bps mean\n", pb->sp_rx_count + 1,
                            pb->sp_rx_oh, pb->sp_rx_plen,100.0*((pb->sp_rx_oh + 0.0) / pb->sp_rx_plen) ,  pktdur, bps);
                    dprintf(stderr, "[PBNET->TUN] Forwarding %d-byte packet to host\n", pb->sp_rx_plen);
#ifdef PB_DUMP_BUF
                    dprintf(stderr,"[DEBUG DUMP] SER PK DEC: ");
                    for(int j = 0; j < pb->sp_rx_plen; j++) {
                        dprintf(stderr, "%02x ",pb->sp_rx_pbuf[j]);
                    }
                    dprintf(stderr, "\n");
#endif
                    write(pb->tun_fd, pb->sp_rx_pbuf, pb->sp_rx_plen);
                    pb->sp_rx_plen = 0; pb->sp_rx_bcount = 0; pb->sp_rx_count++; pb->sp_rx_oh = 0; 
                }
                pb->sp_rx_blen = 0;
            }
        }

    }

}

static void usage() {
    dprintf(stderr,"usage: "PROGNAME" [-d STRING ] [-b NUM] [-B NUM] [-l NUM]\n"\
                   "             [-a STRING ] [-m NUM] [-q NUM] [-h]\n\n"\
                   "         -d: serial device to attach to (default:%s)\n"\
                   "         -b: baud rate (300..9600, default:%d)\n"\
                   "         -B: block size (16..256, default:%d)\n"\
                   "         -l: serial port TX rate limit in Bps/cps\n"\
                   "             (0:no limit, default:%d)\n"\
                   "         -a: IPv4 address on host side (default:%s)\n"\
                   "         -m: netmask (0..31, default:%d)\n"\
                   "         -q: TUN interface TX queue length (0..)\n"\
                   "             default:%d, 0:OS default)\n"\
                   "         -h: this here help\n"\
                   "\n",
                    _config.sdev, _config.baudrate,_config.blocksize,_config.bpslimit,
                    _config.ipstr,_config.netmask,_config.txqlen
    );

}

static int parse_config(int argc, char ** argv) {

    int c, n;

    while( (c=getopt(argc,argv, "hd:b:B:l:a:m:q:")) != -1) {
        switch(c) {
            case 'h': 
                        usage();
                        return -1;
            case 'd':
                        _config.sdev = optarg;
                        break;
            case 'b':
                        n = atoi(optarg);
                        if(n < 300 || n > 9600) {
                            dprintf(stderr,PROGNAME": baud rate out of range (300..9600)\n");
                            return -2;
                        }
                        _config.baudrate = n;
                        break;
            case 'B':
                        n = atoi(optarg);
                        if(n < 16 || n > 256) {
                            dprintf(stderr,PROGNAME": block size out of range (16..256)\n");
                            return -2;
                        }
                        _config.blocksize = n;
                        break;
            case 'l':
                        n = atoi(optarg);
                        if(n < 0) {
                            dprintf(stderr,PROGNAME": rate limit out of range (0..)\n");
                            return -2;
                        }
                        _config.bpslimit = n;
                        break;
            case 'a':
                        _config.ipstr = optarg;
                        break;
            case 'm':
                        n = atoi(optarg);
                        if(n < 0 || n>31) {
                            dprintf(stderr,PROGNAME": netmask out of range (0..31)\n");
                            return -2;
                        }
                        _config.netmask = n;
                        break;
            case 'q':
                        n = atoi(optarg);
                        if(n < 0) {
                            dprintf(stderr,PROGNAME": TX queue length out of range (0..)\n");
                            return -2;
                        }
                        _config.txqlen = n;
                        break;
            default:
                        dprintf(stderr,PROGNAME": try -h for list of options\n");
                        return -3;
        }

    }

    return 0;
}

/* startup + event loop. a good programmer would have separated this into multiple functions - yeah, sue me. */
int main (int argc, char **argv) {

    fd_set fds;
    int res, maxfd;
    int ret = 0;
    size_t len;

    unsigned char sbuf[MTU+1];
    unsigned char nbuf[MTU+1];

    _config.ipstr = def_ipstr;
    _config.sdev = def_sdev;

    if((ret=parse_config(argc, argv)) != 0) {
        return ret;
    }

    /* serial port parameters */
    sp_params params = {
        .baudrate = _config.baudrate,
        .databits = 8,
        .parity = SP_PARITY_NONE,
        .stopbits = 1,
        .flowcontrol = SP_FLOWCONTROL_NONE,
        .minchars = 0,
        .timeout = 0,
        .canonical = 0
    };

    /* set up serial port and tun interface */
    struct pbnet pb = pbnet_init(_config.sdev, &params, _config.ipstr, _config.netmask);

    /* don't worry Bob, I'll fix these later (not) */
    if (pb.sp_fd==-1) {
        dprintf(stderr, "[       ERR] Could not set up serial port\n");
        return -1;
    }
    if (pb.tun_fd == -1) {
        dprintf(stderr, "[       ERR] Could not set up tunnel interface\n");
        return -1;
    }

    /* 1970 called and wants its file descriptors back... */
    maxfd = (pb.sp_fd > pb.tun_fd) ? pb.sp_fd : pb.tun_fd;
    maxfd++;

    /*
     * There is this theory of the Moebius. A twist in the fabric of space where time becomes a loop,
     * from which there is no escape. So when we reach that point, whatever happened will happen again.
     */
    for(;;) {

        FD_ZERO(&fds);
        FD_SET(pb.sp_fd, &fds);

        /* receive packets from host unless PB requested to pause TX or waiting for ACK */
        if(pb.sp_state == PB_READY) {
            FD_SET(pb.tun_fd, &fds);
        }

        res = select(maxfd, &fds, NULL, NULL, NULL);

        if(res < 0 && errno == EINTR) {
            continue;
        }

        /* yeah, need some more safisticashun here */
        if(res < 0) {
            dprintf(stderr, "[       ERR] Select() error\n");
            ret = -1;
            break;
        }

        /* handle incoming data from PB */
        if(FD_ISSET(pb.sp_fd, &fds)) {
            len = read(pb.sp_fd, sbuf, MTU);
            if(len < 0) continue;
            handle_sp(&pb, sbuf, len);
        }

        /* handle incoming data from host, unless we were waiting for something else */
        if(pb.sp_state == PB_READY && FD_ISSET(pb.tun_fd, &fds)) {
            len = read(pb.tun_fd, nbuf, MTU);
            if(len < 0) continue;
            handle_tun(&pb, nbuf, len);
        }

    }

    /* Goodbye all you people, there's nothing you can say, to make me change, my mind, goodbye */
    pbnet_close(&pb);

    /* good boy! */
    return ret;

}
