{ Copyright (c) 2021, Wojciech Owczarek }
{ All rights reserved }
{ BSD 2-clause licence - see LICENSE in the top directory }
{$D-}
unit pbnet;
uses crt; { only for timerxxx functions }
const 
    { protocol properties }
    pb_mcn=4; { max control character=number of reserved characters }
    pb_mc=pb_mcn-1; { mcn minus 1 (so we can use > not >=) }
    min_blsize=16; { minimum block size }
    { control characters }
    pb_sep=0;
    pb_ack=1;
    pb_stx=2;
    pb_rtx=3;
    { location of the protocol field in IPv4 header }
    ipr_off=9;
    { other internal constants }
    eph_min=49152; { lowest ephemeral port, IANA }
    eph_maxr=65535 - eph_min; { random count for ephemeral port }
    timer_no=2;
    cfg_offset=17; { offset of config data from top of the dummy __settings proc }
var 
    f:file; { serial port file descriptor }
    _init:boolean=false; { pbnet initialised }
    { originally we only had cmap[0..255], now reduced to 0..31 + smap }
    _cmap:array [0..31] of byte; { symbol check bit map }
    _smap:array [0..7] of byte = (128,64,32,16,8,4,2,1); { bit shift map, because there is no 1+ bit shift op }
    _wbuf:array [0..256] of byte; { rs232 block work buffer for rx/tx + sep }
    { control characters as variables, so they can be sent as is}
    pbc_sep:byte=pb_sep;pbc_ack:byte=pb_ack;pbc_stx:byte=pb_stx;pbc_rtx:byte=pb_rtx;
    rs232_cnt:byte absolute $08e0; { # bytes in rs232 buffer }
    rs232_pos:byte absolute $08e1; { buffer write position }
    rs232_buf:array [0..255] of byte absolute $08e2; { rs232 buffer location }
    kbbuf_cnt:byte absolute $09ec; { # bytes in keyboard buffer }
    bsize:byte;
    { counters }
    ip_id:word=0;
    { default config - not exposed in the unit }
    dcfg: ^pbn_cfg;
{ ======= Utility routines }

{ strerror(errno) }
function strerr(e:shortint): string;
var se:string[8];
begin
    case e of
        e_nomine:   se:='E_NOMINE';
        e_arg:      se:='E_ARG';
        e_init:     se:='E_INIT';
        e_err:      se:='E_ERR';
        e_unxp:     se:='E_UNXP';
        e_mtu:      se:='E_MTU';
        e_crc:      se:='E_CRC';
        e_trunc:    se:='E_TRUNC';
        e_intr:     se:='E_INTR';
        e_tmo:      se:='E_TMO';
        e_ok:       se:='E_OK';
        de_formerr: se:='FORMERR';
        de_servfail:se:='SERVFAIL';
        de_nxdomain:se:='NXDOMAIN';
        de_notimp:  se:='NOTIMP';
        de_refused: se:='REFUSED';
        de_notzone: se:='NOTZONE';
        else str(e,se);
    end;
    strerr:=se;
end;

{ strtok_r: find next token in ts, separated with c, starting from position _pos, write token start(st) and length (tl), return false if no more tokens }
function next_tok(var ts:string; c:char; var _pos:byte; var st:byte; var tl:byte):boolean;
var p,l:byte;
begin
    next_tok:=false;
    l:=byte(ts[0]);
    if _pos > l then exit;
    p:=pos(c,ts,_pos);
    if p=0 then p:=l+1;
    tl:=p-_pos; st:=_pos; _pos:=p+1;
    next_tok:=true;
end;

{ parse an IPv4 address from string into ipaddr - no range checks! returns true if all 4 octets parsed }
function ipaddr_parse(var s:string;var a:ipaddr):boolean;
var n,p,l:byte;
    p1:byte=1;
    os:string;
    i,j:byte;
begin
    ipaddr_parse:=false;
    if not (length(s) in [7..15]) then exit; { shortest valid is 'n.n.n.n'=3+4*1=7, longest is 3+4*3=15 }
    i:=0;j:=0;
    while next_tok(s,'.',p1,p,l) and (l>0) and (i<4) do begin
        os:=copy(s,p,l);
        val(os,a[j],n); if n=0 then inc(j) else exit;
        inc(i);
    end;
    if j=4 then ipaddr_parse:=true;
end;

{ IPv4 address to string }
function ipaddr_str(var a:ipaddr):string;
var ips_r:string[15]='';
ips_o:string[3];
i:byte;
begin
    for i:=0 to 3 do begin
        str(a[i],ips_o);
        ips_r:=ips_r+ips_o;
        if i < 3 then ips_r:=ips_r+'.';
    end;
    ipaddr_str:=ips_r;
end;

{ process a key,value config entry }
function pbn_set(sk: string; v: string):boolean;
var tip:ipaddr;n:integer;m:byte;emsg:string[24];c:char;w:word=0;k:char;
era:string[14]='outside range ';
label er;
begin
  k:=sk[1];
  pbn_set:=true;
  emsg:='unsupported value';
  case k of
  'i': begin
    if not ipaddr_parse(v,tip) then goto er;
    cfg^.ip:=tip;
  end;
  'd': begin
    if not ipaddr_parse(v,tip) then goto er;
    cfg^.dns:=tip;
  end;
  'b': begin
    val(v,n,m);
    if (m=0) and (n>=min_blsize) and (n<=dcfg^.blsize) then cfg^.blsize:=n
    else begin
        emsg:=era+'(16..224)';
        goto er;
    end;
  end;
  'c': begin
        c:=upcase(v[1]);
        if c in ['T','Y'] then cfg^.csums:=true
        else if c in ['F','N'] then cfg^.csums:=false
        else goto er;
  end;
  'r': begin
        if not (v[1] in ['0'..'7']) then goto er;
        cfg^.baud := v[1];
  end;
  't': begin
    val(v,n,m);
    if (m=0) and (n>0) and (n<256) then cfg^.ttl:=n
    else begin
        emsg:=era+'(1..255)';
        goto er;
    end;
  end;
  's': begin
        cfg^.search := copy(v,1,30);
  end;
  else begin emsg:='unknown setting'; goto er; end;
end;

exit;

er:
  pbn_set:=false;
  writeln('pbn_set: ',k,'=',v,': ',emsg);
end;

{ this is a storage area for our settings, the included assembly }
{ is simply a series of DBs with a jump around it - we write settings to this }
procedure __settings;
begin
{$I settings.pas}
end;

{ set settings to defaults }
procedure pbn_defaults;
begin
    move(dcfg^, cfg^, sizeof(pbn_cfg));
end;

{ ========== other routines follow ======== }

{ RFC1071 checksum, as used in IP header checksum, ICMP header checksum, UDP checksum, etc. }
function pkt_checksum(var data:array of byte;pos:word;len:word):word;
begin
{$I cksum.pas}
end;
{ encode a len-byte block within a packet into wbuf, using check map cmap, starting from offset pos }
procedure enc_block(var pkt:array of byte;var cmap:array of byte;var wbuf:array of byte;pos:word;len:word;var smap:array of byte);
begin
{$I enc_bl.pas}
end;
{ decode a len-byte block into packet from wbuf, starting from offset pos }
procedure dec_block(var pkt:array of byte;var wbuf:array of byte;pos:word;len:byte);
begin
{$I dec_bl.pas}
end;
{ compare two quads of bytes }
function quad_eq(var qa:array of byte;var qb:array of byte): boolean;
begin
{$I quad_eq.pas}
end;

{ don't send us anything for now }
procedure pkt_hold;
begin
    if _init then blockwrite(f, pbc_stx, 1); { stop transmission }
end;

{ call this once a packet has been handled or responded to (resume transmision) }
procedure pkt_ready;
begin
    if _init then blockwrite(f, pbc_rtx, 1); { resume transmission }
end;

{ send a raw IP packet down the pipe }
function pkt_send(var pkt:ippkt;timeout:word):shortint;
var
left:word;
plen:integer=0;
pos:word=0;
n: char;
label etmo; label eok;
begin
    if not _init then begin;
        pkt_send:=e_init;
        exit;
    end;
    { do not allow sending to self or same source and destination }
    if quad_eq(pkt.ip.dst, cfg^.ip) or quad_eq(pkt.ip.src,pkt.ip.dst) then begin
        pkt_send:=e_arg; exit;
    end;
    pkt_send:=e_ok;
    { populate length fields and compute checksums }
    with pkt do begin
        plen:=l4_len;
        case ip.proto of
            ipr_icmp:begin
                inc(plen, icmp_hlen);
                { compute ICMP checksum, unless we were given one }
                if icmp.csum=0 then icmp.csum:=pkt_checksum(payload, 0, plen);
            end;
            ipr_udp:begin
                inc(plen,udp_hlen);
                udp.csum:=0;
                udp.len:=swap(plen);
            end;
            else begin { what kind of sorcery is this?! }
                plen := len - ip_hlen;
            end;
        end;
        inc(plen, ip_hlen);
        left:=word(plen);
        if left>cfg^.mtu then begin
            pkt_send:=e_mtu;
            exit; { don't send a packet that's too large }
        end;
        ip.len:=swap(left);
        { cheksum is zero while computing the header checksum }
        ip.csum:=0;
        ip.csum:=pkt_checksum(pkt.data, 0, ip_hlen);
    end;

    { encode and send block by block }
    while left>bsize do begin
        enc_block(pkt.data, _cmap,_wbuf,pos, bsize,_smap);
        blockwrite(f,_wbuf,cfg^.blsize);
        dec(left, bsize);
        inc(pos,  bsize);
    end;
    { handle the  / last block }
    enc_block(pkt.data, _cmap, _wbuf,pos, left,_smap);
    { end of packet:send pb_mcn+left+pb_sep }
    inc(left,pb_mcn);
    _wbuf[left]:=pb_sep;inc(left);
    blockwrite(f,_wbuf,left);
    inc(txcnt);
    { wait for ACK }
    if timeout>0 then timerstart(timer_no,timeout);
    repeat
        if (rs232_cnt>0) and (rs232_buf[rs232_pos-1]=pb_ack) then goto eok;
        if (timeout>0) and timerout(timer_no) then goto etmo;
        if kbbuf_cnt<>0 then case readkey of
            { consume next byte from a 2-byte keycode }
            #0: n:=readkey;
            { exit on BRK }
            #3: begin pkt_send := e_intr; exit; end;
        end;
    until false;
eok:
    blockread(f,_wbuf,1);
    exit;
etmo:
    pkt_send:=e_tmo;
    exit;
end;
{ wait for an IP packet until one received or key pressed or timeout }
function pkt_get(var pkt:ippkt;timeout:word):shortint;
var
c, br: byte;
n: char;
bpos:byte=0;
plen:word=0;
label have_pkt;
label tmout;
label intr;
begin
    if not _init then begin;
        pkt_get:=e_init;
        exit;
    end;
    if timeout>0 then timerstart(timer_no,timeout);
    repeat
        br:=rs232_cnt;
        if br>0 then begin
            { last byte in the buffer. this can be fragile while still receiving, but we only use this to check for SEP where no more data is received }
            c:=rs232_buf[rs232_pos-1];
            blockread(f, _wbuf[bpos],br);
            if c=pb_sep then dec(br);
            { restart the timer on first byte, to prevent timing out once we started receiving }
            if (bpos=0) and (timeout>0) then timerstart(timer_no,timeout);
            inc(bpos,br);
            { assembled a full block or got SEP }
            if (bpos=cfg^.blsize) or (c=pb_sep) then begin
                if(bpos>0) then begin
                    if (plen+bpos-pb_mcn)>cfg^.mtu then begin
                        pkt_get:=e_mtu;
                        exit;
                    end;
                    { decode the block or remaining data and acknowledge }
                    dec_block(pkt.data,_wbuf,plen,bpos);
                    blockwrite(f,pbc_ack,1);
                    inc(plen, bpos); dec(plen,pb_mcn);
                end;
                bpos:=0;
                if c=pb_sep then begin
                    pkt_hold; { tell the other end to stop transmitting while we process }
                    pkt.len:=plen;plen:=0;
                    goto have_pkt;
                end;
            end;
        end;
        if (timeout>0) and timerout(timer_no) then goto tmout;
        if kbbuf_cnt<>0 then case readkey of
            { consume next byte from a 2-byte keycode }
            #0: n:=readkey;
            { exit on BRK }
            #3: goto intr;
        end;
    until false;
have_pkt:
    pkt_get:=e_nomine;
    if not quad_eq(pkt.ip.dst, cfg^.ip) then exit; { not for me }
    pkt_get:=e_ok;
    { update l4_len based on protocol }
    case pkt.ip.proto of
        ipr_icmp:pkt.l4_len:=swap(pkt.ip.len)-icmp_hlen-ip_hlen; { come on ICMP, get a length field! }
        ipr_udp :pkt.l4_len:=swap(pkt.udp.len)-udp_hlen;
        ipr_tcp :pkt.l4_len:=swap(pkt.ip.len)-(swap(pkt.tcp.doff) shl 2);
    end;
    if swap(pkt.ip.len)=pkt.len then exit;
    pkt_get:=e_trunc;
    exit;
tmout:
    pkt_get:=e_tmo;
    exit;
intr:
    pkt_get:=e_intr;
end;

{ set basic field values in a new packet }
procedure pkt_prime(var pkt:ippkt);
begin
    with pkt.ip do begin
        ver_ihl:=$45; { ver 4, ihl 5 dwords }
        dscp_ecn:=0; { clear }
        fl_foff:=$0040; { set DF flag (swapped byte order) }
        ttl:=cfg^.ttl;
        id:=swap(ip_id);
    end;
    inc(ip_id);
end;

{ respond to ICMP echo }
function echo_respond(var pkt:ippkt; timeout:word): shortint;
var
tmpaddr:ipaddr;
cs:word;
begin
    with pkt do begin
        if (ip.proto=ipr_icmp) and (icmp.mtype=icmp_mecho) then begin
            icmp.mtype:=icmp_mechoreply;
            { swap src and dst address }
            tmpaddr:=ip.dst;
            ip.dst:=ip.src;
            ip.src:=tmpaddr;
            { add 0x0800 to checksum and extra 1 on carry, that's it. still slow because of the swaps. }
            cs:=swap(icmp.csum);
            inc(cs,$0800);
            if cs < swap(icmp.csum) then inc(cs);
            icmp.csum:=swap(cs);
            echo_respond:=pkt_send(pkt,timeout);
        end else echo_respond:=e_nomine;
    end;
end;

{ respond to a packet:send given UDP payload back to where it came from (but reverse ports and source/dest) }
function udp_respond(var pkt:ippkt;timeout:word):shortint;
var
taddr:ipaddr;
tmpport:word;
begin
    { swap src and dst address + port}
    with pkt do begin
        taddr:=ip.dst;
        ip.dst:=ip.src;
        ip.src:=taddr;
        tmpport:=udp.dport;
        udp.dport:=udp.sport;
        udp.sport:=tmpport;
    end;
    udp_respond:=pkt_send(pkt,timeout);
end;

{ wait for data on UDP socket and bind it to the first packet if rport=any an }
function udp_recv(var pkt:ippkt;var sock:socket;timeout:word):shortint;
var res:shortint;
begin
    { wait for a packet }
    res:=pkt_get(pkt, timeout);
    { if no packet or error, return that }
    if res<>e_ok then begin
        udp_recv:=res;
        exit;
    end;
    udp_recv:=e_unxp;
    { check if we got what we wanted }
    if pkt.ip.proto<>ipr_udp then begin { not udp }
        res:=echo_respond(pkt,timeout); { maybe it's an echo request }
        exit;
    end;
    if (sock.lport>port_any) and (sock.lport<>swap(pkt.udp.dport)) then exit; { not for the wanted port }
    udp_recv:=e_nomine; { dst port ok, but not for this socket - application can check this and handle pkt differently }
    if (not quad_eq(sock.dst,addr_any)) and (not quad_eq(pkt.ip.src,sock.dst)) then exit { not from the wanted host }
    else sock.dst := pkt.ip.src; { bind this socket to the remote host }
    if (sock.rport=port_any) then sock.rport := swap(pkt.udp.sport); { bind this socket to the remote source port }
    if swap(pkt.udp.sport) <> sock.rport then exit; { not from the wanted port }
    udp_recv:=e_ok;
end;

{ send data (pkt.udp.payload of len pkt.l4_len) to socket }
function udp_send(var pkt:ippkt;var sock:socket;timeout:word):shortint;
begin
    { 'prime' the packet with basic header fields }
    pkt_prime(pkt);
    { populate IP fields }
    pkt.ip.src:=cfg^.ip;
    pkt.ip.dst:=sock.dst;
    pkt.ip.proto:=ipr_udp;
    pkt.udp.dport:=swap(sock.rport);
    { if we don't have a source port, randomise it and assign it }
    if sock.lport=0 then begin
        sock.lport:=eph_min+random(eph_maxr);
    end;
    pkt.udp.sport:=swap(sock.lport);
    udp_send:=pkt_send(pkt,timeout);
end;

function tcp_connect(var pkt:ippkt;var sock:socket;timeout:word):shortint;
begin
end;

{ check a hostname string and append search domain if no dots }
function hname_check(var shost:string): boolean;
var i:byte; c:char;
begin
    hname_check:=false;
    if (length(shost)=0) or (shost='.') then exit;
    if pos('.',shost) = 0 then shost:=shost+'.'+cfg^.search;
    for i:=1 to length(shost) do if not (shost[i] in [ 'a'..'z','A'..'Z','0'..'9','-','.']) then exit;
    hname_check:=true;
end;
{ resolve shost into ip, using pkt packet }
{ note that we don't map any DNS structures, this all works on raw offsets, to save code }
function dns_resolve(shost:string;var rip:ipaddr;var pkt:ippkt;timeout: word; retr: byte):shortint;
var
id:word;
res: shortint;
anssect: boolean=false;
off:byte;
l,ps,ps1,retries:byte;
sock: socket;
rcount:word=0;
rdlength:word=0;
in_a:quad=(0,1,0,1); { IN A: class+type=0x01 }
label retry;
label rxretry;
label got_in_a;
label ezpz;
label dosect;
begin
    retries:=retr;
    { we don't support reverse lookups - if we got an IP, just parse that. Easy peasy. }
    if ipaddr_parse(shost,rip) then goto ezpz;
    { too short or malformed }
    dns_resolve:=e_arg;
    if not hname_check(shost) then exit;
    dns_resolve:=e_err;
retry:
    { if you've done this once in your life, you'll remember 0x0c and the 0xc00c pointer forever :-) }
    off := 12; { past DNS header - must be done here, because we return to this point on retry }
    id:=ip_id;
    { prepare the socket }
    sock.lport:=port_any;
    sock.rport:=53;
    sock.dst:=cfg^.dns;
    { build the query }
    with pkt.udp do begin
    { part 1: populate DNS header }
        { bytes 0..1 = ID, copy from IP id, could be whatever }
        payload[0]:=hi(id); payload[1]:=lo(id);
        { zeroise the rest of the header, set RD and question count 1 }
        fillchar(payload[3],9,0); payload[2]:=$01; payload[5]:=$01;
        { result is: }
            {  XXXX ID }
            {  0200 QR=0,OPCODE=0,AA=0,TC=0,RD=1,RA=0,Z=0,RCODE=0 }
            {  0001 QDCOUNT=1 }
            {  0000 0000 0000 ANCOUNT=0,NSCOUNT=0,ARCOUNT=0 }
    { part 2: populate the query section }
        { place every token/subdomain in the packet as a label (length byte+string) }
        ps1:=1;
        while next_tok(shost,'.',ps1,ps,l) do begin
            if l>0 then begin
                payload[off]:=l; inc(off);
                move(shost[ps],payload[off],l);
                inc(off,l);
            end;
        end;
        { add null label (root) }
        payload[off]:=0;
        { set query class/type to IN A }
        move(in_a,payload[off+1],4);
    end;
    { send the query, retry if need be }
    pkt.l4_len:=off+5; { 5 for root,type,class }
    res:=udp_send(pkt,sock,timeout);
    if res<>e_ok then begin
        if retries > 0 then begin
            dec(retries);
            goto retry;
        end;
        dns_resolve:=res;
        exit;
    end;
    { we do a separate run of retries for RX }
    retries:=retr * 2;
rxretry:
    pkt_ready; { tell the host we can receive stuff now }
    { wait for a response packet }
    res:=udp_recv(pkt,sock,timeout);
    { retry on error, unless e_intr, that always exits }
    if (retries>1) and (res<>e_ok) and (res<>e_intr) then begin
        dec(retries);
        goto rxretry;
    end;
    if retries = 0 then res := e_tmo;
    if res <> e_ok then begin
        dns_resolve := res;
        exit;
    end;
    { we have a response, hopefully it's what we asked for }
    with pkt.udp do begin
        { different ID, not what we wanted, or Q[R] bit not set, not a response }
        if (payload[0] <> hi(id)) or (payload[1]<>lo(id)) or (payload[2] < $80) then begin
            { request again }
            if retries>0 then goto rxretry;
            exit;
        end;
        { TC flag, truncated message }
        if (payload[2] and $02) > 0 then exit;
        { check RCODE and return it if non-zero }
        if (payload[3] and $0f) > 0 then begin
            dns_resolve:=payload[3] and $0f;
            exit;
        end;
        { check ANCOUNT first, no point going further if there are no answers }
        rcount:=(payload[6] shl 8)+payload[7];
        if rcount = 0 then exit; { there's no answer for the chemical dancers }
        { get QDCOUNT }
        rcount:=(payload[4] shl 8)+payload[5];
        off:=12; { skip header }
dosect:
        { skip QD section or search the AN section: first find the zero label }
        while rcount>0 do begin
            if off>pkt.l4_len then exit; { no peeking!!1! }
            { wait for a null or for a pointer }
            if (payload[off]=0) or (payload[off]>191) then begin
                if (payload[off]>191) then inc(off); { if it's a pointer, move past pointer marker }
                inc(off); { move past the zero or pointer offset }
                { AN section }
                if anssect then begin
                    { do we have an IN A ? }
                    if quad_eq(payload[off],in_a) then goto got_in_a;
                    { otherwise skip past type(2)+class(2)+ttl(4) }
                    inc(off,8); 
                    { get RDLENGTH, skip past RDLENGTH and RDATA (of RDLENGTH length) }
                    rdlength:=(payload[off] shl 8)+payload[off+1];
                    inc(off,rdlength+2);
                { QD section: skip past type(2)+class(2) }
                end else inc(off,4); 
                dec(rcount);
            end else inc(off); { move forward }
        end;
        { now switch to AN }
        if not anssect then begin
            anssect:=true;
            { ANCOUNT }
            rcount:=(payload[6] shl 8)+payload[7];
            goto dosect;
        end;
        { nothing found }
        exit;
got_in_a:
    { skip to RDATA }
    inc(off,10);
    move(payload[off],rip,4);
ezpz:
    dns_resolve:=e_ok;
    end;

end;

{ shut down and clean up }
procedure pbn_shutdown;
begin
    if not _init then exit;
    pkt_hold; { tell the host to stop transmitting }
    close(f);
    _init:=false;
end;
{ initialise pbnet }
function pbn_init:boolean;
begin
    pbn_init:=false;
    { initialise, erm, things }
    bsize:=cfg^.blsize-pb_mcn;
    rxcnt:=0; txcnt:=0;
    assign(f,'COM:'+cfg^.baud+'N81NNN.NN'); { open serial port }
    rewrite(f,1);reset(f,1); { set 1-byte block size for reading and writing }
    randomize; { ooh, InfoSec! }
    ip_id:=random($FFFF); { ooh, fancy! }
    pbn_init:=true;
    _init:=true;
end;
{ main section }
begin
    { initialise settings to where they live }
    cfg := pointer(word(&__settings) + cfg_offset);
    dcfg := pointer(word(&__settings) + cfg_offset + sizeof(pbn_cfg));
end.
