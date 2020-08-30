#include "stdio.h"
#include "stm32f4xx.h"

int main(void)
{
	SCB->CCR |= SCB_CCR_STKALIGN_Msk; // Enable double word stack alignment 
	//(recommended in Cortex-M3 r1p1, default in Cortex-M3 r2px and Cortex-M4)

  printf("\nHello world!\n");
	while (1);
}
