    PRESERVE8 ; Indicate the code here preserve 
              ; 8 byte stack alignment
    THUMB     ; Indicate THUMB code is used
    AREA    |.text|, CODE, READONLY   ; Start of CODE area
    EXPORT __main
    ENTRY
__main     FUNCTION
	MOVS   R0, #'\n'
	BL     Putc
MainLoop
    LDR    R0,=HELLO_TXT  ; Get address of the string
    BL     Puts           ; Display string
    BL     Delay	      ; Delay
    B      MainLoop         
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
HELLO_TXT
    DCB "Hello world\n", 0 ; Null terminated string
    ALIGN   4
    END            ; End of file
