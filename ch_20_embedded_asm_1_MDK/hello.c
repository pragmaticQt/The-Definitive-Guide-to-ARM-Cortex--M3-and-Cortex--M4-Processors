#include "stdio.h"
#include "stm32f4xx.h"

__asm unsigned int simple_sqrt(unsigned int x);

int main(void)
{
  int temp;
  SCB->CCR |= SCB_CCR_STKALIGN_Msk; // Enable double word stack alignment 
  //(recommended in Cortex-M3 r1p1, default in Cortex-M3 r2px and Cortex-M4)

  printf("\nHello world!\n");
  temp = simple_sqrt(0);
  printf ("temp = %d\n", temp);
  temp = simple_sqrt(1);
  printf ("temp = %d\n", temp);
  temp = simple_sqrt(64);
  printf ("temp = %d\n", temp);
  temp = simple_sqrt(1025);
  printf ("temp = %d\n", temp);
  while (1);
}

__asm unsigned int simple_sqrt(unsigned int x)
{
    ; Input  : R0
    ; Output : R0 (square root result)
    MOVW    R1, #0x8000 ; R1 = 0x00008000  
    MOVS    R2, #0      ; Initialize result
simple_sqrt_loop
    ADDS    R2, R2, R1  ; M = (M + N)
    MUL     R3, R2, R2  ; R3 = M^2
    CMP     R3, R0      ; If M^2 > Input
    IT      HI          ; Greater Than
    SUBHI   R2, R2, R1  ; M = (M - N)
    LSRS    R1, R1, #1  ; N = N >> 1
    BNE     simple_sqrt_loop
    MOV     R0, R2      ; Copy to R0 and return
    BX      LR          ; Return
}

