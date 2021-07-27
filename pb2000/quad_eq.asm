; Copyright (c) 2021, Wojciech Owczarek
; All rights reserved.
; BSD 2-clause licence - see LICENSE in the top directory

; base/org are meaningless in inline code, but keep the compiler happy
BASE: EQU &H7000
START here
ORG BASE
here:
    ; check if two 4-byte sequences are equal. faster and not broken like DL-Pascal's longint comparison
    ldm $8, (IX+$31),4   ; load addr ipa and ipb into $8..$11
    pre IZ,$8            ; iz = ipa
    ldm $0, (IZ+$31),4   ; $0...$3 = ipa
    pre IZ,$10           ; iz = ipb
    ldm $4, (IZ+$31),4;  ; $4...$7 = ipb
    ld $10,3             ; constant, IX offset of a boolean return value is -3
    xrcm $0,$4,4         ; xor ipa with ipb
    jr nz,neq            ; not equal
    ld $0,$30,jr res     ; equal, $0=1, set result
neq:
    ld $0,$31            ; not equal, $0=0
res:
    st $0,(IX-$10)
