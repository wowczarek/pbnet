; Copyright (c) 2021, Wojciech Owczarek
; All rights reserved.
; BSD 2-clause licence - see LICENSE in the top directory

; base/org are meaningless in inline code, but keep the compiler happy
BASE: EQU &H7000
START here
ORG BASE
here:

    gre IX, $22 ; save IX
    ; c: $12, n: $13, i: $14,mac: $15,mic:$16; $10..$11 = helpers
    ; $0..$1 = pkt, $2..$3 = cmap, $4..$5 = wbuf; $6..$7 = spos, $8..$9 = len
    ldim $0, (IX + $31), 8 ; load first 4 parameters into $0...$7
    ldw $8, (IX + $31) ; load 5th parameter (len) into $8..$9
    adw $0,$6 ; add offset (spos) to packet address
    pre IZ, $0 ; put packet data address into IZ, buf
    pre IX, $2 ; put check map address into IX, cmap

    ; clear check map
    xrw $14, $14 ; constant = 0
    xrw $16, $16 ; constant = 0
    xrw $18, $18 ; constant = 0
    xrw $20, $20 ; constant = 0

    ld $12, 4 ; loop counter
clloop:
    ; unrolled zeroing of the check map array
    stim $14, (IX+$31), 8 ; zero 8 bytes, IX+=8
    stim $14, (IX+$31), 8 ; zero 8 bytes, IX+=8
    stim $14, (IX+$31), 8 ; zero 8 bytes, IX+=8
    stim $14, (IX+$31), 8 ; zero 8 bytes, IX+=8
    stim $14, (IX+$31), 8 ; zero 8 bytes, IX+=8
    stim $14, (IX+$31), 8 ; zero 8 bytes, IX+=8
    stim $14, (IX+$31), 8 ; zero 8 bytes, IX+=8
    stim $14, (IX+$31), 8 ; zero 8 bytes, IX+=8
    sb $12, $30 ; decrement loop counter
    jr nz, clloop ; repeat until counter = 0

    pre IX, $2 ; reset IX, IX = cmap
    ldw $10,$8 ; copy len into $10
    ld $15,0 ; mac = 0
    ld $16,&hff ; mic = 255

; check and mark bytes present in block
cmloop:
    ldi $12,(IZ+$31) ; c = buf++
    st $30,(IX+$12) ; cmap[c]=1
    sbc $15,$12    ; if c > mac
    jr nc, oldmac   ; nope
    ld $15, $12     ; mac = c
    oldmac:
    sbc $12, $16   ; if mic < c
    jr nc, oldmic   ; nope
    ld $16, $12     ; mic = c
    oldmic:
    sbw $10,$30   ; dec left
    jr nz, cmloop ; any left? repeat
; check if min byte is above max control character
miccheck:
    ld $12, 3     ; pb_mc
    sbc $12, $16 ; if mic > pb_mc
    jr nc, maccheck ; nope
    ; yes: fill replacements with ffff (sub1=sub2 is checked on the other end)
    pre IZ, $4    ; IZ = wbuf
    ldw $10,&hffff ; $10 = ffff
    stiw $10, (IZ+$31) ; wbuf[0..1] = ffff, IZ = wbuf+4
    stiw $10, (IZ+$31) ; wbuf[2..3] = ffff, IZ = wbuf+4
    ; now block copy the rest
    pre IX,$0	; ix (block start) = pkt + pos
    ldw $10, $0 ; $10 = pkt + pos
    adw $10, $8 ; $10 = pkt+pos+len
    pre IY,$10  ; iy (block end) = pkt + pos + len
    bup         ; copy the block
    jr done	; the end
; check if max byte is below 252 
maccheck:
    ld $16, &hfc ; tmp = 0xfc
    sbc $15, $16 ; if mac < 0xfc
    jr nc, findsub; nope
    ; yes: just use fc,fd,fe,ff as replacements
    pre IZ, $4 ; IZ = wbuf
    ldw $12, &hfdfc ; tmp, little endian
    ldw $14, &hfffe; tmp, little endian
    stm $12, (IZ+$31), 4 ; wbuf[0..3] = fcfdfeff
    jr havesub;
; find replacements
findsub:
    pre IZ, $4 ; IZ = wbuf
    ldw $10,4 ; store 4
    adw $10,$2 ; add cmap
    pre IX, $10 ; IX = cmap + 4
    ldw $13, &h0400; i = $14 = 4, n = $13 = 0
    ld $11, 4 ; constant
fsloop:
    ldi $12, (IX+$31) ; c = cmap++
    sbc $12, $30 ; if c = 1
    jr z, cont ; yes -> move on
    sti $14, (IZ+$31) ; wbuf++ = i
    ad $13, $30 ; n++
    sbc $13, $11 ; if n = 4
    jr z, havesub ; then break
    cont:
    ad $14, $30 ; i++
    jr nz, fsloop ; no carry = repeat
; we have idendified replacement bytes
havesub:
    ldw $10,4 ; constant
    adw $10,$4 ; add wbuf
    pre IZ, $10 ; IZ = wbuf + 4;
    pre IX, $0 ; IX = pkt
    ldw $10, $8 ; loop counter = left
    ld $15,4; constant
    ld $13, 0; clear $13 for later (byte->word with $12)
subloop:
    ldi $12, (IX+$31) ; c = pkt++
    sbc $12, $15 ; if c < 4
    jr nc, nosub ; else ->
    ldw $8, $4	; $8 = wbuf
    adw $8, $12  ; $8 = wbuf + c
    ld $12,($8) ; c = wbuf[c];
    nosub: ; -> else
    sti $12, (IZ+$31); wbuf[4] = c, wbuf++
    sbw $10,$30 ; if i left < 0
    jr nz, subloop ; then repeat
done:
    pre IX, $22 ; restore IX
