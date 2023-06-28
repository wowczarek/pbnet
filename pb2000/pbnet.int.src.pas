{ Copyright (c) 2021, Wojciech Owczarek }
{ All rights reserved }
{ BSD 2-clause licence - see LICENSE in the top directory }

interface pbnet;
const 
    { error constants }
    e_ok=0; e_tmo=-1; e_intr=-2; e_trunc=-3; e_crc=-4;
    e_mtu=-5; e_unxp=-6; e_err=-7; e_init=-8; e_arg=-9; e_nomine=-10;
    { IP protocol numbers }
    ipr_zero=0; ipr_icmp=1; ipr_udp =17; ipr_tcp =6;
    { header lengths }
    ip_hlen=20; icmp_hlen=8; udp_hlen=8; tcp_hlen=20;
    { ICMP types }
    icmp_mecho=8; icmp_mechoreply=0;
    { DNS return codes (RCODE) }
    de_formerr=1;de_servfail=2;de_nxdomain=3;de_notimp=4;de_refused=5;de_notzone=9;
    { TCP flags }
    tc_fin=1;tc_syn=2;tc_rst=4;tc_psh=8;tc_ack=16;
    { other socket constants }
    port_any=0;
type
    quad=array [0..3] of byte;
    { an IP address - better this than a longint, really }
    ipaddr=quad;
    {tcp state}
    tcp_state=(none,listen,synsent,synrecv,estab,finwait1,finwait2,closewait,closing,lastack,timewait,closed);
    { a socket descriptor }
    socket=record
        state:tcp_state; { state (for TCP) }
        lport:word; { local port:source port for PB->world, destination port for world->PB }
        rport:word; { remore port}
        dst:ipaddr; { remote address }
    end;
    { IPv4 header }
    ip_hdr=record
        ver_ihl:byte;
        dscp_ecn:byte;
        len:word;
        id:word;
        fl_foff:word;
        ttl:byte;
        proto:byte;
        csum:word;
        src:ipaddr;
        dst:ipaddr;
    end;
    { ICMP header + payload }
    icmp_hdr=record
        mtype:byte;
        mcode:byte;
        csum:word;
        id:word; { belongs to ICMP echo, just here for convenience }
        seq:word; { belongs to ICMP echo }
        payload:array [0..1471] of byte;
    end;
    { UDP header + payload }
    udp_hdr=record
        sport:word;
        dport:word;
        len:word;
        csum:word;
        payload:array [0..1471] of byte;
    end;
    { TCP header + payload }
    tcp_hdr=record
        sport:word;
        dport:word;
        seqno: quad;
        ackno: quad;
        doff:byte; { 5 without options, 6 with MSS }
        flags:byte;
        wsize:word;
        csum:word;
        uptr:word;
        payload:array[0..1459] of byte;
    end;
    { all-in-one packet format using record variants - Pascal's riot police }
    { what riot police you say? response to unions! }
    ippkt=record
        len:word; { total length of packet }
        l4_len:word; { length of layer 4 payload:icmp, udp, tcp, etc. }
        case :boolean of
            false:{ variant 1:raw packet buffer } (data:array [0..1499] of byte);
            true: { variant 2:IPv4 header } (
                ip:ip_hdr;
                case :byte of
                    0:{ variant 1:Raw IPv4 payload } (payload:array [0..1479] of byte);
                    1:{ variant 2:ICMP + payload   } (icmp:   icmp_hdr);
                    2:{ variant 3:UDP + payload    } (udp:    udp_hdr);
                    3:{ variant 4:TCP + payload    } (tcp:    tcp_hdr)
            )
    end;
    { PBNET settings }
    pbn_cfg = record
        ip: ipaddr;
        dns: ipaddr;
        mtu: word;
        ttl: byte;
        blsize: byte;
        csums: boolean;
        baud: char;
        search: string[30];
    end;

var 
    addr_any:ipaddr=(0,0,0,0); { INADDR_ANY }
    rxcnt:word; { RX/TX counters }
    txcnt:word;
    cfg: ^pbn_cfg;
{ pbnet API }
function  strerr(e:shortint): string;
function  ipaddr_parse(var s:string;var a:ipaddr):boolean;
function  ipaddr_str(var a:ipaddr):string;
function  quad_eq(var qa:array of byte;var qb:array of byte): boolean;
procedure pkt_hold;
procedure pkt_ready;
procedure pkt_prime(var pkt:ippkt);
function  pkt_get(var pkt:ippkt;timeout:word):shortint;
function  pkt_send(var pkt:ippkt;timeout:word):shortint;
function  echo_respond(var pkt:ippkt; timeout:word):shortint;
function  udp_recv(var pkt:ippkt;var sock:socket;timeout:word):shortint;
function  udp_send(var pkt:ippkt;var sock:socket;timeout:word):shortint;
function  udp_respond(var pkt:ippkt;timeout:word):shortint;
function  tcp_connect(var pkt:ippkt;var sock:socket;timeout:word):shortint;
function  dns_resolve(shost:string;var rip:ipaddr;var pkt:ippkt;timeout: word; retr: byte):shortint;
procedure pbn_shutdown;
function  pbn_init:boolean;
function  pbn_set(sk: string; v: string):boolean;
procedure pbn_defaults;

end.
