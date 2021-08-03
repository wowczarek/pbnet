{ Copyright (c) 2021, Wojciech Owczarek }
{ All rights reserved }
{ BSD 2-clause licence - see LICENSE in the top directory }

{$D-}
program net;
uses pbnet;

procedure icmp_test;
var pkt:ippkt;
r:shortint;
label en;
begin
    if not pbn_init then exit;
    writeln('ICMP echo responder running');
    repeat
        pkt_ready;
        r:=pkt_get(pkt,30);
        if r=e_intr then goto en;
        if r=e_ok then r:=echo_respond(pkt,30);
    until false;
en:
    pbn_shutdown;
end;

procedure dns_test;
var
pkt:ippkt;
r:shortint;
ps:string;
ip:ipaddr;
begin
    ps:=paramstr(2);
    if not pbn_init then exit;
    writeln('looking up ',ps,'...');
    r:=dns_resolve(ps,ip,pkt,30,5);
    if r=e_ok then begin
        write(ps,' is: ', ipaddr_str(ip));
        writeln;
    end else writeln('dns_resolve error: ',strerr(r));
    pbn_shutdown;
end;

label usage;
begin
    if (paramcount<1) then goto usage;

    if paramstr(1)='icmp' then begin
        icmp_test;
        exit;
    end else if paramstr(1)='ns' then begin
        if paramcount<2 then begin
            writeln('resolve: hostname required');
            exit;
        end;
        dns_test;
        exit;
    end;

usage:
        writeln('usage: ',paramstr(0),' [icmp | ns <host> | ping <host> [count] [size] ]');
        exit;

end.
