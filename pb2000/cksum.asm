; base/org are meaningless in inline code, but keep the compiler happy
BASE: EQU &H7000
START here
ORG BASE
here:
    ldm $0, (IX+$31), 6 ; load all three params into $0..$5
    ; data: $0..$1, offset: $2..$3, len: $4..$5
    adw $0, $2        ; add offset to address
    pre IZ, $0      ; put the result into IZ (starting address)
    ldw $2, 2 ; constant = 2
    xrw $6, $6 ; zeroize $6 = sum
loop:
    sbcw $4, $2 ; check if len > 1
    jr c, lastbyte ; carry = len <=1, no loop
    ldiw $8,(IZ+$31) ; next word into $8, pos increments by 2
    adw $6,$8 ; sum up
    jr nc, nofold ; no carry = continue
fold:
    adw $6, $30 ; carry? add 1
    jr c, fold ; still carry? fold again
nofold:
    sbw $4, $2  ; decrement len by 2
    jr loop ; back to beginning
lastbyte:
    sbcw  $4, $30 ; len = 1?
    jr nz, end; no
    ld $8, (IZ+$31) ; last byte into $8:hi
    xr $9,$9 ; zero into $8:lo
    adw $6, $8 ; add last byte
    jr nc, end ; no carry = done
fold2:
    adw $6, $30 ; carry ? add 1
    jr c, fold2 ; still carry? fold again
end:
    invw $6 ; invert
    ld $10,4
    stw $6, (IX-$10) ; result! (IX-4)
