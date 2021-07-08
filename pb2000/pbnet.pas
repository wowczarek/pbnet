{$D-}
{$DEFINE PBNET_GUI}
{$UNDEF PBNET_GUI}
program pbnet;
uses crt;
const
pb_mtu = 1500;
pb_mcn = $04;
pb_mc = pb_mcn - 1;
blocksize = 190;
bsize = blocksize - pb_mcn;
pb_ok =    0;
pb_tmout = 1;
pb_intr  = 2;
pb_trunc = 3;
pb_corrupt = 4;
pb_mtuerr = 5;
pb_sep = $00;
pb_ack = $01;
pb_stx = $02;
pb_rtx = $03;
ipproto_zero = 0;
ipproto_icmp = 1;
ipproto_udp  = 17;
ipproto_tcp  = 6;
ipproto_offset = 9;
ip_hdrlen = 20;
icmp_hdrlen = 8;
udp_hdrlen = 8;
icmp_mtype_echo = 8;
icmp_mtype_echoreply = 0;
rs232_baddr = $08e2;
{$IFDEF PBNET_GUI}
rxcnt_pos = 19;
txcnt_pos = 25;
{$ENDIF}
timer_no = 2;
type
socket = record
proto: byte;
state: byte;
sport: word;
dport: word;
end;
ipaddr = array [0..3] of byte;
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
icmp_hdr = record
mtype: byte;
mcode: byte;
csum: word;
id: word;
seq: word;
payload: array [0..1471] of byte;
end;
udp_hdr = record
sport: word;
dport: word;
len: word;
csum: word;
payload: array [0..1471] of byte;
end;
ippkt = record
len: word;
case :boolean of
false:  (data: array [0..1499] of byte;);
true:   (
ip: ip_hdr;
case :byte of
0:  (payload: array [0..1479] of byte;);
1:  (icmp:    icmp_hdr;);
2:  (udp:     udp_hdr)
)
end;
{$IFDEF PBNET_GUI}
gc = array[0..29] of byte;
gc1 = array[0..17] of byte;
{$ENDIF}
var
f:file;
_cmap: array [0..255] of byte;
_wbuf: array [0..255] of byte;
pbc_sep: byte=pb_sep;
pbc_ack: byte=pb_ack;
pbc_stx: byte=pb_stx;
pbc_rtx: byte=pb_rtx;
rs232_cnt: byte absolute $08e0;
rs232_pos: byte absolute $08e1;
rs232_buf: array [0..255] of byte absolute $08e2;
kbbuf_cnt: byte absolute $09ec;
{$IFDEF PBNET_GUI}
ch: gc absolute $506;
ch1: gc1 absolute $560;
cntpad:string=' 00000';
lastx,lasty: byte;
{$ENDIF}
my_ip:ipaddr=(0,0,0,0);
res: byte;
rxcnt:word=0;
txcnt:word=0;
ipkt: ippkt;
ip: ipaddr = (10,99,99,2);
dnsip: ipaddr = (172,16,100,1);
{$IFDEF PBNET_GUI}
procedure cnt_update(tx:boolean);
var
sn: string;
begin
lastx:=wherex; lasty:=wherey;
byte(cntpad[0]):=6;
if tx then begin
cntpad[1]:=#3;
gotoxy(txcnt_pos,1);
str(txcnt,sn);
sound(600,2);
end else begin
cntpad[1]:=#1;
gotoxy(rxcnt_pos,1);
str(rxcnt,sn);
sound(800,2);
end;
dec(cntpad[0], length(sn));
write(cntpad,sn);
gotoxy(lastx,lasty);
end;
procedure mark_progress(tx:boolean);
begin
lastx:=wherex; lasty:=wherey;
if tx then begin
gotoxy(txcnt_pos,1); write(#4);
end else begin
gotoxy(rxcnt_pos,1); write(#2);
end;
gotoxy(lastx,lasty);
end;
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
function pkt_checksum(var data: array of byte; pos: word; len: word): word;
begin
inline(
$E8,$00,$A0,$88,$60,$02,$96,$40,$D1,$02,$02,$00,$8F,$66,$06,$81,
$64,$02,$B5,$11,$AB,$08,$88,$66,$08,$B1,$05,$88,$26,$B5,$83,$89,
$64,$02,$B7,$94,$81,$24,$B4,$0F,$29,$08,$0F,$69,$09,$88,$66,$08,
$B1,$05,$88,$26,$B5,$83,$9B,$46,$42,$0A,$04,$A0,$E6,$0A
);
end;
procedure enc_block(var pkt:array of byte; var cmap: array of byte; var wbuf: array of byte; pos:word; len:word);
begin
inline(
$9E,$16,$EA,$00,$E0,$A8,$08,$88,$60,$06,$96,$40,$96,$02,$8F,$6E,
$0E,$8F,$70,$10,$8F,$72,$12,$8F,$74,$14,$42,$0C,$04,$E2,$0E,$E0,
$E2,$0E,$E0,$E2,$0E,$E0,$E2,$0E,$E0,$E2,$0E,$E0,$E2,$0E,$E0,$E2,
$0E,$E0,$E2,$0E,$E0,$09,$2C,$B4,$9B,$96,$02,$82,$6A,$08,$42,$0F,
$00,$42,$10,$FF,$2B,$0C,$20,$7E,$0C,$01,$6F,$0C,$B1,$04,$02,$6F,
$0C,$01,$6C,$10,$B1,$04,$02,$70,$0C,$89,$2A,$B4,$98,$42,$0C,$03,
$01,$6C,$10,$B1,$17,$96,$44,$D1,$0A,$FF,$FF,$A3,$0A,$A3,$0A,$96,
$00,$82,$4A,$88,$6A,$08,$96,$2A,$D8,$B7,$67,$42,$10,$FC,$01,$6F,
$10,$B1,$10,$96,$44,$D1,$0C,$FC,$FD,$D1,$0E,$FE,$FF,$E1,$0C,$60,
$B7,$26,$96,$44,$D1,$0A,$04,$00,$88,$6A,$02,$96,$0A,$D1,$0D,$00,
$04,$42,$0B,$04,$2A,$0C,$01,$2C,$B0,$0A,$23,$0E,$08,$2D,$01,$6D,
$0B,$B0,$05,$08,$2E,$B4,$92,$D1,$0A,$04,$00,$88,$6A,$04,$96,$4A,
$96,$00,$82,$6A,$08,$42,$0F,$04,$42,$0D,$00,$2A,$0C,$01,$6C,$0F,
$B1,$0A,$82,$68,$04,$88,$68,$0C,$11,$6C,$08,$23,$0C,$89,$2A,$B4,
$95,$96,$16
);
end;
procedure dec_block(var pkt: array of byte; var wbuf: array of byte; pos: word; len: byte);
begin
inline(
$E8,$00,$C0,$9E,$12,$88,$60,$04,$96,$40,$96,$02,$EA,$08,$60,$07,
$68,$09,$B4,$12,$82,$6A,$02,$0F,$67,$07,$88,$6A,$06,$96,$2A,$D8,
$B7,$2F,$49,$06,$04,$2A,$0C,$07,$6C,$08,$B4,$05,$42,$8C,$00,$1B,
$07,$6C,$09,$B4,$05,$42,$8C,$01,$12,$07,$6C,$0A,$B4,$05,$42,$8C,
$02,$09,$07,$6C,$0B,$B4,$04,$42,$0C,$03,$23,$0C,$09,$26,$B4,$AA,
$96,$12
);
end;
procedure pkt_hold;
begin
blockwrite(f, pbc_stx, 1);
end;
procedure pkt_done;
begin
{$IFDEF PBNET_GUI}
lastx:=wherex; lasty:=wherey;
mark_proto(ipproto_zero);
gotoxy(lastx,lasty);
{$ENDIF}
blockwrite(f, pbc_rtx, 1);
end;
procedure pkt_send(var pkt:ippkt);
var
left: word;
pos: word = 0;
n: byte; i: byte;
begin
pkt.ip.len := swap(pkt.len);
pkt.ip.csum := 0;
pkt.ip.csum := pkt_checksum(pkt.data, 0, ip_hdrlen);
{$IFDEF PBNET_GUI}
mark_proto(pkt.ip.proto); mark_progress(true);
{$ENDIF}
left := pkt.len;
while left > bsize do begin
enc_block(pkt.data, _cmap,_wbuf,pos, bsize);
blockwrite(f,_wbuf,blocksize);
dec(left, bsize);
inc(pos,  bsize);
end;
enc_block(pkt.data, _cmap, _wbuf,pos, left);
inc(left,pb_mcn);
_wbuf[left] := pb_sep; inc(left);
blockwrite(f,_wbuf,left);
inc(txcnt);
{$IFDEF PBNET_GUI}
cnt_update(true);
mark_proto(ipproto_zero);
{$ENDIF}
end;
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
procedure echo_respond(var pkt: ippkt);
var
tmpaddr:ipaddr;
begin
with pkt do begin
if ip.proto = ipproto_icmp then begin
if icmp.mtype = icmp_mtype_echo then begin
icmp.mtype := icmp_mtype_echoreply;
tmpaddr := ip.dst;
ip.dst := ip.src;
ip.src := tmpaddr;
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
assign(f,'COM:7N81NNN.NN');
rewrite(f,1); reset(f,1);
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
