    PRESERVE8 ; Indicate the code here preserve 
              ; 8 byte stack alignment
    THUMB     ; Indicate THUMB code is used
    AREA    |.text|, CODE, READONLY   ; Start of CODE area
    EXPORT __main
    ENTRY
__main     FUNCTION
    LDR    R0,=Start_message  ; Get address of the string
	BL     Puts
	LDR    R0,=0x01234567
	BL     PutHex
	MOVS   R0, #'\n'
	BL     Putc
    LDR    R0,=0x89ABCDEF
	BL     PutHex
	MOVS   R0, #'\n'
	BL     Putc
	LDR    R0,=0xFFFFFFFF ; 4294967295
    BL     PutDec
	MOVS   R0, #'\n'
	BL     Putc
	LDR    R0,=0
    BL     PutDec
	MOVS   R0, #'\n'
	BL     Putc
	LDR    R0,=10234503
    BL     PutDec
	MOVS   R0, #'\n'
	BL     Putc
	LDR    R0,=1
    BL     PutDec
	MOVS   R0, #'\n'
	BL     Putc
	LDR    R0,=10
    BL     PutDec
	MOVS   R0, #'\n'
	BL     Putc
	LDR    R0,=100
    BL     PutDec
	MOVS   R0, #'\n'
	BL     Putc
MainLoop
    BL     Delay	      ; Delay
    B      MainLoop         
    ENDFUNC

;------------------------------------------------
PutHex FUNCTION
    ; Output register value in hexadecimal format
    ; Input R0 = value to be displayed
    PUSH   {R4 - R6, LR}
    MOV    R4, R0    ; Save register value to R4 because R0 is used
                     ; for passing input parameter
    MOV    R0,#'0'   ; Starting the display with "0x"
    BL     Putc
    MOV    R0,#'x'
    BL     Putc
    MOV    R5, #8       ; Set loop counter
    MOV    R6, #28      ; Rotate offset
PutHexLoop
    ROR    R4, R6       ; Rotate data value left by 4 bits 
                        ; (right 28)
    AND    R0, R4,#0xF  ; Extract the lowest 4 bit
    CMP    R0, #0xA     ; Convert to ASCII
    ITE    GE
    ADDGE  R0, #55      ; If larger or equal 10, then convert 
                        ; to A-F
    ADDLT  R0, #48      ; otherwise convert to 0-9
    BL     Putc         ; Output 1 hex character
    SUBS   R5, #1       ; decrement loop counter
    BNE    PutHexLoop   ; if all 8 hexadecimal character been 
                        ; display then
    POP    {R4-R6,PC}   ; return, otherwise process next 4-bit
    ENDFUNC
;------------------------------------------------
PutDec  FUNCTION
    ; Subroutine to display register value in decimal
    ; Input R0 = value to be displayed.
    ; Since it is 32 bit, the maximum number of character
    ; in decimal format, including null termination is 11
    ; According AAPCS, a function can change R0-R3, R12
    ; So we use these registers for processing.
    ; R0 - Input value
    ; R1 - Divide value (10)
    ; R2 - Divide result
    ; R3 - Remainder
    ; R12- Text buffer pointer	
    PUSH   {R4, LR}      ; Save register values
    MOV    R12, SP       ; Copy current Stack Pointer to R4
    SUB    SP, SP, #12   ; Reserved 12 bytes as text buffer
    MOVS   R1, #0        ; Null character
    STRB   R1,[R12, #-1]!; Put a null character at end of text buffer
    ; buffer,pre-indexed
    MOVS   R1, #10       ; Set divide value
PutDecLoop
    UDIV   R2, R0, R1    ; R2 = R0 / 10 = divide result
    MUL    R4, R2, R1 	 ; R4 = R2 * 10 = Value - remainder (multiple of 10)
    SUB    R3, R0, R4 	 ; R2 = R0 - (R2 * 10) = remainder
    ADDS   R3, #48       ; convert to ASCII (R2 can only be 0-9)
    STRB   R3,[R12, #-1]!; Put ascii character in text buffer
                         ; pre-indexed
    MOVS   R0, R2        ; Set R0 = Divide result and set Z flag if R4=0
    BNE    PutDecLoop    ; If R0(R4) is already 0, then there 
                         ; is no more digit
    MOV    R0, R12       ; Put R0 to starting location of text 
                         ; buffer
    BL     Puts          ; Display the result using Puts
    ADD    SP, SP, #12 	 ; Restore stack location
    POP    {R4, PC}      ; Return
    ENDFUNC
;------------------------------------------------
Puts   FUNCTION
    ; Subroutine to send string to display
    ; Input R0 = starting address of string.
    ; The string should be null terminated
    PUSH  {R4, LR} 	; Save registers
    MOVS  R4, R0    
    ; Copy address to R1, because R0 will 
    ; be used
PutsLoop           	; as input for Putc
    LDRB  R0,[R4],#1 
    ; Read one character and increment address (post index)
    CBZ   R0, PutsLoopExit ; if character is null, goto end
    BL    Putc          ; Output character to UART
    B     PutsLoop   	; Next character
PutsLoopExit
    POP   {R4, PC} ; Return
    ENDFUNC
;------------------------------------------------
; Send a character via ITM
ITM_BASE   EQU 0xE0000000
ITM_PORT0  EQU (ITM_BASE+0x000)
ITM_TER    EQU (ITM_BASE+0xE00)
ITM_TCR    EQU (ITM_BASE+0xE80)

Putc  FUNCTION
    ; Function to display one character
    ; Input R0 - chacter to be displayed
    LDR   R1,=ITM_TCR  ; 0xE0000E80
    LDR   R2,[R1]
    MOVS  R3, #1  ; Check ITMENA bit. If 0, exit
    TST   R2, R3
    BEQ   PutcExit
    LDR   R1,=ITM_TER  ; 0xE0000E00
    LDR   R2,[R1] ; Check Port 0 is enabled. If 0, exit
    TST   R2, R3
    BEQ   PutcExit
    LDR   R1,=ITM_PORT0  ; 0xE0000000
PutcWait
    LDR   R2,[R1]  ; Read status
    CMP   R2, #0
    BEQ   PutcWait
    STRB  R0,[R1]  ; Write a character	
PutcExit
    BX    LR
    ENDFUNC
;--------------------------------------
Delay  FUNCTION
    LDR   R0, =0x01000000
DelayLoop
    SUBS  R0, R0, #1
    BNE   DelayLoop
    BX    LR
    ENDFUNC
;--------------------------------------
; Text to be displayed
Start_message
    DCB " Value display test\n", 0 ; Null terminated string
    ALIGN   4
    END            ; End of file
