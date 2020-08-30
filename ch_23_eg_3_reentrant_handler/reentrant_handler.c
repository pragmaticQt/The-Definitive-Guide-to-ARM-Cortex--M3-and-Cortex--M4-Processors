#include "stm32f4xx.h"
#include <stdio.h>

void Unknown_SVC_Request_Msg(unsigned int svc_num);
void Reentrant_SysTick_Handler(void);
void Delay(uint32_t nCount);
void LED_config(void);
void blinky(void);

volatile uint32_t SysTick_Nest_Level=0;

int main(void)
{
  SCB->CCR |= SCB_CCR_STKALIGN_Msk; // Enable double word stack alignment 
  //(recommended in Cortex-M3 r1p1, default in Cortex-M3 r2px and Cortex-M4)
  printf ("Test start\n");
  NVIC_SetPriority(SVCall_IRQn ,(0x80>>(8-__NVIC_PRIO_BITS)));   //SVC     - medium priority level
  NVIC_SetPriority(SysTick_IRQn,(0xE0>>(8-__NVIC_PRIO_BITS)));   //SysTick - lowest priority level
  SCB->ICSR |= SCB_ICSR_PENDSTSET_Msk;  //Set pending status for SysTick
                                        // start test
  Delay(10);
  printf ("Test end\n");
  LED_config();
  blinky();
}
/*---------------------------------------*/
__asm void SysTick_Handler(void)
{ 
  ; Now we are in Handler mode, using Main Stack, and
  ; SP should be Double word aligned
  MRS  R0, PSR
  PUSH {R0, LR}       ; Need to save PSR and LR in stack
  SUB  SP, SP , #0x20 ; Reserve 8 words for dummy stack frame for return
  LDR  R0,=SysTick_Handler_thread_pt
  STR  R0,[SP, #24]
  LDR  R0,=0x01000000 ; xPSR
  STR  R0,[SP, #28]
  LDR  R0,=0xFFFFFFF9 ; Return to Thread with Main Stack, 8 word stack frame
  MOV  LR, R0
  BX   LR
SysTick_Handler_thread_pt
  BL   __cpp(Reentrant_SysTick_Handler)
  SVC  0              ; Use SVC to return to original Thread
  B    .              ; Should not return here
}
__asm void SVC_Handler(void)
{
  ; Extract SVC number
  TST	    LR,  #0x4	; Test EXC_RETURN bit 2
  ITE     EQ        ; if zero then
  MRSEQ   R0, MSP	  ; Get correct stack pointer to R0
  MRSNE   R0, PSP
  LDR     R1, [R0, #24]  ; Get stacked PC
  LDRB.W  R0, [R1, #-2]  ; Get SVC parameter at stacked PC minus 2
  CBZ     R0, svc_service_0 ; if zero, branch to SVC service 0
  B       Unknown_SVC_Request
svc_service_0
  ; Reentrant code finished, we can discard the current stack frame 
  ; and restore the original stack frame. However, the current 
  ; stack frame could be 8 words or 26 words.
  TST     LR,  #0x10	; Test EXC_RETURN bit 4
  ITE     EQ
  ADDEQ   SP, SP, #104 ; LR Bit 4 was 0, 26 words in stack frame
  ADDNE   SP, SP, #32  ; LR Bit 4 was 1,  8 words in stack frame
  POP     {R0, R1}
  MSR     PSR, R0
  BX      R1
Unknown_SVC_Request
  BL	    __cpp(Unknown_SVC_Request_Msg)
}
void Unknown_SVC_Request_Msg(unsigned int svc_num)
{  /* Display Error Message when SVC service is not known */
  printf("Error: Unknown SVC service request %d\n", svc_num);
  while(1);
}
/*---------------------------------------*/
void Reentrant_SysTick_Handler(void)
{
  printf ("[SysTick]\n");
	
  if (SysTick_Nest_Level < 3){
    SysTick_Nest_Level++;
    SCB->ICSR |= SCB_ICSR_PENDSTSET_Msk;  //Set pending status for SysTick
    __DSB();
    __ISB();
    Delay(10);
    SysTick_Nest_Level--;
    } else {
    printf ("SysTick_Nest_Level = 3\n");
  }
  printf ("leaving [SysTick]\n");
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
