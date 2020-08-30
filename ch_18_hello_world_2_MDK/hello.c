#include "stm32f4xx.h"
#include "stdio.h"

int main(void)
{
  char textbuffer[40]; // Text buffer

  SCB->CCR |= SCB_CCR_STKALIGN_Msk; // Enable double word stack alignment 
  //(recommended in Cortex-M3 r1p1, default in Cortex-M3 r2px and Cortex-M4)
  printf("\nHello world!\n");
  while (1) {
    printf("Please enter text:");
    fgets(textbuffer, (sizeof(textbuffer)-1), stdin);		
    printf("\nYou entered      :%s\n",textbuffer);
  }
}
