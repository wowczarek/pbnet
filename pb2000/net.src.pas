{ Copyright (c) 2021, Wojciech Owczarek }
{ All rights reserved }
{ BSD 2-clause licence - see LICENSE in the top directory }
{$D-}
program net;
uses pbnet,crt;
var pkt:ippkt;
    ip:ipaddr;
    host:string;
    r:shortint=e_ok;
    snt,rcv:word;
    c:char;n:integer;m:byte;
    cnt:shortint=1;sz:word=32;intvl:word=10;
    sep:string[16]='----------------';
    tm0: word absolute $06a3; { where the timer value is kept by DL-Pascal - it's a3,a5,a7 for 3 timers }
{ keep responding to ICMP echo requests }
procedure icmp_resp;
begin
    snt:=0;rcv:=0;
    writeln('ICMP echo responder running');
    repeat
        pkt_ready;
        r:=pkt_get(pkt,30);
        if (pkt.ip.proto=ipr_icmp) and (pkt.icmp.mtype=icmp_mecho) then inc(rcv);
        if r=e_ok then r:=echo_respond(pkt,30);
        if r=e_ok then inc(snt);
    until r=e_intr;
    writeln(sep);writeln('totals: rx ',rcv,', tx ',snt);
end;
{ simple DNS query wrapper, we use globals for parameters }
procedure dns_query;
begin
    writeln('looking up ',host,'...');
    r:=dns_resolve(host,ip,pkt,30,5);
    if r=e_ok then begin
        write(host,' is: ', ipaddr_str(ip));
        writeln;
    end else writeln('dns_resolve error: ',strerr(r));
end;
{ no IP stack may exist without a 'ping' utility }
procedure ping(var ip: ipaddr; cnt: shortint; sz: word; intvl: word);
const iid=$3713; { because I'm twelve years old }
var sq,lastrepl,tmrcv: word; miss: boolean; pref: string[3];
begin
    miss := false;
    sq := 1; lastrepl := 0; snt := 0; rcv := 0;
    writeln('PING ',ipaddr_str(ip),' ',sz,'(',sz + 28,') B.');
    { prime the packet }
    pkt_prime(pkt);
    repeat
        {set source / dest address and protocol}
        with pkt.ip do begin
            src:=cfg^.ip; dst:=ip; proto:=ipr_icmp;
        end;
        { populate ICMP fields }
        with pkt.icmp do begin
            mtype:=icmp_mecho; mcode:=0; id:=iid; seq:=swap(sq); csum:=0;
            { fill the payload with FF, no need to be proper here }
            fillchar(pkt.icmp.payload, $ff, sz);
        end;
        pkt.l4_len:=sz;
        r:=pkt_send(pkt, 2*intvl); { send the packet with a timeout of 2 x interval }
        if r=e_ok then begin
            timerstart(0,$ffff); { arm timer for max value }
            inc(snt);
            pkt_ready;
            r:=pkt_get(pkt, 2*intvl );
            pkt_hold; { tell host to stop forwarding }
            if (r=e_ok) and quad_eq(pkt.ip.src,ip) and (pkt.ip.proto=ipr_icmp) and (pkt.icmp.mtype=icmp_mechoreply) and (pkt.icmp.id=iid) then begin
                tmrcv:=tm0; { grab current timer value }
                pref:='R ';
                if swap(pkt.icmp.seq) > sq then pref := '? ' { seq no greater than our last sent - this will fail on rollover and we accept this }
                else if swap(pkt.icmp.seq) < lastrepl then pref:='D ' { DUP - will also trigger on rollover }
                else begin inc(rcv); lastrepl := swap(pkt.icmp.seq); end;
                if miss then writeln; { newline after a series of '.'s }
                writeln(pref,ipaddr_str(pkt.ip.src),' t',pkt.ip.ttl,' s',swap(pkt.icmp.seq)); { print info line }
                writeln('time: ~',($ffff-tmrcv)*100,' ms'); { silly approximation assuming 1 tick = 100 ms }
                miss := false;
            end else begin write('.'); miss:=true; end; { it's a miss }
        end;
        if cnt > 0 then dec(cnt); { will not decrease if cnt = -1, which will keep looping then }
        inc(sq);
        if cnt <> 0 then begin { do not wait on last countdown }
            timerstart(0,intvl);
            repeat until timerout(0);
        end;
    until (r=e_intr) or (cnt=0); { loop when interrupted or when counter down }
    if miss then writeln;
    writeln(sep);writeln(ipaddr_str(ip),': tx ',snt,', rx ',rcv); { summary line }

end;

label ex;
{ main() }
begin
    if (paramcount<1) then begin
        writeln('usage: ',paramstr(0),' [ icmp | ns <hst> | ping <hst> [cnt] [sz] [ivl] | set < def | <k> <v> > ]');
        goto ex;
    end;
    { reuse the host var temporarily, accessing function result with index is problematic }
    host := paramstr(1); c := upcase(host[1]);
    { do this once }
    if paramcount > 1 then host := paramstr(2);
    { do not init PBNET unless we need it }
    if (c <> 'S') and not(pbn_init) then exit;
    case c of
    'I': begin
        icmp_resp;
        goto ex;
    end;
    'N': begin
        if paramcount<2 then begin
            writeln('ns: hostname required');
            goto ex;
        end;
        { query DNS regardles if it's an IP address or domain }
        dns_query;
        goto ex;
    end;
    'P': begin
        if paramcount<2 then begin
            writeln('ping: hostname required');
            goto ex;
        end;
        if paramcount > 2 then begin
            val(paramstr(3),n,m); if m=0 then begin
                if n < 1 then n := -1;
                cnt:=n;
            end;
        end;
        if paramcount > 3 then begin
            val(paramstr(4),n,m); if (m=0) and (n>-1) then sz:=n;
        end;
        if paramcount > 4 then begin
            val(paramstr(5),n,m); if (m=0) then intvl:=n;
        end;
        { only query DNS if it's not an IP address }
        if not ipaddr_parse(host,ip) then dns_query;
        if r <> e_ok then goto ex;
        ping(ip, cnt, sz, intvl);
    end;
    'S': begin
        if (paramcount=2) and (paramstr(2)='defaults') then begin
            pbn_defaults;
            goto ex;
        end;
        if paramcount<3 then begin
            writeln('set: key and value required');
            goto ex;
        end;
        if not pbn_set(paramstr(2), paramstr(3)) then begin
            writeln('set: unsuccessful');
        end;
    end;

    end; { case }
ex:
pbn_shutdown;

end.
