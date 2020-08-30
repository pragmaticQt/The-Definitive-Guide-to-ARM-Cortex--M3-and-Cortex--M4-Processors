#include "stm32f4xx.h"

void Delay(uint32_t nCount);

int main(void)
{
	SCB->CCR |= SCB_CCR_STKALIGN_Msk; // Enable double word stack alignment 
	//(recommended in Cortex-M3 r1p1, default in Cortex-M3 r2px and Cortex-M4)

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

