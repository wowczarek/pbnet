; Copyright (c) 2021, Wojciech Owczarek
; All rights reserved.
; BSD 2-clause licence - see LICENSE in the top directory

; base/org are meaningless in inline code, but keep the compiler happy
BASE: EQU &H7000
START here
ORG BASE
here:

    ; calculate an RFC1071 checksum. we are little-endian, network is big-endian,
    ; but behold, the wonder of mathematics! the sum is byte-order independent!

    ; 20 instructions, 51 bytes of machine code.
    ; note that we are subtracting, not adding, just to save an inversion at the end.

    ; data addr: $0..$1, offset: $2..$3, len: $4..$5
    ldm $0,(IX+$31),6 ; load all three args into $0..$5
    adw $0, $2      ; add offset to address
    pre IZ, $0      ; put the result into IZ (starting address)
    ldw $2, 2       ; constant = 2
    xrw $6, $6      ; zeroize $6 = sum
loop:
    ; you would expect sbcw first (check), jump on carry, then sbw (subtract)...
    ; but we decrement in advance, and thus save an extra instruction per loop. WIN.
    sbw $4, $2      ; len -= 2 -> if(len>1)
    jr c, lastbyte  ; no? no loop
    ldiw $8,(IZ+$31); next word into $8..$9, pos+=2
    sbw $6,$8       ; add word (well, subtract, you get me?)
    jr nc, loop     ; no carry = continue
    sbw $6,$30,loop ; extra 1 for carry and continue
lastbyte:
    adw $4, $2      ; we end up with -1 or -2 here, so add it back
    jr z, end       ; 0=done, the only other possible $4 value is 1 = last byte
    ld $8, (IZ+$31) ; last byte into $8
    byu $9          ; zero into $9
    sbw $6, $8      ; add last word (subtract that is)
    jr nc,end       ; no carry = done
    sbw $6,$30      ; extra 1 for carry
end:
    biu $2          ; turn the 2 into 4
    stw $6, (IX-$2) ; result! (IX-4)
