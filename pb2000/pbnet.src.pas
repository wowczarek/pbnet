{$D-}
{$DEFINE PBNET_GUI}
{$UNDEF PBNET_GUI}
program pbnet;
uses crt;
const 
    { protocol properties }
    pb_mtu = 1500;
    pb_mcn = $04; { max control character = number of reserved characters }
    pb_mc = pb_mcn - 1; { mcn minus 1 (so we can use > not >=) }
    blocksize = 190; { block size }
    bsize = blocksize - pb_mcn; { maximum payload in a single block }
    { result constants }
    pb_ok =    0;
    pb_tmout = 1;
    pb_intr  = 2;
    pb_trunc = 3;
    pb_corrupt = 4;
    pb_mtuerr = 5;
    { control characters }
    pb_sep = $00;
    pb_ack = $01;
    pb_stx = $02;
    pb_rtx = $03;
    { IP protocol numbers }
    ipproto_zero = 0;
    ipproto_icmp = 1;
    ipproto_udp  = 17;
    ipproto_tcp  = 6;
    { location of the protocol field in IPv4 header }
    ipproto_offset = 9;
    { header lengths }
    ip_hdrlen = 20;
    icmp_hdrlen = 8;
    udp_hdrlen = 8;
    { ICMP types }
    icmp_mtype_echo = 8;
    icmp_mtype_echoreply = 0;
    { other internal constants }
    rs232_baddr = $08e2;
    {$IFDEF PBNET_GUI}
    rxcnt_pos = 19; { screen x of RX counter }
    txcnt_pos = 25; { screen x of TX counter }
    {$ENDIF}
    timer_no = 2;
type
    { a socket descriptor }
    socket = record
        proto: byte;
        state: byte;
        sport: word;
        dport: word;
    end;
    { an IP address - better this than a longint, really }
    ipaddr = array [0..3] of byte;
    { IPv4 header }
    ip_hdr = record
        ver_ihl: byte;
        dscp_ecn: byte;
        len: word;
        id: word;
        fl_foff: word;
        ttl: byte;
        proto: byte;
        csum: word;
        src: ipaddr;
        dst: ipaddr;
    end;
    { ICMP header + payload }
    icmp_hdr = record
        mtype: byte;
        mcode: byte;
        csum: word;
        id: word;
        seq: word;
        payload: array [0..1471] of byte;
    end;
    { UDP header + payload }
    udp_hdr = record
        sport: word;
        dport: word;
        len: word;
        csum: word;
        payload: array [0..1471] of byte;
    end;
    { all-in-one packet format using record variants - Pascal's riot police }
    { what riot police you say? response to unions! }
    ippkt = record
        len: word;
        case :boolean of
            false: { variant 1: raw packet buffer } (data: array [0..1499] of byte;);
            true:  { variant 2: IPv4 header } (
                ip: ip_hdr;
                case :byte of
                    0: { variant 1: Raw IPv4 payload } (payload: array [0..1479] of byte;);
                    1: { variant 2: ICMP + payload   } (icmp:    icmp_hdr;);
                    2: { variant 3: UDP + payload    } (udp:     udp_hdr)
            )
    end;
{$IFDEF PBNET_GUI}
    gc = array[0..29] of byte; { first set of custom characters }
    gc1 = array[0..17] of byte; { second set of custom characters }
{$ENDIF}

var 
    f:file; { serial port file descriptor }
    _cmap: array [0..255] of byte; { symbol check map }
    _wbuf: array [0..255] of byte; { rs232 block work buffer for rx/tx + sep }
    { control characters as variables, so they can be sent as is}
    pbc_sep: byte=pb_sep;
    pbc_ack: byte=pb_ack;
    pbc_stx: byte=pb_stx;
    pbc_rtx: byte=pb_rtx;
    rs232_cnt: byte absolute $08e0; { # bytes in rs232 buffer }
    rs232_pos: byte absolute $08e1; { buffer write position }
    rs232_buf: array [0..255] of byte absolute $08e2;
    kbbuf_cnt: byte absolute $09ec; { # bytes in keyboard buffer }
{$IFDEF PBNET_GUI}
    ch: gc absolute $506; { character map 1, #01-#04 = rx/tx arrows }
    ch1: gc1 absolute $560; { character map 2, #16-#18 = protocol dots }
    cntpad:string=' 00000';
    lastx,lasty: byte;
{$ENDIF}
    my_ip:ipaddr=(0,0,0,0); { my own ip address }
    res: byte; { general result capture variable }
    { counters }
    rxcnt:word=0;
    txcnt:word=0;

 { ---- user variables here - the above will go into a unit ---- }
    ipkt: ippkt;
    ip: ipaddr = (10,99,99,2);
    dnsip: ipaddr = (172,16,100,1);

{$IFDEF PBNET_GUI}
{ update counter display }
procedure cnt_update(tx:boolean);
var
sn: string;
begin
    lastx:=wherex; lasty:=wherey;
    byte(cntpad[0]):=6; { cheat: reset string length to original }
    if tx then begin
        cntpad[1]:=#3; { triangle up }
        gotoxy(txcnt_pos,1);
        str(txcnt,sn);
        sound(600,2);
    end else begin
        cntpad[1]:=#1; { triangle down }
        gotoxy(rxcnt_pos,1);
        str(rxcnt,sn);
        sound(800,2);
    end;
    dec(cntpad[0], length(sn)); { cheat: shortern length of padding }
    write(cntpad,sn);
    gotoxy(lastx,lasty);
end;
{ indicate RX or TX in progress }
procedure mark_progress(tx:boolean);
begin
    lastx:=wherex; lasty:=wherey;
    if tx then begin
        gotoxy(txcnt_pos,1); write(#4); { show TX marker on screen (triangle up, reversed) }
    end else begin
        gotoxy(rxcnt_pos,1); write(#2); { show RX marker on screen (triangle down, reversed) }
    end;
    gotoxy(lastx,lasty);
end;
{ mark a protocol dot on display }
procedure mark_proto(proto: byte);
begin
    lastx:=wherex; lasty:=wherey;
    gotoxy(31,1);
    case proto of
        ipproto_icmp: write(#16,#5);
        ipproto_udp: write(#17,#5);
        ipproto_tcp: write(#18,#5);
        else write(' ',#5);
    end;
    gotoxy(lastx,lasty);
end;
{$ENDIF}
{ RFC1071 checksum, as used in IP header checksum, ICMP header checksum, UDP checksum, etc. }
function pkt_checksum(var data: array of byte; pos: word; len: word): word;
begin
{$I cksum.pas}
end;
{ encode a len-byte block within a packet into wbuf, using check map cmap, starting from offset pos }
procedure enc_block(var pkt:array of byte; var cmap: array of byte; var wbuf: array of byte; pos:word; len:word);
begin
{$I enc_bl.pas}
end;
{ decode a len-byte block into packet from wbuf, starting from offset pos }
procedure dec_block(var pkt: array of byte; var wbuf: array of byte; pos: word; len: byte);
begin
{$I dec_bl.pas}
end;

{ don't send us anything for now }
procedure pkt_hold;
begin
    blockwrite(f, pbc_stx, 1); { stop transmission }
end;

{ call this once a packet has been handled or responded to (resume transmision) }
procedure pkt_done;
begin
    {$IFDEF PBNET_GUI}
    lastx:=wherex; lasty:=wherey;
    mark_proto(ipproto_zero);
    gotoxy(lastx,lasty);
    {$ENDIF}
    blockwrite(f, pbc_rtx, 1); { resume transmission }
end;

{ send an IP packet down the pipe }
procedure pkt_send(var pkt:ippkt);
var
left: word;
pos: word = 0;
n: byte; i: byte;
begin
    { swap the length field again }
    pkt.ip.len := swap(pkt.len);
    { cheksum is zero while computing the header checksum }
    pkt.ip.csum := 0;
    pkt.ip.csum := pkt_checksum(pkt.data, 0, ip_hdrlen);
    {$IFDEF PBNET_GUI}
    { show TX marker and protocol marker on screen }
    mark_proto(pkt.ip.proto); mark_progress(true);
    {$ENDIF} 
    left := pkt.len;
    { encode and send block by block }
    while left > bsize do begin
        enc_block(pkt.data, _cmap,_wbuf,pos, bsize);
        blockwrite(f,_wbuf,blocksize);
        dec(left, bsize);
        inc(pos,  bsize);
    end;
    { handle the  / last block }
    enc_block(pkt.data, _cmap, _wbuf,pos, left);
    { end of packet: send pb_mcn + left + pb_sep }
    inc(left,pb_mcn);
    _wbuf[left] := pb_sep; inc(left);
    blockwrite(f,_wbuf,left);
    inc(txcnt);
    {$IFDEF PBNET_GUI}
    cnt_update(true);
    mark_proto(ipproto_zero); { clear protocol marker }
    {$ENDIF}
end;

{ wait for an IP packet until one received or key pressed or timeout }
function pkt_get(var pkt: ippkt; timeout: word): byte;
var
c: byte;
br: byte;
bpos:byte = 0;
plen:word = 0;
label have_pkt;
label trunc;
label tmout;
label intr;
begin
    if timeout > 0 then timerstart(timer_no,timeout);
    repeat
        br:=rs232_cnt;
        if br > 0 then begin
            c := rs232_buf[rs232_pos - 1];
                if c = pb_ack then begin
                    blockread(f, _wbuf,br);
                    blockwrite(f,pbc_ack,1);
                    bpos:=0;
                    plen:=0;
                end else begin
                    blockread(f, _wbuf[bpos],br);
                    if c = pb_sep then dec(br);
                    inc(bpos,br);
                    if bpos = blocksize then begin
                        if timeout > 0 then timerstart(timer_no,timeout);
                        if (plen + bsize) > pb_mtu then begin
                            pkt_get := pb_mtuerr;
                            exit;
                        end;
                        dec_block(pkt.data,_wbuf,plen,bpos);
                        blockwrite(f,pbc_ack,1);
                        inc(plen, bsize);
                        bpos := 0;
                    end;
                    if c = pb_sep then begin
                        if bpos > 0 then begin
                            if bpos < pb_mcn then goto trunc;
                            if (plen + bpos - pb_mcn) > pb_mtu then begin
                                pkt_get := pb_mtuerr;
                                exit;
                            end;
                            dec_block(pkt.data, _wbuf, plen, bpos);
                            blockwrite(f,pbc_ack,1);
                            if timeout > 0 then timerstart(timer_no,timeout);
                        end;
                        pkt.len := plen + bpos - pb_mcn;
                        bpos := 0;
                        plen := 0;
                        goto have_pkt;
                    end;
                 end;
        end;
        if (timeout > 0) and timerout(timer_no) then goto tmout;
        if (kbbuf_cnt <> 0) then goto intr;
    until false;
have_pkt:
    pkt_get := pb_ok;
    if swap(pkt.ip.len) = pkt.len then exit;
trunc:
    pkt_get := pb_trunc;
    exit;
tmout:
    pkt_get := pb_tmout;
    exit;
intr:
    pkt_get := pb_intr;
end;

{ example: respond to ICMP echo }
procedure echo_respond(var pkt: ippkt);
var
tmpaddr:ipaddr;
begin
    with pkt do begin
        if ip.proto = ipproto_icmp then begin
            if icmp.mtype = icmp_mtype_echo then begin
                icmp.mtype := icmp_mtype_echoreply;
                { swap src and dst address }
                tmpaddr := ip.dst;
                ip.dst := ip.src;
                ip.src := tmpaddr;
                { compute UDP checksum }
                icmp.csum := 0;
                icmp.csum := pkt_checksum(payload, 0, len - ip_hdrlen);
                pkt_send(pkt);
            end;
        end;
    end;
end;

procedure pkt_init(mip: ipaddr);
{$IFDEF PBNET_GUI}
var
    nch: gc = (32,48,56,48,32,0 ,223,207,199,207,223,0, 8,24,56,24,8,0 ,247,231,199,231,247,0, 0,0,26,203,26,0);
    nch1:gc1= (0,0,0,0,192,192, 0,0,0,0,24,24, 0,0,0,0,3,3);
{$ENDIF}
begin
    my_ip := ip;
    rxcnt :=0;
    txcnt :=0;
{$IFDEF PBNET_GUI}
    ch := nch;
    ch1 :=nch1;
    gotoxy(1,1); write ('               ');
    gotoxy(1,1); write (my_ip[0],'.',my_ip[1],'.',my_ip[2],'.',my_ip[3]);
    gotoxy(rxcnt_pos,1); write(#1,'00000',#3,'00000 ',#5);
    gotoxy(1,2);
{$ENDIF}
    assign(f,'COM:7N81NNN.NN'); { open serial port }
    rewrite(f,1); reset(f,1); { set 1-byte block size for reading and writing }

end;

begin

    pkt_init(ip);

    writeln('Receiving (any key to stop)');

    repeat
        res := pkt_get(ipkt, 300);
        if res = pb_ok then begin
            echo_respond(ipkt);
        end;
        pkt_done;
        if res = pb_tmout then writeln('timeout');
    until kbbuf_cnt > 0;

    close(f);
    clrscr;
end.

