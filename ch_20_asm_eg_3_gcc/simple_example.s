     .text              /* text section */
     .syntax unified    /* Unified Assembly Syntax – UAL */
     .thumb             /* Thumb instruction set */
     .type __main, %function
     .global __START    /* Make __START visible from outside */
__START:
     /* initialize registers */
     MOV  r0, #10  	/* Starting loop counter value */
     MOV  r1, #0   	/* starting result */
     /* Calculated 10+9+8+...+1 */
loop:
     ADD  r1, r0   	/* R1 = R1 + R0 */
     SUBS r0, #1   	/* Decrement R0, update flag (“S” suffix) */
     BNE  loop    	/* If result not zero jump to loop */
     /* Result is now in R1 */
deadloop:
     B   deadloop  	/* Infinite loop  */
     .end               /* End of file */

