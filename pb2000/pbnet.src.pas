{ Copyright (c) 2021, Wojciech Owczarek }
{ All rights reserved }
{ BSD 2-clause licence-see LICENSE in the top directory }

{$D-}
program pbnet;
uses crt;
const 
    { global settings }
    cfg_file='pbnet.cfg';
    def_baud='7'; { 9600 }
    { protocol properties }
    pb_mtu=1500;
    pb_mcn=4; { max control character=number of reserved characters }
    pb_mc=pb_mcn-1; { mcn minus 1 (so we can use > not >=) }
    def_blocksize=224; { block size }
    { result constants }
    pb_ok=   0;
    pb_tmout=1;
    pb_intr =2;
    pb_trunc=3;
    pb_corrupt=4;
    pb_mtuerr=5;
    pb_unexp=6;
    { control characters }
    pb_sep=0;
    pb_ack=1;
    pb_stx=2;
    pb_rtx=3;
    { IP protocol numbers }
    ipproto_zero=0;
    ipproto_icmp=1;
    ipproto_udp =17;
    ipproto_tcp =6;
    { location of the protocol field in IPv4 header }
    ipproto_offset=9;
    { header lengths }
    ip_hdrlen=20;
    icmp_hdrlen=8;
    udp_hdrlen=8;
    { ICMP types }
    icmp_mtype_echo=8;
    icmp_mtype_echoreply=0;
    { other socket constants }
    port_any=0;
    ephem_minport=16385; { lowest ephemeral port }
    ephem_maxrand=65000 - ephem_minport; { random count for ephemeral port }
    { other internal constants }
    rs232_baddr=$08e2;
    timer_no=2;
type
    { an IP address - better this than a longint, really }
    ipaddr=array [0..3] of byte;
    { a socket descriptor }
    socket=record
        state:byte; { state (for TCP) }
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
                    2:{ variant 3:UDP + payload    } (udp:    udp_hdr)
            )
    end;

var 
    f:file; { serial port file descriptor }
    _cmap:array [0..255] of byte; { symbol check map }
    _wbuf:array [0..255] of byte; { rs232 block work buffer for rx/tx + sep }
    { control characters as variables, so they can be sent as is}
    pbc_sep:byte=pb_sep;
    pbc_ack:byte=pb_ack;
    pbc_stx:byte=pb_stx;
    pbc_rtx:byte=pb_rtx;
    rs232_cnt:byte absolute $08e0; { # bytes in rs232 buffer }
    rs232_pos:byte absolute $08e1; { buffer write position }
    rs232_buf:array [0..255] of byte absolute $08e2;
    kbbuf_cnt:byte absolute $09ec; { # bytes in keyboard buffer }
    baud:char=def_baud;
    my_ip:ipaddr=(10,99,99,2); { my own ip address }
    dns_ip:ipaddr=(8,8,8,8);
    addr_any:ipaddr=(0,0,0,0);
    blocksize:byte=def_blocksize;
    checksums:boolean=true;
    bsize:byte;
    res:byte; { general result capture variable }
    { counters }
    ip_id:word=0;
    rxcnt:word=0;
    txcnt:word=0;

{ ======= Config file related routines }

{ trim [^a-Az-Z0-9] from both ends of a string }
function trim(var s:string):string;
var 
  i,l,spos,epos,tl:byte;
  c:char;
  r:string[64];
  label st;
  label en;
begin
  l:=length(s);trim:=s;
  if (l<=0) or (l >= 64) then exit;
  { search ltr until an alnum character is found }
  for i:=1 to l do begin
    c:=upcase(s[i]);
    if ((c>'0') and (c<'9')) or ((c>'A') and (c<'Z')) then begin
      spos:=i;goto st;
    end;
  end;
  { none found:exit }
  exit;
  st:
  { search rtl until an alnum character is found }
  for i:=l downto 1 do begin
    c:=upcase(s[i]);
    if ((c>'0') and (c<'9')) or ((c>'A') and (c<'Z')) then begin
      epos:=i;goto en;
    end;
  end;
  exit;
  en:
  { set string result to a substring spos...epos }
  tl:=epos-spos+1;byte(r[0]):=tl;move(s[spos],r[1],tl);trim:=r;
end;

{ parse a byte from string, return 0 when all parsed }
function parse_byte(var s:string;var b:byte):byte;
var p:byte;
begin
    val(s,b,p);
    parse_byte:=p;
end;

{ parse an IP address from string into ipaddr - no range checks! returns 0 if all 4 octets parsed }
function parse_ip(var v:string;var ip:ipaddr):byte;
var p,p1:byte;
    s:string;
    n,np:byte;
    i,j:byte;
begin
    parse_ip:=1;
    i:=0;j:=0;
    p:=pos('.',v);p1:=1;
    while (p>0) and (i<4) do begin
      p:=pos('.',v,p1);
      if p=0 then begin
        if i<3 then exit;
        s:=copy(v,p1,64);
      end else begin
        s:=copy(v,p1,p-p1);p1:=p+1;
      end;
        inc(i);
        if parse_byte(s,ip[j])=0 then inc(j);
    end;
    if j=4 then parse_ip:=0;
end;

{ process a key,value config entry }
procedure parsekv(var k:string;var v:string);
var ip:ipaddr;n:byte;
begin

  if k='ip' then begin
    if parse_ip(v,ip)=0 then my_ip:=ip;
  end;

  if k='dns' then begin
    if parse_ip(v,ip)=0 then dns_ip:=ip;
  end;

  if k='blocksize' then begin
    if (parse_byte(v,n)=0) and (n>16) then blocksize:=n;
  end;

  if k='checksums' then begin
        if upcase(v[1])='T' then checksums:=true;
        if upcase(v[1])='Y' then checksums:=true;
        if upcase(v[1])='F' then checksums:=false;
        if upcase(v[1])='N' then checksums:=false;
  end;

  if k='baud' then begin
        if v='9600' then baud:='7';
        if v='4800' then baud:='6';
        if v='2400' then baud:='5';
        if v='1200' then baud:='4';
        if v='600' then baud:='3';
        if v='300' then baud:='2';
        if v='150' then baud:='1';
        if v='75' then baud:='0';
  end;
end;

{$I-}
{ load config file and parse settings, returns 1 on any failure }
function readconfig :byte;
var
 i,l,p:byte;
 f:text;
 s,k,v:string;
label cont;
label en;
begin
  readconfig:=1;
  assign(f,cfg_file);
  if ioresult<>0 then goto en;
  reset(f);
  { read file line by line }
  while not eof(f) do begin
    readln(f,s);
    if ioresult<>0 then goto en;
    l:=length(s);
    if l<2 then goto cont;
    { split into k,v on '=' }
    p:=pos('=',s);
    if p>64 then goto en;
    if p=0 then goto cont;
    k:=copy(s,0,p-1);v:=copy(s,p+1,64);
    k:=trim(k);v:=trim(v);
    parsekv(k,v);
  cont:
  end;
  readconfig:=0;
en:
  close(f);
end;
{$I+}
{ ========== other routines follow ======== }
{ RFC1071 checksum, as used in IP header checksum, ICMP header checksum, UDP checksum, etc. }
function pkt_checksum(var data:array of byte;pos:word;len:word):word;
begin
{$I cksum.pas}
end;
{ encode a len-byte block within a packet into wbuf, using check map cmap, starting from offset pos }
procedure enc_block(var pkt:array of byte;var cmap:array of byte;var wbuf:array of byte;pos:word;len:word);
begin
{$I enc_bl.pas}
end;
{ decode a len-byte block into packet from wbuf, starting from offset pos }
procedure dec_block(var pkt:array of byte;var wbuf:array of byte;pos:word;len:byte);
begin
{$I dec_bl.pas}
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
procedure pkt_send(var pkt:ippkt);
var
left:word;
plen:word=0;
pos:word=0;
n:byte;i:byte;
begin

    { populate length fields and compute checksums }
    with pkt do begin
        plen:=l4_len;
        case ip.proto of
            ipproto_icmp:begin
                inc(plen, icmp_hdrlen);
                { compute ICMP checksum }
                icmp.csum:=0;
                icmp.csum:=pkt_checksum(payload, 0, plen);
            end;
            ipproto_udp:begin
                inc(plen,udp_hdrlen);
                udp.csum:=0;
                udp.len:=swap(plen);
            end;
        end;

        left:=plen+ip_hdrlen;
        if len>pb_mtu then exit; { don't send a packet that's too large }
        ip.len:=swap(left);
        { cheksum is zero while computing the header checksum }
        ip.csum:=0;
        ip.csum:=pkt_checksum(pkt.data, 0, ip_hdrlen);
    end;

    { encode and send block by block }
    while left>bsize do begin
        enc_block(pkt.data, _cmap,_wbuf,pos, bsize);
        blockwrite(f,_wbuf,blocksize);
        dec(left, bsize);
        inc(pos,  bsize);
    end;
    { handle the  / last block }
    enc_block(pkt.data, _cmap, _wbuf,pos, left);
    { end of packet:send pb_mcn+left+pb_sep }
    inc(left,pb_mcn);
    _wbuf[left]:=pb_sep;inc(left);
    blockwrite(f,_wbuf,left);
    inc(txcnt);
end;
{ wait for an IP packet until one received or key pressed or timeout, while responding to ICMP echo }
function pkt_get(var pkt:ippkt;timeout:word):byte;
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
            c:=rs232_buf[rs232_pos-1];
                if c=pb_ack then begin
                    blockread(f, _wbuf,br);
                    blockwrite(f,pbc_ack,1);
                    bpos:=0;
                    plen:=0;
                end else begin
                    blockread(f, _wbuf[bpos],br);
                    if c=pb_sep then dec(br);
                    inc(bpos,br);
                    if bpos=blocksize then begin
                        if timeout>0 then timerstart(timer_no,timeout);
                        if (plen+bsize)>pb_mtu then begin
                            pkt_get:=pb_mtuerr;
                            exit;
                        end;
                        dec_block(pkt.data,_wbuf,plen,bpos);
                        blockwrite(f,pbc_ack,1);
                        inc(plen, bsize);
                        bpos:=0;
                    end;
                    if c=pb_sep then begin
                        if bpos>0 then begin
                            if bpos<pb_mcn then goto trunc;
                            if (plen+bpos-pb_mcn)>pb_mtu then begin
                                pkt_get:=pb_mtuerr;
                                exit;
                            end;
                            dec_block(pkt.data, _wbuf, plen, bpos);
                            blockwrite(f,pbc_ack,1);
                            if timeout>0 then timerstart(timer_no,timeout);
                        end;
                        pkt_hold; { tell the other end to stop transmitting while we process }
                        pkt.len:=plen+bpos-pb_mcn;bpos:=0;plen:=0;
                        goto have_pkt;
                    end;
                 end;
        end;
        if (timeout>0) and timerout(timer_no) then goto tmout;
        if (kbbuf_cnt<>0) then goto intr;
    until false;
have_pkt:
    pkt_get:=pb_unexp;
    if longint(pkt.ip.dst)<>longint(my_ip) then exit; { not for me }
    pkt_get:=pb_ok;
    case pkt.ip.proto of
        ipproto_icmp:pkt.l4_len:=swap(pkt.ip.len)-icmp_hdrlen-ip_hdrlen; { come on ICMP, get a length field! }
        ipproto_udp :pkt.l4_len:=swap(pkt.udp.len)-udp_hdrlen;
    end;
    if swap(pkt.ip.len)=pkt.len then exit;
trunc:
    pkt_get:=pb_trunc;
    exit;
tmout:
    pkt_get:=pb_tmout;
    exit;
intr:
    pkt_get:=pb_intr;
end;

{ set basic field values in a new packet }
procedure pkt_prime(var pkt:ippkt);
begin
    { ver/ihl 45, ttl 64 }
    pkt.data := ($45,$00,$00,$00,$00,$00,$00,$00,64);
    pkt.ip.id:=swap(ip_id);
    inc(ip_id);
end;

{ respond to ICMP echo }
procedure echo_respond(var pkt:ippkt);
var
tmpaddr:ipaddr;
begin
    with pkt do begin
        if (ip.proto=ipproto_icmp) and (icmp.mtype=icmp_mtype_echo) then begin
            icmp.mtype:=icmp_mtype_echoreply;
            { swap src and dst address }
            tmpaddr:=ip.dst;
            ip.dst:=ip.src;
            ip.src:=tmpaddr;
            pkt_send(pkt);
        end;
    end;
end;

{ respond to a packet:send given UDP payload back where it came from (but reverse ports and source/dest) }
procedure udp_respond(var pkt:ippkt);
var
tmpaddr:ipaddr;
tmpport:word;
begin
    { swap src and dst address + port}
    with pkt do begin
        tmpaddr:=ip.dst;
        ip.dst:=ip.src;
        ip.src:=tmpaddr;
        tmpport:=udp.dport;
        udp.dport:=udp.sport;
        udp.sport:=tmpport;
    end;
    pkt_send(pkt);
end;

{ wait for data on UDP socket }
function udp_recvfrom(var pkt:ippkt;var sock:socket;timeout:word):byte;
var res:byte;
begin
    { wait for a packet }
    res:=pkt_get(pkt, timeout);
    { if no packet or error, return that }
    if res<>pb_ok then begin
        udp_recvfrom:=res;
        exit;
    end;
    udp_recvfrom:=pb_unexp;
    { check if we got what we wanted }
    if pkt.ip.proto<>ipproto_udp then begin { not udp }
        echo_respond(pkt); { maybe it's an echo request }
        exit;
    end;
    if (longint(sock.remote)>0) and (longint(pkt.ip.src)<>longint(sock.remote)) then exit; { not from the wanted host }
    if (sock.lport>port_any) and (sock.lport<>swap(pkt.udp.dport)) then exit; { not for the wanted port }
    udp_recvfrom:=pb_ok;
end;

{ send data (pkt.udp.payload of len pkt.l4_len) to socket }
procedure udp_sendto(var pkt:ippkt;var sock:socket);
begin
    { 'prime' the packet with basic header fields }
    pkt_prime(pkt);
    { populate IP fields }
    pkt.ip.src:=my_ip;
    pkt.ip.dst:=sock.remote;
    pkt.ip.proto:=ipproto_udp;
    pkt.udp.dport:=swap(sock.rport);
    { if we don't have a source port, randomise it and assign it }
    if sock.lport=0 then begin
        sock.lport:=ephem_minport+random(ephem_maxrand);
    end;
    pkt.udp.sport:=swap(sock.lport);
    pkt_send(pkt);
end;

{ shut down and clean up }
procedure pbn_shutdown;
begin
    pkt_hold; { tell the host to stop transmitting }
    close(f);
end;
{ initialise pbnet }
procedure pbn_init;
var r:byte;
comstr:string;
begin
    { initialise, erm, things }
    bsize:=blocksize-pb_mcn;
    rxcnt:=0;
    txcnt:=0;
    { load the config }
    r:=readconfig;
    comstr:='COM:?N81NNN.NN';
    comstr[5]:=baud;
    assign(f,comstr); { open serial port }
    rewrite(f,1);reset(f,1); { set 1-byte block size for reading and writing }
    randomize; { ooh, InfoSec! }
    ip_id:=random($FFFF); { ooh, fancy! }
    pkt_ready; { send RTX }
end;

procedure icmp_test;
var pkt:ippkt;
begin
    pbn_init;
    repeat
        res:=pkt_get(pkt, 300);
        if res=pb_ok then begin
            echo_respond(pkt);
        end;
        pkt_ready;
    until res=pb_intr;
    pbn_shutdown;
end;

procedure udp_test;
var
pkt:ippkt;
sock:socket;
res:byte;
begin
    pbn_init;
    sock.rport:=port_any;
    sock.lport:=7;
    sock.remote:=addr_any;
    repeat
        res:=udp_recvfrom(pkt, sock, 300);
        if res=pb_ok then begin
            writeln('got udp, ',pkt.l4_len,' bytes');
            udp_respond(pkt);
        end;
        pkt_ready;
    until res=pb_intr;
    pbn_shutdown;
end;

begin
    writeln('Receiving (any key to stop)');
    udp_test;
    clrscr;
end.

