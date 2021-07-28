{ Copyright (c) 2021, Wojciech Owczarek }
{ All rights reserved }
{ BSD 2-clause licence-see LICENSE in the top directory }

{$D-}
program pbnet;
uses crt; { only for timerxxx functions }
const 
    { global settings }
    cfg_file='pbnet.cfg';
    def_baud='7'; { 9600 }
    { protocol properties }
    pb_mtu=1500;
    pb_mcn=4; { max control character=number of reserved characters }
    pb_mc=pb_mcn-1; { mcn minus 1 (so we can use > not >=) }
    min_blsize=16; { minimum block size }
    def_blsize=224; { block size }
    { error constants }
    e_ok=0; e_tmo=-1; e_intr=-2; e_trunc=-3;
    e_crc=-4; e_mtu=-5; e_unxp=-6; e_err=-7;
    { control characters }
    pb_sep=0;
    pb_ack=1;
    pb_stx=2;
    pb_rtx=3;
    { IP protocol numbers }
    ipr_zero=0;
    ipr_icmp=1;
    ipr_udp =17;
    ipr_tcp =6;
    { location of the protocol field in IPv4 header }
    ipr_off=9;
    { header lengths }
    ip_hlen=20;
    icmp_hlen=8;
    udp_hlen=8;
    tcp_hlen=20;
    { ICMP types }
    icmp_mecho=8;
    icmp_mechoreply=0;
    { DNS return codes (RCODE) }
    de_formerr=1;de_servfail=2;de_nxdomain=3;de_notimp=4;de_refused=5;de_notzone=9;
    { TCP flags }
    tc_fin=1;tc_syn=2;tc_rst=4;tc_psh=8;tc_ack=16;
    { other socket constants }
    port_any=0;
    eph_min=49152; { lowest ephemeral port, IANA }
    eph_maxr=65535 - eph_min; { random count for ephemeral port }
    { other internal constants }
    def_ttl=64;
    rs232_baddr=$08e2;
    timer_no=2;
    kv_maxlen=127; { max key/value string length }
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
        remote:ipaddr; { remote address }
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
        id:word;
        seq:word;
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
var 
    f:file; { serial port file descriptor }
    { originally we only had cmap[0..255], now reduced to 0..31 + smap }
    _cmap:array [0..31] of byte; { symbol check bit map }
    _smap:array [0..7] of byte = (1,2,4,8,16,32,64,128);
    _wbuf:array [0..256] of byte; { rs232 block work buffer for rx/tx + sep }
    { control characters as variables, so they can be sent as is}
    pbc_sep:byte=pb_sep;pbc_ack:byte=pb_ack;pbc_stx:byte=pb_stx;pbc_rtx:byte=pb_rtx;
    rs232_cnt:byte absolute $08e0; { # bytes in rs232 buffer }
    rs232_pos:byte absolute $08e1; { buffer write position }
    rs232_buf:array [0..255] of byte absolute $08e2;
    kbbuf_cnt:byte absolute $09ec; { # bytes in keyboard buffer }
    baud:char=def_baud;
    _ttl:byte=def_ttl;
    my_ip:ipaddr=(10,99,99,2); { my own ip address }
    dns_ip:ipaddr=(9,9,9,9); { Quad9 - sod Google }
    addr_any:ipaddr=(0,0,0,0); { INADDR_ANY }
    search_domain:string='';
    blsize:byte=def_blsize;
    checksums:boolean=true;
    bsize:byte;
    { counters }
    ip_id:word=0;
    rxcnt:word=0;
    txcnt:word=0;

{ ======= Utility and config file related routines }

{ strerror(errno) }
function strerr(e:shortint): string;
var se:string[8];
begin
    case e of
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

{ check if c is an alphanumeric character (or a hyphen, because DNS) }
function alnum(c: char):boolean;
begin
    alnum:=false;
    c:=upcase(c);
    if (c in ['A'..'Z']) or (c in ['0'..'9']) or (c='-') then alnum:=true;
end;

{ trim [^a-Az-Z0-9-] from both ends of a string }
function trim(var astr:string):string;
var 
  i,l,spos,epos,tl:byte;
  r:string;
  label st;
  label en;
begin
  l:=length(astr);trim:=astr;
  if (l<=0) or (l >= kv_maxlen) then exit;
  { search ltr until an alnum character is found }
  for i:=1 to l do begin
    if alnum(astr[i]) then begin
      spos:=i;goto st;
    end;
  end;
  { none found:exit }
  exit;
  st:
  { search rtl until an alnum character is found }
  for i:=l downto 1 do begin
    if alnum(astr[i]) then begin
      epos:=i;goto en;
    end;
  end;
  exit;
  en:
  { set string result to a substring spos...epos }
  tl:=epos-spos+1;byte(r[0]):=tl;move(astr[spos],r[1],tl);trim:=r;
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

{ parse an IP address from string into ipaddr - no range checks! returns 0 if all 4 octets parsed }
function parse_ip(var v:string;var tgt:ipaddr):byte;
var n,p,l:byte;
    p1:byte=1;
    os:string;
    i,j:byte;
begin
    parse_ip:=1;
    if not (length(v) in [7..15]) then exit; { shortest valid is 'n.n.n.n'=3+4*1=7, longest is 3+4*3=15 }
    i:=0;j:=0;
    while next_tok(v,'.',p1,p,l) and (l>0) and (i<4) do begin
        os:=copy(v,p,l);
        val(os,tgt[j],n); if n=0 then inc(j) else exit;
        inc(i);
    end;
    if j=4 then parse_ip:=0;
end;

{ process a key,value config entry }
function parsekv(var k:string;var v:string): boolean;
var tip:ipaddr;n,m:byte;emsg:string[30];c:char;w:word=0;
label er;
begin
  parsekv:=true;
  if k='ip' then begin
    if parse_ip(v,tip)>0 then begin
         emsg:='could not parse';
         goto er;
    end;
    my_ip:=tip;
    exit;
  end;
  if k='dns' then begin
    if parse_ip(v,tip)>0 then begin
        emsg:='could not parse';
        goto er;
    end;
    dns_ip:=tip;
    exit;
  end;
  if k='blsize' then begin
    val(v,n,m);
    if (m=0) and (n>=min_blsize) and (n<=def_blsize) then begin blsize:=n; exit; end
    else begin
        emsg:='outside range (16..224)';
        goto er;
    end;
  end;
  if k='checksums' then begin
        c:=upcase(v[1]);
        if (c='T') or (c='Y') then checksums:=true;
        if (c='F') or (c='N') then checksums:=true;
        exit;
  end;
  if k='baud' then begin
        emsg:='unsupported value';
        val(v,w,n);
        if n > 0 then goto er;
        case w of
            9600:baud:='7';
            4800:baud:='6';
            2400:baud:='5';
            1200:baud:='4';
             600:baud:='3';
             300:baud:='2';
             150:baud:='1';
              75:baud:='0';
            else goto er;
        end;

        exit;
  end;
  if k='ttl' then begin
    val(v,n,m);
    if (m=0) and (n>0) then begin _ttl:=n; exit; end
    else begin
        emsg:='outside range (1..255)';
        goto er;
    end;
  end;

  if k='search' then search_domain:=v;

  exit;
er:
  parsekv:=false;
  writeln(cfg_file,': ',k,'=',v,': ',emsg);
end;
{ suppress I/O errors temporarily, so that the file not existing doesn't halt us }
{$I-}
{ load config file and parse settings, returns false on failure }
{ note: 'failure' is only a parse failure, the file does not need to exist }
function readconfig:boolean;
var
 i:byte=1;
 p:byte;
 lc:byte=1;
 f:text;
 s,k,v:string;
label cont;
label er;
label en;
begin
  readconfig:=true;
  assign(f,cfg_file);
  if ioresult<>0 then goto en;
  reset(f);
  { read file line by line }
  while not eof(f) do begin
    readln(f,s);
    if ioresult<>0 then goto en;
    repeat
        if s[i]=';' then goto cont; {skip line (comment) if ; is the first non-alnum byte }
        inc(i);
    until alnum(s[i]) or (i=length(s));
    s:=trim(s);
    if length(s)<3 then goto er;
    { split into k,v on '=' }
    p:=pos('=',s);
    if p>kv_maxlen then goto er;
    if p=0 then goto er;
    k:=copy(s,0,p-1);v:=copy(s,p+1,kv_maxlen);
    k:=trim(k);v:=trim(v);
    if not parsekv(k,v) then goto er;
  cont:
    inc(lc);
  end;
  close(f);
en:
    exit;
er:
  writeln(cfg_file,': error in line ',lc);
  readconfig:=false;
  close(f);
end;
{ restore halt on I/O errors}
{$I+}

{ ========== other routines follow ======== }

procedure print_ipaddr(var a:ipaddr);
begin
    write(a[0],'.',a[1],'.',a[2],'.',a[3]);
end;

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
function quad_eq(var ada:array of byte;var adb:array of byte): boolean;
begin
{$I quad_eq.pas}
end;

{ don't send us anything for now }
procedure pkt_hold;
begin
    blockwrite(f, pbc_stx, 1); { stop transmission }
end;

{ call this once a packet has been handled or responded to (resume transmision) }
procedure pkt_ready;
begin
    blockwrite(f, pbc_rtx, 1); { resume transmission }
end;


{ send an IP packet down the pipe }
function pkt_send(var pkt:ippkt;tout:word):shortint;
var
left:word;
plen:word=0;
pos:word=0;
n:byte;i:byte;
label etmo; label eok;
begin
    pkt_send:=e_ok;
    { populate length fields and compute checksums }
    with pkt do begin
        plen:=l4_len;
        case ip.proto of
            ipr_icmp:begin
                inc(plen, icmp_hlen);
                { compute ICMP checksum }
                icmp.csum:=0;
                icmp.csum:=pkt_checksum(payload, 0, plen);
            end;
            ipr_udp:begin
                inc(plen,udp_hlen);
                udp.csum:=0;
                udp.len:=swap(plen);
            end;
        end;

        left:=plen+ip_hlen;
        if left>pb_mtu then begin
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
        blockwrite(f,_wbuf,blsize);
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
    if tout>0 then timerstart(timer_no,tout);
    repeat
        if (rs232_cnt>0) and (rs232_buf[rs232_pos-1]=pb_ack) then goto eok;
        if (tout>0) and timerout(timer_no) then goto etmo;
    until kbbuf_cnt<>0;
    pkt_send:=e_intr;
    exit;
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
c:byte;
br:byte;
bpos:byte=0;
plen:word=0;
label have_pkt;
label trunc;
label tmout;
label intr;
begin
    if timeout>0 then timerstart(timer_no,timeout);
    repeat
        br:=rs232_cnt;
        if br>0 then begin
            { last byte in the buffer. this can be fragile while still receiving, but we only use this to check for SEP where no more data is received }
            c:=rs232_buf[rs232_pos-1];
            blockread(f, _wbuf[bpos],br);
            if c=pb_sep then dec(br);
            inc(bpos,br);
            { assembled a full block }
            if bpos=blsize then begin
                { restart the timer for next block }
                if timeout>0 then timerstart(timer_no,timeout);
                if (plen+bsize)>pb_mtu then begin
                    pkt_get:=e_mtu;
                    exit;
                end;
                { decode the block and acknowledge }
                dec_block(pkt.data,_wbuf,plen,bpos);
                blockwrite(f,pbc_ack,1);
                inc(plen, bsize);
                bpos:=0;
            end;
            { got SEP, end of packet }
            if c=pb_sep then begin
                { there is data to decode = block shorter than block size = last block }
                if bpos>0 then begin
                    if bpos<pb_mcn then goto trunc;
                    if (plen+bpos-pb_mcn)>pb_mtu then begin
                        pkt_get:=e_mtu;
                        exit;
                    end;
                    { decode the remaining data and acknowledge }
                    dec_block(pkt.data, _wbuf, plen, bpos);
                    blockwrite(f,pbc_ack,1);
                end;
                pkt_hold; { tell the other end to stop transmitting while we process }
                pkt.len:=plen+bpos-pb_mcn;bpos:=0;plen:=0;
                goto have_pkt;
            end;
        end;
        if (timeout>0) and timerout(timer_no) then goto tmout;
        if (kbbuf_cnt<>0) then goto intr;
    until false;
have_pkt:
    pkt_get:=e_unxp;
    if not quad_eq(pkt.ip.dst, my_ip) then exit; { not for me }
    pkt_get:=e_ok;
    { update l4_len based on protocol }
    case pkt.ip.proto of
        ipr_icmp:pkt.l4_len:=swap(pkt.ip.len)-icmp_hlen-ip_hlen; { come on ICMP, get a length field! }
        ipr_udp :pkt.l4_len:=swap(pkt.udp.len)-udp_hlen;
        ipr_tcp :pkt.l4_len:=swap(pkt.ip.len)-(swap(pkt.tcp.doff) shl 2);
    end;
    if swap(pkt.ip.len)=pkt.len then exit;
trunc:
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
        ttl:=_ttl;
        id:=swap(ip_id);
    end;
    inc(ip_id);
end;

{ respond to ICMP echo }
function echo_respond(var pkt:ippkt; t:word): shortint;
var
tmpaddr:ipaddr;
begin
    with pkt do begin
        if (ip.proto=ipr_icmp) and (icmp.mtype=icmp_mecho) then begin
            icmp.mtype:=icmp_mechoreply;
            { swap src and dst address }
            tmpaddr:=ip.dst;
            ip.dst:=ip.src;
            ip.src:=tmpaddr;
            echo_respond:=pkt_send(pkt,t);
        end else echo_respond:=e_unxp;
    end;
end;

{ respond to a packet:send given UDP payload back to where it came from (but reverse ports and source/dest) }
function udp_respond(var pkt:ippkt;t:word):shortint;
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
    udp_respond:=pkt_send(pkt,t);
end;

{ wait for data on UDP socket }
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
    if (not quad_eq(sock.remote,addr_any)) and (not quad_eq(pkt.ip.src,sock.remote)) then exit; { not from the wanted host }
    if (sock.lport>port_any) and (sock.lport<>swap(pkt.udp.dport)) then exit; { not for the wanted port }
    udp_recv:=e_ok;
end;

{ send data (pkt.udp.payload of len pkt.l4_len) to socket }
function udp_send(var pkt:ippkt;var sock:socket;t:word):shortint;
begin
    { 'prime' the packet with basic header fields }
    pkt_prime(pkt);
    { populate IP fields }
    pkt.ip.src:=my_ip;
    pkt.ip.dst:=sock.remote;
    pkt.ip.proto:=ipr_udp;
    pkt.udp.dport:=swap(sock.rport);
    { if we don't have a source port, randomise it and assign it }
    if sock.lport=0 then begin
        sock.lport:=eph_min+random(eph_maxr);
    end;
    pkt.udp.sport:=swap(sock.lport);
    udp_send:=pkt_send(pkt,t);
end;

function tcp_connect(var pkt:ippkt;var sock:socket;t:word):shortint;

begin
end;


{ clean up a hostname string and append search domain if no dots }
function hname_cleanup(var shost:string): boolean;
var i:byte;
begin
    hname_cleanup:=false;
    if (length(shost)=0) or (shost='.') then exit;
    shost:=trim(shost);
    if length(shost)=0 then exit;
    if pos('.',shost) = 0 then shost:=shost+'.'+search_domain;
    if length(shost)=0 then exit;
    for i:=1 to length(shost) do if not (alnum(shost[i]) or (shost[i]='.')) then exit;
    hname_cleanup:=true;
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
    if parse_ip(shost,rip)=0 then goto ezpz;
    { too short or malformed }
    dns_resolve:=e_unxp;
    if not hname_cleanup(shost) then exit;
    dns_resolve:=e_err;
retry:
    { if you've done this once in your life, you'll remember 0x0c and the 0xc00c pointer forever :-) }
    off := 12; { past DNS header - must be done here, because we return to this point on retry }
    id:=ip_id;
    { prepare the socket }
    sock.lport:=port_any;
    sock.rport:=53;
    sock.remote:=dns_ip;
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
    retries:=retr;
rxretry:
    pkt_ready; { tell the host that we can receive stuff now }
    { wait for a response packet }
    res:=udp_recv(pkt,sock,timeout);
    { retry on error, unless e_intr, that always exits }
    if (retries>1) and (res<>e_ok) and (res<>e_intr) then begin
        dec(retries);
        goto rxretry;
    end;
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
    pkt_hold; { tell the host to stop transmitting }
    close(f);
end;
{ initialise pbnet }
function pbn_init:boolean;
begin
    pbn_init:=false;
    { initialise, erm, things }
    bsize:=blsize-pb_mcn;
    rxcnt:=0; txcnt:=0;
    { load the config }
    if not readconfig then exit;
    assign(f,'COM:'+baud+'N81NNN.NN'); { open serial port }
    rewrite(f,1);reset(f,1); { set 1-byte block size for reading and writing }
    randomize; { ooh, InfoSec! }
    ip_id:=random($FFFF); { ooh, fancy! }
    pbn_init:=true;
end;

procedure dns_test;
var
pkt:ippkt;
r:shortint;
ps:string;
ip:ipaddr;
begin
    if (paramcount=0) then exit;
    ps:=paramstr(1);
    ps:=trim(ps);
    if ps='.' then begin
        writeln('malformed hostname: ',ps);
        exit;
    end;
    if not pbn_init then exit;
    writeln('looking up ',ps,'...');
    r:=dns_resolve(ps,ip,pkt,30,3);
    if r=e_ok then begin
        write(ps,' is: '); print_ipaddr(ip);
        writeln;
    end else writeln('dns_resolve error: ',strerr(r));
end;

begin
    dns_test;
end.

