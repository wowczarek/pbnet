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

    ; 21 instructions, 51 bytes of machine code.

    ; $SZ=>$0,$SY=>$30(=1),$SX=>$31(=0)

    ; data addr: $0..$1, offset: $2..$3, len: $4..$5
    ldm $0,(IX+$SX),6 ; load all three args into $0..$5
    adw $0, $2      ; add offset to address
    pre IZ, $0      ; put the result into IZ (starting address)
    ldw $2, 2       ; constant = 2
    xrw $6, $6      ; zeroize $6 = sum
    ; we will now be using $0..$1 to load the words, 
    ; because it is available as specific register $SZ, saving 3 cycles on adw
loop:
    ; you would expect sbcw first (check), jump on carry, then sbw (subtract)...
    ; but we decrement in advance, and thus save an extra instruction per loop. WIN.
    sbw $4, $2      ; len -= 2 -> if(len>1)
    jr c, lastbyte  ; no? no loop
    ldiw $0,(IZ+$SX); next word into $0..$1, pos+=2
    adw $6,$SZ      ; add word
    jr nc, loop     ; no carry = continue
    adw $6,$SY,loop ; extra 1 for carry and continue
lastbyte:
    adw $4, $2      ; we end up with -1 or -2 here, so add it back
    jr z, end       ; 0=done, the only other possible $4 value is 1 = last byte
    ld $0, (IZ+$SX) ; last byte into $0
    byu $1          ; zero into $1
    adw $6,$SZ      ; add last word
    jr nc,end       ; no carry = done
    adw $6,$SY      ; extra 1 for carry
end:
    invw $6         ; invert
    biu $2          ; turn the 2 into 4
    stw $6, (IX-$2) ; result! (IX-4)
