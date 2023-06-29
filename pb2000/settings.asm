; Copyright (c) 2021, Wojciech Owczarek
; All rights reserved.
; BSD 2-clause licence - see LICENSE in the top directory

; base/org are meaningless in inline code, but keep the compiler happy
BASE: EQU &H7000
START here
ORG BASE
here:

    ; so it's safe to call this proc
    jr skip

    ; this data lives at offset 17 (&h13) from top of procedure.
    ; since the PB-2000C uses RAM for file storage, we simply read
    ; and write this area, without the need for a config file.

    ; 45 bytes, work area
    db 10,99,99,2               ; IP address
    db 9,9,9,9                  ; DNS IP
    db &hdc,&h05                ; MTU 1500 = &h05dc
    db 128                      ; default TTL
    db 224                      ; default block size
    db 1                        ; checksums
    db '7'                      ; baud rate (9600 = 7)
    db 0                        ; search domain, length 0
    db 0,0,0,0,0,0,0,0,0,0      ; search domain[30], empty data
    db 0,0,0,0,0,0,0,0,0,0
    db 0,0,0,0,0,0,0,0,0,0

    ; another 45 bytes, defaults
    db 10,99,99,2               ; IP address
    db 9,9,9,9                  ; DNS IP
    db &hdc,&h05                ; MTU 1500 = &h05dc
    db 128                      ; default TTL
    db 224                      ; default block size
    db 1                        ; checksums
    db '7'                      ; baud rate (9600 = 7)
    db 0                        ; search domain, length 0
    db 0,0,0,0,0,0,0,0,0,0      ; search domain[30], empty data
    db 0,0,0,0,0,0,0,0,0,0
    db 0,0,0,0,0,0,0,0,0,0


skip: