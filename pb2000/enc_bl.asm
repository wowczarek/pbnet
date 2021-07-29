; Copyright (c) 2021, Wojciech Owczarek
; All rights reserved.
; BSD 2-clause licence - see LICENSE in the top directory

; base/org are meaningless in inline code, but keep the compiler happy
BASE: EQU &H7000
START here
ORG BASE
here:

    ; encode a block of data using the escapeless encoding.

    gre IX, $22 ; save IX
    ; c: $12, n: $13, i: $14,mac: $15,mic:$16; $10..$11 = helpers
    ; $0..$1 = pkt, $2..$3 = cmap, $4..$5 = wbuf; $6..$7 = spos, $8..$9 = len
    ldim $0, (IX + $31), 8  ; load first 4 args into $0...$7
    ldiw $8, (IX + $31)     ; load 5th arg (len) into $8..$9
    adw $0, $6              ; add offset (spos) to packet address, we don't need $6 anymore
    ldw $6, (IX+$31)        ; load bit shift buffer (smap) address into $6..$7
    pre IZ, $0              ; put packet data address into IZ, buf
    pre IX, $2              ; put check map address into IX, cmap

    ; clear the check map
    xrw $14,$14             ; $14..$21 = 0
    xrw $16,$16
    xrw $18,$18
    xrw $20,$20
    stim $14, (IX+$31), 8   ; zero 8 bytes, IX+=8, times 4
    stim $14, (IX+$31), 8
    stim $14, (IX+$31), 8
    stim $14, (IX+$31), 8
    pre IX, $2              ; reset IX, IX = cmap

    ldw $10,$8              ; copy len into $10

; check and mark bytes present in block
cmloop:

    ldi $12,(IZ+$31)        ; $12 = c = buf++

    ; in C, the whole next block would be: cmap[c>>3] |= (1 << (c & 7))
    ; instead we have this:

    ; === oneliner start
    ; split c into 5 upper bits (n) and 3 lower bits (b)
    byu $18                 ; b[hi] = 0
    ld $17,$12              ; b[lo] = c
    an $17,&h07             ; clear upper 5 bits in b[lo]
    ld $13,$12              ; n=c>>3
    bid $13                 ; no bid $13,3, come on Hitachi...
    bid $13
    bid $13
    ; set bit b in cmap[n]
    ; $13=cmap index=c, $17=bit index
    ld $14,(IX+$13)         ; $14 = cmap[n]
    adw $17, $6             ; $17 = &smap[b]
    ld  $19, ($17)          ; $19 = smap[b], temp
    or $14,$19              ; $14 |= smap[b]
    st $14,(IX+$13)         ; cmap[n] = $14
    ; === oneliner end

sbw $10,$30                 ; dec left
jr nz, cmloop               ; any left? repeat

; if there are no reserved bytes in the bitmap, there is no need for substitution
miccheck:
    ld $16,($2)            ; $16 = cmap[0];
    anc $16, &hf0          ; check if 4 "left" bits in cmap[0] are 0
    jr nz, maccheck        ; nope: try the other special case
    ; yes: fill replacements with ffff (sub1=sub2 is checked on the other end)
    pre IZ, $4              ; IZ = wbuf
    ldw $10,&hffff          ; $10 = ffff
    ; we could do stim, 4 bytes, and put 2 bytes of rubbish ($12..$13) into wbuf [2..3] and save an instruction...
    stiw $10, (IZ+$31)      ; wbuf[0..1] = ffff, IZ = wbuf+4
    stiw $10, (IZ+$31)      ; wbuf[2..3] = ffff, IZ = wbuf+4
    ; now block copy the rest
    pre IX,$0	            ; ix (block start) = pkt + pos
    ldw $10, $0             ; $10 = pkt + pos
    adw $10, $8             ; $10 = pkt+pos+len
    pre IY,$10              ; iy (block end) = pkt + pos + len
    bup                     ; copy the block
    jr done	                ; the end
; check if last 4 bytes are unused
maccheck:
    ld $15, $2              ; $15 = cmap
    ad $15, &hff            ; cmap+=255
    ld $16, ($15)           ; $16 = cmap[255];
    anc $16, &h0f           ; check if 4 lower bits in cmap[255] are 0
    jr nz, findsub          ; nope: find substitutes as usual
    ; yes: just use fc,fd,fe,ff as substitutes
    pre IZ, $4              ; IZ = wbuf
    ldw $12, &hfdfc         ; tmp, little endian
    ldw $14, &hfffe         ; tmp, little endian
    stm $12, (IZ+$31), 4    ; wbuf[0..3] = fcfdfeff
    jr havesub;

; find replacements
findsub:
    pre IZ, $4              ; IZ = wbuf
    pre IX, $2              ; IX = cmap
    ldw $13, &h0300         ; i = $14 = 3, n = $13 = 0
    ld $11, 4               ; constant
    ldi $12, (IX+$31)       ; c = cmap++;
    biu $12                 ; skip first 4 bits
    biu $12
    biu $12
    biu $12
    ld $17,4,jr fsiloop     ; start in inner loop with 4 bits left
fsloop:                     ; outer loop: move up in cmap
    ldi $12, (IX+$31)       ; c = cmap++
    ld $17,8                ; b = 8 (8 bits left to shift)
fsiloop:                    ; inner loop: shift bits in cmap[i]
    ad $14, $30             ; i++
    biu $12                 ; shift c
    jr c, cont              ; carry = had 1, move on
    sti $14, (IZ+$31)       ; wbuf++ = i
    ad $13, $30             ; n++
    sbc $13, $11            ; if n = 4
    jr z, havesub           ; then break
    cont:
    sb $17,$30              ; b--
    jr nz,fsiloop           ; more bits to shift? repeat
    xrc $14,$31             ; if i = 0 (rolled over)
    jr nz, fsloop           ; no = repeat

; we have idendified the replacement bytes, prepare
havesub:
    ldw $10,4               ; constant
    adw $10,$4              ; add wbuf
    pre IZ, $10             ; IZ = wbuf + 4;
    pre IX, $0              ; IX = pkt
    ldw $10, $8             ; loop counter = left
    ld $15,4                ; constant
    byu $13                 ; clear $13 for later (byte->word with $12)

; do the substitutions
subloop:
    ldi $12, (IX+$31)       ; c = pkt++
    sbc $12, $15            ; if c < 4
    jr nc, nosub            ; else ->
    ldw $8, $4              ; $8 = wbuf
    adw $8, $12             ; $8 = wbuf + c
    ld $12,($8)             ; c = wbuf[c];
    nosub:                  ; -> else
    sti $12, (IZ+$31)       ; wbuf[4] = c, wbuf++
    sbw $10,$30             ; if i left < 0
    jr nz, subloop          ; then repeat

done:
    pre IX, $22             ; restore IX
