#include "stm32f4xx.h"
#include <stdio.h>

void branch_simple(unsigned int i);
void branch_method1(unsigned int i);
void branch_method2(unsigned int i0, unsigned int i1, unsigned int i2, 
unsigned int i3,unsigned int i4, unsigned int i);
__asm void branch_method3(unsigned int i);
void branch_method4(unsigned int i);
void branch_method3_printf(unsigned int i);

void Delay(uint32_t nCount);
void LED_config(void);
void blinky(void);

int main(void)
{
  unsigned int i, i0, i1, i2, i3, i4;
  SCB->CCR |= SCB_CCR_STKALIGN_Msk; // Enable double word stack alignment 
  //(recommended in Cortex-M3 r1p1, default in Cortex-M3 r2px and Cortex-M4)
  LED_config();
  printf ("Simple code:\n");
  for (i=0; i<32; i++) {
    branch_simple(i);
    }
  printf ("Method 1:\n");
  for (i=0; i<32; i++) {
    branch_method1(i);
    }
  printf ("Method 2:\n");
  for (i=0; i<32; i++) {
    i0 =  i & 0x1;    // Extract bit 0
    i1 = (i & 0x2)>>1;// Extract bit 1
    i2 = (i & 0x4)>>2;// Extract bit 2
    i3 = (i & 0x8)>>3;// Extract bit 3
    i4 = (i & 0x10)>>4;// Extract bit 4
    branch_method2(i0, i1, i2, i3, i4, i);
    }	
  printf ("Method 3:\n");
  for (i=0; i<32; i++) {
    branch_method3(i);
    }
  printf ("Method 4:\n");
  for (i=0; i<128; i++) {
    branch_method4(i);
    }

  blinky(); /* end test */
}
/*---------------------------------------*/
void branch_simple(unsigned int i)
{
  if ((i==2) || (i==3) || (i==5) || (i==7) ||
    (i==11) || (i==13) || (i==17) || (i==19) ||
    (i==23) || (i==29) || (i==31))  {
    printf ("- %d\n", i);
    }
  return;
}
/*---------------------------------------*/
void branch_method1(unsigned int i)
{
  /* Bit pattern is
  31:0 - 1010 0000 1000 1010 0010 1000 1010 1100 = 0xA08A28AC */
  if ((1<<i) & (0xA08A28AC)) {
    printf ("- %d\n", i);
  }
  return;
}
/*---------------------------------------*/
void branch_method2(unsigned int i0, unsigned int i1, unsigned int i2, 
unsigned int i3,unsigned int i4,unsigned int i)
{
  unsigned int tmp=0;
  if (i0) tmp=1;
  if (i1) tmp|=2;
  if (i2) tmp|=4;
  if (i3) tmp|=8;	
  if (i4) tmp|=0x10;	
  if ((1<<tmp) & (0xA08A28AC)) {
    printf ("- %d\n", i);
  }
  return;
}
/*---------------------------------------*/
__asm void branch_method3(unsigned int i)
{
  PUSH {R4,LR} ; Push two registers to ensure double word stack alignment
  LDR  R1, =0xA08A28AC
  ADDS R2, R0, #1
  LSRS R1, R1, R2	
  BCC  	branch_method3_exit
  BL   __cpp(branch_method3_printf)
branch_method3_exit
  POP {R4, PC}
  ALIGN
}
void branch_method3_printf(unsigned int i)
{
  printf ("- %d\n", i);
	return;
}
/*---------------------------------------*/
void branch_method4(unsigned int i)
{
  /* Bit pattern is
  31: 0 - 1010 0000 1000 1010 0010 1000 1010 1100 = 0xA08A28AC
  63:32 - 0010 1000 0010 0000 1000 1010 0010 0000 = 0x28208A20
  95:64 - 0000 0010 0000 1000 1000 0010 1000 1000 = 0x02088288
 127:96 - 1000 0000 0000 0010 0010 1000 1010 0010 = 0x800228A2
  */
  const uint32_t bit_pattern[4] = {0xA08A28AC,
		0x28208A20, 0x02088288, 0x800228A2};
  uint32_t i1, i2;
  i1 = i & 0x1F;       // Bit position
  i2 = (i & 0x60) >> 5;// Mask index
  if ((1<<i1) & (bit_pattern[i2])) {
    printf ("- %d\n", i);
  }
  return;
}
/*---------------------------------------*/
/**
  * @brief  Delay Function.
  * @param  nCount:specifies the Delay time length.
  * @retval None
  */
void Delay(uint32_t nCount)
{
  while(nCount--)
  {
		__NOP();
  }
}
/*---------------------------------------*/

void LED_config(void)
{
	// Configure LED outputs
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // Enable Port D clock
	
	GPIOD->MODER |= (GPIO_MODER_MODER12_0 | // Set pin 12, 13, 14, 15 as general purpose output mode (pull-push) 
	                 GPIO_MODER_MODER13_0 |
	                 GPIO_MODER_MODER14_0 |
	                 GPIO_MODER_MODER15_0 ) ;
	// GPIOD->OTYPER  |= 0; // No need to change - use pull-push output
#define USE_2MHZ_OUTPUT
#ifdef USE_2MHZ_OUTPUT
	// GPIOD->OSPEEDR |= 0; // No need to change - slow speed
#endif
#ifdef USE_25MHZ_OUTPUT
	GPIOD->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR12_0 |
	                   GPIO_OSPEEDER_OSPEEDR13_0 |
	                   GPIO_OSPEEDER_OSPEEDR14_0 |
	                   GPIO_OSPEEDER_OSPEEDR15_0 );
#endif
#ifdef USE_50MHZ_OUTPUT
	GPIOD->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR12_1 |
	                   GPIO_OSPEEDER_OSPEEDR13_1 |
	                   GPIO_OSPEEDER_OSPEEDR14_1 |
	                   GPIO_OSPEEDER_OSPEEDR15_1 );
#endif
#ifdef USE_100MHZ_OUTPUT
	GPIOD->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR12 |
	                   GPIO_OSPEEDER_OSPEEDR13 |
	                   GPIO_OSPEEDER_OSPEEDR14 |
	                   GPIO_OSPEEDER_OSPEEDR15 );
#endif
	GPIOD->PUPDR = 0; // No pull up , no pull down

	return;
}
/*---------------------------------------*/

void blinky(void){
	#define LOOP_COUNT 0x3FFFFF	
	while(1){
		GPIOD->BSRRL = (1<<12); // Set   bit 12
		Delay(LOOP_COUNT);
		GPIOD->BSRRH = (1<<12); // Clear bit 12
		GPIOD->BSRRL = (1<<13); // Set   bit 13
		Delay(LOOP_COUNT);
		GPIOD->BSRRH = (1<<13); // Clear bit 13
		GPIOD->BSRRL = (1<<14); // Set   bit 14
		Delay(LOOP_COUNT);
		GPIOD->BSRRH = (1<<14); // Clear bit 14
		GPIOD->BSRRL = (1<<15); // Set   bit 15
		Delay(LOOP_COUNT);
		GPIOD->BSRRH = (1<<15); // Clear bit 15
	  };	
}
/*---------------------------------------*/
