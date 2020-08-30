#include "stm32f4xx.h"
#include <stdio.h>
#define HW_REG32(addr) (*((volatile uint32_t *)(addr)))
typedef struct
{
  __IO uint32_t CTRL;   /*!< Flash Patch Control Register,                Address offset: 0x00 */
  __IO uint32_t REMAP;  /*!< Flash Patch Remap Register,                  Address offset: 0x04 */      
  __IO uint32_t COMP[8];/*!< Flash Patch Comparator Register #0 to #8,    Address offset: 0x08 -0x24 */
} FPB_TypeDef;

#define FPB_BASE           0xE0002000UL
#define FPB                ((FPB_TypeDef *) FPB_BASE)

       void function1(void);
extern void function2(void);  // Done in assembly code
       void function2b(void);
extern void fpb_test1(void);  // forward  BL test
extern void fpb_test2(void);  // backward BL test
void        fpb_setup(uint32_t test_func_addr, uint32_t offset, uint32_t updated_func_addr, uint32_t p_table_addr);
uint32_t    bl_inst_encode(uint32_t b_src, uint32_t b_dest);
void Delay(uint32_t nCount);
void LED_config(void);
void blinky(void);

int main(void)
{
  SCB->CCR |= SCB_CCR_STKALIGN_Msk; // Enable double word stack alignment 
  //(recommended in Cortex-M3 r1p1, default in Cortex-M3 r2px and Cortex-M4)
  printf ("Test start\n");
  // Setup the FPB
  // - offset is 2 due to the PUSH instruction
  // - patch table located in 0x20010000
  fpb_setup(((uint32_t) fpb_test1), 2, ((uint32_t) function2), 0x20010000UL );
  fpb_test1();  // Test the patch with forward branch
  // Setup the FPB
  // - offset is 2 due to the PUSH instruction
  // - patch table located in 0x20010000
  fpb_setup(((uint32_t) fpb_test2), 2, ((uint32_t) function2), 0x20010000UL );
  fpb_test2();  // Test the patch with backward branch
  printf ("Test end\n");
  LED_config();
  blinky();
}
// Original code print out this
void function1(void)
{
  printf ("Function 1\n");
  return;
}	
// Patched code print out this
void function2b(void)
{
  printf ("Function 2\n");
  return;
}	


// Setup the FPB to patch the call to function1 to become function2
void fpb_setup(uint32_t test_func_addr, uint32_t offset, uint32_t updated_func_addr, uint32_t p_table_addr )
{
  uint32_t bl_addr;                     // address of the BL instruction
  uint32_t bl_inst;                     // branch instruction
	uint32_t temp;
  bl_addr = test_func_addr + offset;    // including offset for additional instructions before BL
  // Generate BL instruction
  bl_inst = bl_inst_encode(bl_addr,updated_func_addr);
  // Note: In real situation we don't have to calculate all these in runtime.
  //       The program to be patched is fixed in OTP ROM/mask ROM already
  if ((bl_addr & 0x2) == 0) {
  // BL address is aligned
    HW_REG32(p_table_addr) = bl_inst;  // First entry of remap table in RAM for COMP0
    FPB->COMP[0] = (((unsigned int) bl_addr) & 0xFFFFFFFD) | 1;// LSB Enable comparator
    FPB->COMP[1] = 0; // Disable
  } else {
    // BL address is unaligned, need 2 comparators
    temp = (bl_addr & 0xFFFFFFFC); // Aligned address
    HW_REG32(p_table_addr+0) = HW_REG32(temp);     // Copy original
    HW_REG32(p_table_addr+4) = HW_REG32(temp + 4); // Copy original
    HW_REG32(p_table_addr+2) = bl_inst;            // Unaligned transfer
    FPB->COMP[0] = (temp  ) | 1;// LSB Enable comparator
    FPB->COMP[1] = (temp+4) | 1;// LSB Enable comparator
  }
  FPB->REMAP = p_table_addr; // Set remap table base
  FPB->CTRL  = 3;  // Enable
  return;
}
// Encode BL instruction
// bit [15:0]  - first half word
// bit [31:16] - second half word
uint32_t bl_inst_encode(uint32_t b_src, uint32_t b_dest)
{
  uint32_t   bl_offset;       // Offset of branch
  uint32_t   bl_offset_comp;  // Bitwise inverted value
  uint32_t   bl_inst;         // Encoded BL instruction

  bl_offset      =  b_dest - b_src - 4; // extra 4 bytes is needed due to pipeline
  bl_offset_comp = (b_dest > b_src) ? bl_offset : ~bl_offset;

  bl_inst = 0xD000F000UL |                 // BL instruction encoding
      (( bl_offset >> 14) & 0x00000400UL) | // S (offset bit 24 -> bit 10)
      ((~bl_offset_comp <<  6) & 0x20000000UL) | // J1 (offset bit 23 -> bit 29)
      ((~bl_offset_comp <<  5) & 0x08000000UL) | // J2 (offset bit 22 -> bit 27)
      (( bl_offset >> 12) & 0x000003FFUL) | // imm10 offset[21:12] -> bit[9:0]
      (( bl_offset << 15) & 0x07FF0000UL) ; // imm11 = offset[11:1] -> bit[26:16]
  return (bl_inst);
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
