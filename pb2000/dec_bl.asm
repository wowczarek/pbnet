; base/org are meaningless in inline code, but keep the compiler happy
BASE: EQU &H7000
START here
ORG BASE
here:
    ; $0..$1 = pkt, $2..$3 = wbuf, $4..$5 = pos, $6 = len
    ldm $0, (IX + $31), 7 ; load first 4 parameters into $0...$6
    ; we are done with IX for now, store it until near exit
    gre IX, $18 ; save IX

    adw $0,$4 ; add offset (pos) to packet address
    pre IZ, $0 ; put packet data address into IZ, pkt
    pre IX, $2 ; put buffer address into IX, wbuf

    ldim $8, (IX+$31), 4 ; $8...$11 = sub1...sub4; wbuf+=4
    xrc $8,$9 ; if sub1=sub2
    jr nz,copyloop ; else continue
    ; block copy, no replacements
    ldw $10, $2; $10 = wbuf
    xr $7,$7 ; $7 = 0
    adw $10, $6 ; $10 = wbuf+len
    pre IY,$10  ; iy (block end) = wbuf + len
    bup         ; copy the block
    jr done	; the end

    sb $6,4 ; len -= 4;
copyloop:
    ldi $12, (IX+$31) ; c = buf++
sub1:
    xrc $12, $8 ; if c = sub1
    jr nz, sub2
    ld $12, 0, jr bytedone ; then c = 0
sub2:
    xrc $12, $9 ; if c = sub2
    jr nz, sub3
    ld $12, 1, jr bytedone ; then c = 1
sub3:
    xrc $12, $10 ; if c = sub3
    jr nz, sub4
    ld $12, 2, jr bytedone ; then c = 2
sub4:
    xrc $12, $11 ; if c = sub4
    jr nz, bytedone
    ld $12, 3 ; then c = 3
bytedone:
    sti $12, (IZ+$31) ; *pkt++ = c
    sb $6,$30 ; len--
    jr nz, copyloop;
done:
    pre IX, $18 ; restore IX
