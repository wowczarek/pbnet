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
    ip_hdrlen=20;
    icmp_hdrlen=8;
    udp_hdrlen=8;
    { ICMP types }
    icmp_mtype_echo=8;
    icmp_mtype_echoreply=0;
    { DNS return codes (RCODE) }
    de_formerr=1;de_servfail=2;de_nxdomain=3;de_notimp=4;de_refused=5;de_notzone=9;
    { other socket constants }
    port_any=0;
    ephem_minport=16384; { lowest ephemeral port }
    ephem_maxrand=65000 - ephem_minport; { random count for ephemeral port }
    { other internal constants }
    def_ttl=64;
    rs232_baddr=$08e2;
    timer_no=2;
    kv_maxlen=127; { max key/value string length }
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
    _wbuf:array [0..256] of byte; { rs232 block work buffer for rx/tx + sep }
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
    _ttl:byte=def_ttl;
    my_ip:ipaddr=(10,99,99,2); { my own ip address }
    dns_ip:ipaddr=(8,8,8,8);
    addr_any:ipaddr=(0,0,0,0);
    search_domain:string='';
    blsize:byte=def_blsize;
    checksums:boolean=true;
    bsize:byte;
    { counters }
    ip_id:word=0;
    rxcnt:word=0;
    txcnt:word=0;

{ ======= Utility and config file related routines }

{ strerr(errno) }
function strerr(e:shortint): string;
var se:string;
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
    if ((c>'/') and (c<':')) or ((c>'@') and (c<='[')) or (c='-') then alnum:=true;
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

{ parse a byte from string, return 0 when all parsed }
function parse_byte(var sb:string;var b:byte):byte;
var p:byte;
begin
    val(sb,b,p);
    parse_byte:=p;
end;

{ parse an IP address from string into ipaddr - no range checks! returns 0 if all 4 octets parsed }
function parse_ip(var v:string;var tgt:ipaddr):byte;
var p,p1:byte;
    os:string;
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
        os:=copy(v,p1,kv_maxlen);
      end else begin
        os:=copy(v,p1,p-p1);p1:=p+1;
      end;
        inc(i);
        if parse_byte(os,tgt[j])=0 then inc(j);
    end;
    if j=4 then parse_ip:=0;
end;

{ process a key,value config entry }
function parsekv(var k:string;var v:string): boolean;
var tip:ipaddr;n:byte;
label er;
begin
  parsekv:=true;
  if k='ip' then begin
    if parse_ip(v,tip)=0 then my_ip:=tip
    else begin
        writeln(cfg_file,': ip=',v,': could not parse');
         goto er;
    end;
  end;
  if k='dns' then begin
    if parse_ip(v,tip)=0 then dns_ip:=tip
    else begin
        writeln(cfg_file,': dns=',v,': could not parse');
        goto er;
    end;
  end;
  if k='blsize' then begin
    if (parse_byte(v,n)=0) and (n>=min_blsize) and (n<=def_blsize) then blsize:=n
    else begin
        writeln(cfg_file,': blsize=',v,': outside range (',min_blsize,'..',def_blsize,')');
        goto er;
    end;
  end;
  if k='checksums' then begin
        if upcase(v[1])='T' then checksums:=true;
        if upcase(v[1])='Y' then checksums:=true;
        if upcase(v[1])='F' then checksums:=false;
        if upcase(v[1])='N' then checksums:=false;
  end;
  if k='baud' then begin
        if v='9600' then baud:='7' else
        if v='4800' then baud:='6' else
        if v='2400' then baud:='5' else
        if v='1200' then baud:='4' else
        if v='600' then baud:='3' else
        if v='300' then baud:='2' else
        if v='150' then baud:='1' else
        if v='75' then baud:='0' else begin
            writeln(cfg_file,': baud=',v,': unsupported value');
            goto er;
        end;
  end;
  if k='ttl' then begin
    if (parse_byte(v,n)=0) and (n>0) then _ttl:=n
    else begin
        writeln(cfg_file,': ttl=',v,': outside range (1..255)');
        goto er;
    end;
  end;

  if k='search' then search_domain:=v;

  exit;
er:
  parsekv:=false;
end;

{$I-}
{ load config file and parse settings, returns false on failure }
{ note: 'failure' is only a parse failure, the file does not need to exist }
function readconfig:boolean;
var
hvan: boolean=false;
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
            ipr_icmp:begin
                inc(plen, icmp_hdrlen);
                { compute ICMP checksum }
                icmp.csum:=0;
                icmp.csum:=pkt_checksum(payload, 0, plen);
            end;
            ipr_udp:begin
                inc(plen,udp_hdrlen);
                udp.csum:=0;
                udp.len:=swap(plen);
            end;
        end;

        left:=plen+ip_hdrlen;
        if left>pb_mtu then exit; { don't send a packet that's too large }
        ip.len:=swap(left);
        { cheksum is zero while computing the header checksum }
        ip.csum:=0;
        ip.csum:=pkt_checksum(pkt.data, 0, ip_hdrlen);
    end;

    { encode and send block by block }
    while left>bsize do begin
        enc_block(pkt.data, _cmap,_wbuf,pos, bsize);
        blockwrite(f,_wbuf,blsize);
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
                    if bpos=blsize then begin
                        if timeout>0 then timerstart(timer_no,timeout);
                        if (plen+bsize)>pb_mtu then begin
                            pkt_get:=e_mtu;
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
                                pkt_get:=e_mtu;
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
    pkt_get:=e_unxp;
    if longint(pkt.ip.dst)<>longint(my_ip) then exit; { not for me }
    pkt_get:=e_ok;
    case pkt.ip.proto of
        ipr_icmp:pkt.l4_len:=swap(pkt.ip.len)-icmp_hdrlen-ip_hdrlen; { come on ICMP, get a length field! }
        ipr_udp :pkt.l4_len:=swap(pkt.udp.len)-udp_hdrlen;
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
procedure echo_respond(var pkt:ippkt);
var
tmpaddr:ipaddr;
begin
    with pkt do begin
        if (ip.proto=ipr_icmp) and (icmp.mtype=icmp_mtype_echo) then begin
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
    pkt_send(pkt);
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
        echo_respond(pkt); { maybe it's an echo request }
        exit;
    end;
    if (longint(sock.remote)>0) and (longint(pkt.ip.src)<>longint(sock.remote)) then exit; { not from the wanted host }
    if (sock.lport>port_any) and (sock.lport<>swap(pkt.udp.dport)) then exit; { not for the wanted port }
    udp_recv:=e_ok;
end;

{ send data (pkt.udp.payload of len pkt.l4_len) to socket }
procedure udp_send(var pkt:ippkt;var sock:socket);
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
        sock.lport:=ephem_minport+random(ephem_maxrand);
    end;
    pkt.udp.sport:=swap(sock.lport);
    pkt_send(pkt);
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
{ note that we don't map any DNS structures, this all works on raw offsets, to save source code }
function dns_resolve(shost:string;var rip:ipaddr;var pkt:ippkt;timeout: word; retries: byte):shortint;
var
id:word;
res: shortint;
off:byte;
len,ps,ps1:byte;
sock: socket;
qdc:word=0;
anc:word=0;
rdlength:word=0;
label retry;
label rxretry;
label got_in_a;
label ezpz;
begin
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
        repeat
            ps:=pos('.',shost,ps1);
            if ps=0 then ps:=length(shost)+1;
            len:=ps-ps1;
            payload[off]:=len; inc(off);
            move(shost[ps1],payload[off],len);
            inc(off,len); ps1:=ps+1;
        until ps>length(shost);
        { add null label (root) + two zero words for type and class }
        fillchar(payload[off],5,0);
        { set query to IN A }
        inc(off,2); payload[off]:=1; { QTYPE=0x0001=A}
        inc(off,2); payload[off]:=1; { QCLASS=0x0001=IN }
        inc(off);
    end;
    { send the query }
    pkt.l4_len:=off;
    udp_send(pkt,sock);
rxretry:
    if retries>0 then dec(retries);
    pkt_ready; { tell the host that we can receive stuff now }
    { wait for a response packet }
    res:=udp_recv(pkt,sock,timeout);
    { look for more packets on error, unless e_intr }
    if (res<>e_ok) and (res<>e_intr) and (retries>0) then goto retry;
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
        { get QDCOUNT and ANCOUNT }
        qdc:=(payload[4] shl 8)+payload[5];
        anc:=(payload[6] shl 8)+payload[7];
        if anc = 0 then exit; { there's no answer for the chemical dancers }
        off:=12; { skip header }
        { skip QD/query section: first find the zero label }
        while qdc>0 do begin
            if off>pkt.l4_len then exit;
            { wait for a null or for a pointer }
            if (payload[off]=0) or (payload[off]>191) then begin
                if (payload[off]>191) then inc(off); { if it's a pointer, move past pointer marker }
                inc(off); { move past the zero or pointer offset }
                { then skip past type(2)+class(2) }
                inc(off,4); 
                dec(qdc);
            end else inc(off); { move forward }
        end;
        { now we should be in the AN section, but there could be CNAMEs - just keep skipping until we find IN A }
        while anc>0 do begin
            if off>pkt.l4_len then exit;
            { wait for a null or a pointer }
            if (payload[off]=0) or (payload[off]>191) then begin
                if (payload[off]>191) then inc(off); { if it's a pointer, move past pointer marker }
                inc(off); { move past the zero or pointer offset }
                { we have an IN A ! }
                if (payload[off]=0) and (payload[off+1]=1) and (payload[off+2]=0) and (payload[off+3]=1) then goto got_in_a;
                { otherwise skip past type(2)+class(2)+ttl(4) }
                inc(off,8); 
                { get RDLENGTH, skip past RDLENGTH and RDATA (of RDLENGTH length) }
                rdlength:=(payload[off] shl 8)+payload[off+1];
                inc(off,rdlength+2);
                dec(anc);
            end else inc(off); { move forward }
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
    rxcnt:=0;
    txcnt:=0;
    { load the config }
    if not readconfig then exit;
    assign(f,'COM:'+baud+'N81NNN.NN'); { open serial port }
    rewrite(f,1);reset(f,1); { set 1-byte block size for reading and writing }
    randomize; { ooh, InfoSec! }
    ip_id:=random($FFFF); { ooh, fancy! }
    pbn_init:=true;
end;

{ UDP echo test }
procedure udp_test;
var
pkt:ippkt;
sock:socket;
res:shortint;
begin
    if not pbn_init then exit;
    sock.rport:=port_any;
    sock.lport:=7;
    sock.remote:=addr_any;
    repeat
        res:=udp_recv(pkt, sock, 300);
        if res=e_ok then begin
            writeln('got udp, ',pkt.l4_len,' bytes');
            udp_respond(pkt);
        end;
        pkt_ready;
    until res=e_intr;
    pbn_shutdown;
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
    end;
    if not pbn_init then exit;
    writeln('looking up ',ps,'...');
    r:=dns_resolve(ps,ip,pkt,50,3);
    if r=e_ok then begin
        write(ps,' is: '); print_ipaddr(ip);
        writeln;
    end else writeln('dns_resolve error: ',strerr(r));
end;

begin
    dns_test;
end.

