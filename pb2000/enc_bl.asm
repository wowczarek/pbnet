; Copyright (c) 2021, Wojciech Owczarek
; All rights reserved.
; BSD 2-clause licence - see LICENSE in the top directory

; base/org are meaningless in inline code, but keep the compiler happy
BASE: EQU &H7000
START here
ORG BASE
here:

    ; encode a block of data using the escapeless encoding.

    ; to be rewritten with a 32-byte bit map instead of a 256-byte byte map.
    ; only problem is that HD67100 lacks n-bit shift instructions,
    ; DL-Pascal's shr and shl actually do loops and are stupidly expensive.
    ; so to set the bits, we will use an 8-byte LUT [1,2,4,8,16,32,64,128]
    ; we gain 216 bytes of RAM, reduce the check map clear code,
    ; but gain cycles per byte. to be tested!

    gre IX, $22 ; save IX
    ; c: $12, n: $13, i: $14,mac: $15,mic:$16; $10..$11 = helpers
    ; $0..$1 = pkt, $2..$3 = cmap, $4..$5 = wbuf; $6..$7 = spos, $8..$9 = len
    ldim $0, (IX + $31), 8 ; load first 4 parameters into $0...$7
    ldiw $8, (IX + $31) ; load 5th parameter (len) into $8..$9
    adw $0, $6 ; add offset (spos) to packet address, we don't need it anymore
    ldw $6, (IX+$31) ; load bit shift buffer (smap) address into $6..$7
    pre IZ, $0 ; put packet data address into IZ, buf
    pre IX, $2 ; put check map address into IX, cmap
    ; clear check map

    ; clear the check map
    xrw $14, $14 ; $14..$21 = 0
    xrw $16, $16
    xrw $18, $18
    xrw $20, $20
    stim $14, (IX+$31), 8 ; zero 8 bytes, IX+=8, times 4
    stim $14, (IX+$31), 8
    stim $14, (IX+$31), 8
    stim $14, (IX+$31), 8

    pre IX, $2 ; reset IX, IX = cmap
    ldw $10,$8 ; copy len into $10
    byu $15; mac = 0
    ld $16,&hff ; mic = 255

; check and mark bytes present in block
cmloop:

    ldi $12,(IZ+$31) ; $12 = c = buf++

    ; in C, the whole next block would be: cmap[c>>3] |= (1 << (c & 7))
    ; instead we have this:

    ; === oneliner start
    ; split the 5 upper bits into n and 3 lower bits into b
    byu $18          ; b[hi] = 0
    ld $17,$12       ; b[lo] = c
    an $17,&h07      ; clear upper 5 bits in b[lo]
    ld $13,$12       ; n=c>>3
    bid $13
    bid $13
    bid $13
    ; set bit b in cmap[n]
    ; $13=cmap index=c, $17=bit index
    ld $14,(IX+$13) ; $14 = cmap[n]
    adw $17, $6     ; $17 = &smap[b]
    ld  $19, ($17)  ; $19 = smap[b], temp
    or $14,$19      ; $14 |= smap[b]
    st $14,(IX+$13) ; cmap[n] = $14
    ; === oneliner end

    ; keep track of the minimum and maximum byte as we go
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
    pre IX, $2; IX = cmap
    ldw $13, &h0400; i = $14 = 4, n = $13 = 0
    ld $11, 4 ; constant
    ldi $12, (IX+$31) ; c = cmap++;
    bid $12 ; skip first 4 bits
    bid $12
    bid $12
    ld $17,4,jr fsiloop; start with 4 bits left
fsloop: ; outer loop: move up in cmap
    ldi $12, (IX+$31) ; c = cmap++
    ld $17,8 ; b = 8 (8 bits to shift)
fsiloop: ; inner loop: shift bits in cmap[i]
    ad $14, $30 ; i++
    bid $12 ; shift c
    jr c, cont ; carry = had 1, move on
    sti $14, (IZ+$31) ; wbuf++ = i
    ad $13, $30 ; n++
    sbc $13, $11 ; if n = 4
    jr z, havesub ; then break
    cont:
    sb $17,$30  ; b--
    jr nz,fsiloop ; b>0? repeat
    xrc $14,$31 ; if i = 0 (rolled over)
    jr nz, fsloop ; no = repeat
; we have idendified the replacement bytes
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
