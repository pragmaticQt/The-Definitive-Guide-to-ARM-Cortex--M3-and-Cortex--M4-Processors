#include "stm32f4xx.h"
#include "stdio.h"
#define LED_USE_2MHZ_OUTPUT
//#define LED_USE_25MHZ_OUTPUT
//#define LED_USE_50MHZ_OUTPUT
//#define LED_USE_100MHZ_OUTPUT
#define LED0 (1<<12)
#define LED1 (1<<13)
#define LED2 (1<<14)
#define LED3 (1<<15)
/* Macros for word accesses */
#define HW32_REG(ADDRESS)  (*((volatile unsigned long  *)(ADDRESS)))
/* Use Breakpoint to stop when error is detected (KEIL MDK specific intrinsic) */
/* it can be switch to while(1) if needed */
#define stop_cpu   __breakpoint(0)
void LED_initize(void); // Initialize LED
void HardFault_Handler_C(unsigned long * svc_args, unsigned int lr_value);
__asm void undef(void);
// -------------------------------------------------------------------------
// Start of main program
int main(void)
{
	SCB->CCR |= SCB_CCR_STKALIGN_Msk; // Enable double word stack alignment 
	//(recommended in Cortex-M3 r1p1, default in Cortex-M3 r2px and Cortex-M4)

	puts("Undef instruction");
	LED_initize(); 
  undef();
  while(1){		
		stop_cpu;// Should not be here
	  };
}
// -------------------------------------------------------------------------
__asm void undef(void)
{
    DCI  0xF123
    DCI  0x4567
    BX   LR
}

// -------------------------------------------------------------------------
// Hard Fault handler wrapper in assembly
// It extracts the location of stack frame and passes it to handler
// in C as a pointer. We also extract the LR value as second 
// parameter.
__asm void HardFault_Handler(void)
{
	TST    LR, #4
	ITE    EQ
	MRSEQ  R0, MSP
	MRSNE  R0, PSP
	MOV    R1, LR
	B      __cpp(HardFault_Handler_C)
}	
// Hard Fault handler in C, with stack frame location and LR value
// extracted from the assembly wrapper as input parameters
void HardFault_Handler_C(unsigned long * hardfault_args, unsigned int lr_value)
{
	unsigned long stacked_r0;
  unsigned long stacked_r1;
  unsigned long stacked_r2;
  unsigned long stacked_r3;
  unsigned long stacked_r12;
  unsigned long stacked_lr;
  unsigned long stacked_pc;
  unsigned long stacked_psr;
	unsigned long cfsr;
	unsigned long bus_fault_address;
	unsigned long memmanage_fault_address;

	bus_fault_address       = SCB->BFAR;
	memmanage_fault_address = SCB->MMFAR;
	cfsr                    = SCB->CFSR;
	
  stacked_r0 = ((unsigned long) hardfault_args[0]);
  stacked_r1 = ((unsigned long) hardfault_args[1]);
  stacked_r2 = ((unsigned long) hardfault_args[2]);
  stacked_r3 = ((unsigned long) hardfault_args[3]);
  stacked_r12 = ((unsigned long) hardfault_args[4]);
  stacked_lr  = ((unsigned long) hardfault_args[5]);
  stacked_pc  = ((unsigned long) hardfault_args[6]);
  stacked_psr = ((unsigned long) hardfault_args[7]);

  printf ("[HardFault]\n");
  printf ("- Stack frame:\n"); 
	printf (" R0  = %x\n", stacked_r0);
  printf (" R1  = %x\n", stacked_r1);
  printf (" R2  = %x\n", stacked_r2);
  printf (" R3  = %x\n", stacked_r3);
  printf (" R12 = %x\n", stacked_r12);
  printf (" LR  = %x\n", stacked_lr);
  printf (" PC  = %x\n", stacked_pc);
  printf (" PSR = %x\n", stacked_psr);
  printf ("- FSR/FAR:\n");  
  printf (" CFSR = %x\n", cfsr);
  printf (" HFSR = %x\n", SCB->HFSR);
  printf (" DFSR = %x\n", SCB->DFSR);
  printf (" AFSR = %x\n", SCB->AFSR);
	if (cfsr & 0x0080) printf (" MMFAR = %x\n", memmanage_fault_address);
	if (cfsr & 0x8000) printf (" BFAR = %x\n", bus_fault_address);
  printf ("- Misc\n"); 
	printf (" LR/EXC_RETURN= %x\n", lr_value);
		
  while(1); // endless loop
}

// -------------------------------------------------------------------------
void LED_initize(void)
{
	// Configure LED outputs
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // Enable Port D clock
	 // Set pin 12, 13, 14, 15 as general purpose output mode (pull-push) 
	GPIOD->MODER |= (GPIO_MODER_MODER12_0 |
                   GPIO_MODER_MODER13_0 |
	                 GPIO_MODER_MODER14_0 |
	                 GPIO_MODER_MODER15_0 ) ;
	// GPIOD->OTYPER  |= 0; // No need to change - use pull-push output
#ifdef LED_USE_2MHZ_OUTPUT
	// GPIOD->OSPEEDR |= 0; // No need to change - slow speed
#endif
#ifdef LED_USE_25MHZ_OUTPUT
	GPIOD->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR12_0 |
	                   GPIO_OSPEEDER_OSPEEDR13_0 |
										 GPIO_OSPEEDER_OSPEEDR14_0 |
										 GPIO_OSPEEDER_OSPEEDR15_0 );
#endif
#ifdef LED_USE_50MHZ_OUTPUT
	GPIOD->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR12_1 |
	                   GPIO_OSPEEDER_OSPEEDR13_1 |
										 GPIO_OSPEEDER_OSPEEDR14_1 |
										 GPIO_OSPEEDER_OSPEEDR15_1 );
#endif
#ifdef LED_USE_100MHZ_OUTPUT
	GPIOD->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR12 |
	                   GPIO_OSPEEDER_OSPEEDR13 |
										 GPIO_OSPEEDER_OSPEEDR14 |
										 GPIO_OSPEEDER_OSPEEDR15 );
#endif
	GPIOD->PUPDR = 0; // No pull up , no pull down  
	return;	
}
// -------------------------------------------------------------------------
