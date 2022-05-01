; Copyright (c) 2021, Wojciech Owczarek
; All rights reserved.
; BSD 2-clause licence - see LICENSE in the top directory

; base/org are meaningless in inline code, but keep the compiler happy
BASE: EQU &H7000
START here
ORG BASE
here:

    gre IX, $18             ; save IX

    ; $0..$1 = pkt, $2..$3 = wbuf, $4..$5 = pos, $6 = len
    ldm $0, (IX + $31), 7   ; load first 4 args into $0...$6
    ; we are done with IX for now, store it until near exit

    adw $0,$4               ; add offset (pos) to packet address
    pre IZ, $0              ; put packet data address into IZ, pkt
    pre IX, $2              ; put buffer address into IX, wbuf

    ldim $8, (IX+$31), 4    ; $8...$11 = sub1...sub4; wbuf+=4
    xrc $8,$9               ; if sub1=sub2
    jr nz,nocopy            ; else continue
    ; block copy, no replacements
    ldw $10, $2             ; $10 = wbuf
    byu $7                  ; $7 = 0
    adw $10, $6             ; $10 = wbuf+len
    pre IY,$10              ; iy (block end) = wbuf + len
    bup                     ; copy the block
    jr done	                ; the end

nocopy:
    sb $6,4                 ; len -= 4;

; surely there has to be a faster way to do this...

; note we operate on $0 and use it as 2nd argument for xor,
; so it gets optimised to SZ
decloop:
    ldi $0, (IX+$31)        ; c = buf++
sub1:
    xrc $8, $0              ; if c = sub1
    jr nz, sub2
    ld $0, $31, jr bytedone ; then c = 0
sub2:
    xrc $9, $0              ; if c = sub2
    jr nz, sub3
    ld $0, $30, jr bytedone ; then c = 1
sub3:
    xrc $10, $0             ; if c = sub3
    jr nz, sub4
    ld $0, 2, jr bytedone   ; then c = 2
sub4:
    xrc $11, $0             ; if c = sub4
    jr nz, bytedone
    ld $0, 3                ; then c = 3
bytedone:
    sti $0, (IZ+$31)        ; *pkt++ = c
    sb $6,$30               ; len--
    jr nz, decloop          ; if len > 0 then repeat
done:
    pre IX, $18             ; restore IX
