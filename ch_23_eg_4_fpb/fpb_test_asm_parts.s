                 PRESERVE8
                 THUMB

                 AREA    |.text|, CODE, READONLY

                 ;// Program sequence to be patched
fpb_test1        PROC				 
                 EXPORT  fpb_test1
                 IMPORT  function1
                 PUSH    {R4, LR}         ; Need to Push LR, push R4 as well for double word stack alignment
                 BL      function1        ; Call Function 1 - to be patched to call function2
                 POP     {R4, PC}         ; Return				 
                 ENDP
                 ;// Place the patch function here just so that we can test forward and backward BL
                 ;// In practice it can be anywhere as long as it is within BL range.
function2        PROC
                 EXPORT  function2
                 IMPORT  function2b
                 PUSH    {R4, LR}         ; Need to Push LR, push R4 as well for double word stack alignment
                 BL      function2b       ; Call Function 2b to display message
                 POP     {R4, PC}
                 ENDP
                 ;// Program sequence to be patched
fpb_test2        PROC				 
                 EXPORT  fpb_test2
                 IMPORT  function1
                 PUSH    {R4, LR}         ; Need to Push LR, push R4 as well for double word stack alignment
                 BL      function1        ; Call Function 1 - to be patched to call function2
                 POP     {R4, PC}         ; Return				 
                 ENDP
                 ALIGN
                 END
