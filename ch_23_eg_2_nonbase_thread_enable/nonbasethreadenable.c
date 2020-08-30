#include "stm32f4xx.h"
#include <stdio.h>

void Unknown_SVC_Request_Msg(unsigned int svc_num);
void User_SysTick_Handler(void);
void Delay(uint32_t nCount);
void LED_config(void);
void blinky(void);
uint8_t   svc_PSP_adjust; // Number of byte for PSP adjustment
uint32_t  process_stack[128];
volatile uint32_t ext0_irq_occurred;
volatile uint32_t ext1_irq_occurred;
int main(void)
{
  uint32_t  temp;
  SCB->CCR |= SCB_CCR_STKALIGN_Msk; // Enable double word stack alignment 
  //(recommended in Cortex-M3 r1p1, default in Cortex-M3 r2px and Cortex-M4)
  printf ("Test start\n");
  temp = ((uint32_t)(&process_stack[0])) + (4*128);
  __set_PSP(temp);
  printf ("PSP set to 0x%x\n", temp);
  NVIC_SetPriority(SVCall_IRQn ,(0x80>>(8-__NVIC_PRIO_BITS)));  //SVC     - medium priority level
  NVIC_SetPriority(SysTick_IRQn,(0xC0>>(8-__NVIC_PRIO_BITS)));  //SysTIck - low priority level
  NVIC_SetPriority(EXTI0_IRQn,  (0xC0>>(8-__NVIC_PRIO_BITS)));  //EXTI0   - low priority level
  NVIC_EnableIRQ(EXTI0_IRQn);
  NVIC_SetPriority(EXTI1_IRQn,  (0xA0>>(8-__NVIC_PRIO_BITS)));  //EXTI1   - medium low priority level
  NVIC_EnableIRQ(EXTI1_IRQn);
  SCB->CCR |= SCB_CCR_USERSETMPEND_Msk; //Enable unprivileged code to write to NVIC->STIR
  SCB->ICSR |= SCB_ICSR_PENDSTSET_Msk;  //Set pending status for SysTick
  Delay(10);
  printf ("Test end\n");
  LED_config();
  blinky();
}
/*---------------------------------------*/
__asm void SysTick_Handler(void)
{ // Redirect handler - no Floating point instruction
  PUSH {R4, LR} ; Push 2 words for double word stack alignment
  SVC  0        ; A SVC function to change from privileged to
                ; unprivileged mode
  BL   __cpp(User_SysTick_Handler)
  SVC  1        ; A SVC function to change back from user to
                ; privileged mode
  POP  {R4, PC} ; Return
}
/*---------------------------------------*/
void User_SysTick_Handler(void)
{
  printf ("[SysTick]\n");
  /* Try trigger an IRQ at same priority level as SysTick */
  printf ("- trying to set EXTI0_IRQn pending status\n");
  ext0_irq_occurred=0;
  ext1_irq_occurred=0;
  NVIC->STIR = (uint32_t) EXTI0_IRQn;
  __DSB();
  __ISB();
  if (ext0_irq_occurred != 0) {
    printf("Error: EXTI0_IRQn same priority as SysTick - not expected to run\n");
  }
  else {
    printf ("- EXTI0_IRQn does not run as it has same priority as SysTick\n");
  }
  /* Try trigger an IRQ at higher priority level than SysTick */
  printf ("- trying to set EXTI1_IRQn pending status\n");
  NVIC->STIR = (uint32_t) EXTI1_IRQn;
  __DSB();
  __ISB();
  if (ext1_irq_occurred == 0) {
    printf("Error: EXTI1_IRQn higher priority then SysTick - expected to run\n");
  } else {
    printf ("- EXTI1_IRQn run as expected because it has higher priority than SysTick\n");
  }
  printf ("[SysTick end]\n");	
  return;
}
/*---------------------------------------*/
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
  CMP     R0, #1
  BEQ     svc_service_1     ; if one, branch to SVC service 1
  B.W     Unknown_SVC_Request
  ; -----------------------------------------------------------
svc_service_0	; Service to switch handler from  
              ; privileged mode to unprivileged mode
              ; and use Process Stack. For this service,
              ; stack framce must be in Main Stack because it is
              ; called from an exception handler.
  MRS     R0, PSP        ; Adjust PSP to create space for new stack frame
                         ; and make a back up of PSP due to the 	
  TST     R0, #0x4       ; Check PSP to see if it is double word aligned
  ITE     EQ
  MOVSEQ  R3, #0x20      ; No Padding
  MOVWNE  R3, #0x24      ; Padding needed.
  LDR     R1, =__cpp(&svc_PSP_adjust)
  STRB    R3, [R1]       ; Record PSP adjustment for use in SVC #1
  SUBS    R0, R0, R3     ; PSP = PSP - 0x20 or 0x24
                         ; Make sure Process Stack is double word aligned
  MSR     PSP, R0        ; Copy back to PSP
  MOVS    R1, #0x20      ; Copy stack frame from main stack to
                         ; process stack. This stack frame is 8 words
                         ; because SysTick_Handler has not execute
                         ; any FP instruction yet.
svc_service_0_copy_loop  
  SUBS    R1, R1, #4
  LDR     R2, [SP, R1]   ; Read data
  STR     R2, [R0, R1]   ; 
  CMP     R1, #0
  BNE     svc_service_0_copy_loop

  LDR     R1,[R0, #0x1C]  ; Changed stacked xPSR so that IPSR=0
  MOVW    R2, #0x3FF      ; Clear IPSR, stack alignment bit
  BIC     R1, R1, R2
  STR     R1,[R0, #0x1C]  ; Clear stacked IPSR of user stack to 0
  LDR     R0, =0xE000ED14 ; Set Non-base thread enable in CCR
  LDR     r1,[r0]
  ORR     r1, #1
  STR     r1,[r0]
  MRS     R0, CONTROL     ; Set CONTROL[0] so Thread run in unprivileged state
  ORRS    R0, R0, #1
  MSR     CONTROL, R0
  ORR     LR, #0x1C       ; Change LR to return to thread,using PSP, 
                          ; 8 words stack frame
  BX      LR
  ; -----------------------------------------------------------
svc_service_1	; Service to switch handler back from unprivileged mode to
              ; privileged mode
  MRS     R0, PSP         ; Update stacked PC in original privileged
                          ; stack so that it
  LDR     R1,[R0, #0x18]  ; return to the instruction after 2nd
                          ; SVC in the redirect handler
  STR     R1,[SP, #0x18]	; 
  MRS     R0, PSP         ; Adjust PSP back to what it was
                          ; before 1st SVC
  LDR     R1, =__cpp(&svc_PSP_adjust)
  LDRB    R1, [R1]
  ADDS    R0, R0, R1
  MSR     PSP, R0
  LDR     R0, =0xE000ED14 ; Clear Non-base thread enable in CCR
  LDR     r1,[r0]
  BIC     r1, #1
  STR     r1,[r0]
  MRS     R0, CONTROL     ; Clear CONTROL[0]
  BICS    R0, R0, #1
  MSR     CONTROL, R0
  ORR     LR, #0x10 ; Return using 8 word stack frame
  BIC     LR, #0xC  ; Return to handler mode, using main stack
  BX      LR
Unknown_SVC_Request ; Output error message
  BL      __cpp(Unknown_SVC_Request_Msg)
  B       .
  ALIGN
}
void Unknown_SVC_Request_Msg(unsigned int svc_num)
{
	printf("Error: Unknown SVC service request %d\n", svc_num);
	while(1);
}
/*---------------------------------------*/
void EXTI0_IRQHandler(void)
{
  printf ("[EXTI0_IRQHandler]\n");
  ext0_irq_occurred ++;
  return;
}	
/*---------------------------------------*/
void EXTI1_IRQHandler(void)
{
  printf ("[EXTI1_IRQHandler]\n");
  ext1_irq_occurred ++;
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
