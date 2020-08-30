#include "stm32f4xx.h"
#include "stdio.h"
#define LED0 (1<<12)
#define LED1 (1<<13)
#define LED2 (1<<14)
#define LED3 (1<<15)

#define MPU_DEFS_RASR_SIZE_32B   (0x04 << MPU_RASR_SIZE_Pos)
#define MPU_DEFS_RASR_SIZE_64B   (0x05 << MPU_RASR_SIZE_Pos)
#define MPU_DEFS_RASR_SIZE_128B  (0x06 << MPU_RASR_SIZE_Pos)
#define MPU_DEFS_RASR_SIZE_256B  (0x07 << MPU_RASR_SIZE_Pos)
#define MPU_DEFS_RASR_SIZE_512B  (0x08 << MPU_RASR_SIZE_Pos)
#define MPU_DEFS_RASR_SIZE_1KB   (0x09 << MPU_RASR_SIZE_Pos)
#define MPU_DEFS_RASR_SIZE_2KB   (0x0A << MPU_RASR_SIZE_Pos)
#define MPU_DEFS_RASR_SIZE_4KB   (0x0B << MPU_RASR_SIZE_Pos)
#define MPU_DEFS_RASR_SIZE_8KB   (0x0C << MPU_RASR_SIZE_Pos)
#define MPU_DEFS_RASR_SIZE_16KB  (0x0D << MPU_RASR_SIZE_Pos)
#define MPU_DEFS_RASR_SIZE_32KB  (0x0E << MPU_RASR_SIZE_Pos)
#define MPU_DEFS_RASR_SIZE_64KB  (0x0F << MPU_RASR_SIZE_Pos)
#define MPU_DEFS_RASR_SIZE_128KB (0x10 << MPU_RASR_SIZE_Pos)
#define MPU_DEFS_RASR_SIZE_256KB (0x11 << MPU_RASR_SIZE_Pos)
#define MPU_DEFS_RASR_SIZE_512KB (0x12 << MPU_RASR_SIZE_Pos)
#define MPU_DEFS_RASR_SIZE_1MB   (0x13 << MPU_RASR_SIZE_Pos)
#define MPU_DEFS_RASR_SIZE_2MB   (0x14 << MPU_RASR_SIZE_Pos)
#define MPU_DEFS_RASR_SIZE_4MB   (0x15 << MPU_RASR_SIZE_Pos)
#define MPU_DEFS_RASR_SIZE_8MB   (0x16 << MPU_RASR_SIZE_Pos)
#define MPU_DEFS_RASR_SIZE_16MB  (0x17 << MPU_RASR_SIZE_Pos)
#define MPU_DEFS_RASR_SIZE_32MB  (0x18 << MPU_RASR_SIZE_Pos)
#define MPU_DEFS_RASR_SIZE_64MB  (0x19 << MPU_RASR_SIZE_Pos)
#define MPU_DEFS_RASR_SIZE_128MB (0x1A << MPU_RASR_SIZE_Pos)
#define MPU_DEFS_RASR_SIZE_256MB (0x1B << MPU_RASR_SIZE_Pos)
#define MPU_DEFS_RASR_SIZE_512MB (0x1C << MPU_RASR_SIZE_Pos)
#define MPU_DEFS_RASR_SIZE_1GB   (0x1D << MPU_RASR_SIZE_Pos)
#define MPU_DEFS_RASR_SIZE_2GB   (0x1E << MPU_RASR_SIZE_Pos)
#define MPU_DEFS_RASR_SIZE_4GB   (0x1F << MPU_RASR_SIZE_Pos)
#define MPU_DEFS_RASE_AP_NO_ACCESS       (0x0 << MPU_RASR_AP_Pos)
#define MPU_DEFS_RASE_AP_PRIV_RW         (0x1 << MPU_RASR_AP_Pos)
#define MPU_DEFS_RASE_AP_PRIV_RW_USER_RO (0x2 << MPU_RASR_AP_Pos)
#define MPU_DEFS_RASE_AP_FULL_ACCESS     (0x3 << MPU_RASR_AP_Pos)
#define MPU_DEFS_RASE_AP_PRIV_RO         (0x5 << MPU_RASR_AP_Pos)
#define MPU_DEFS_RASE_AP_RO              (0x6 << MPU_RASR_AP_Pos)
#define MPU_DEFS_NORMAL_MEMORY_WT        (MPU_RASR_C_Msk)
#define MPU_DEFS_NORMAL_MEMORY_WB        (MPU_RASR_C_Msk | MPU_RASR_B_Msk)
#define MPU_DEFS_NORMAL_SHARED_MEMORY_WT (MPU_RASR_C_Msk | MPU_RASR_S_Msk)
#define MPU_DEFS_NORMAL_SHARED_MEMORY_WB (MPU_DEFS_NORMAL_MEMORY_WB | MPU_RASR_S_Msk)
#define MPU_DEFS_SHARED_DEVICE           (MPU_RASR_B_Msk | MPU_RASR_S_Msk)
#define MPU_DEFS_STRONGLY_ORDERED_DEVICE (0x0)

void Delay(uint32_t nCount);
int  mpu_setup(void);
void LED_initize(void);
// -------------------------------------------------------------------------
// Start of main program
int main(void)
{
  SCB->CCR |= SCB_CCR_STKALIGN_Msk; // Enable double word stack alignment 
  //(recommended in Cortex-M3 r1p1, default in Cortex-M3 r2px and Cortex-M4)
  SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk; // Enable MemManage fault

  puts("Simple MPU example");

  mpu_setup();
  LED_initize(); 
  
#define LOOP_COUNT 0x3FFFFF	
  while(1){		
    GPIOD->BSRRL = LED0; // Set   LED0
    Delay(LOOP_COUNT);
    GPIOD->BSRRH = LED0; // Clear LED0
    GPIOD->BSRRL = LED1; // Set   LED1
    Delay(LOOP_COUNT);
    GPIOD->BSRRH = LED1; // Clear LED1
    GPIOD->BSRRL = LED2; // Set   LED2
    Delay(LOOP_COUNT);
    GPIOD->BSRRH = LED2; // Clear LED2
    GPIOD->BSRRL = LED3; // Set   LED3
    Delay(LOOP_COUNT);
    GPIOD->BSRRH = LED3; // Clear LED3
    };
}
// -------------------------------------------------------------------------
void Delay(uint32_t nCount)
{
  while(nCount--)
  {
    __DSB();
  }
}
// -------------------------------------------------------------------------
int mpu_setup(void)
{
  if (MPU->TYPE==0) {return 1;}    // Return 1 to indicate error
  __DMB();                         // Make sure outstanding transfers are done
  MPU->CTRL = 0;                   // Disable the MPU
  MPU->RBAR = 0x08000000 | MPU_RBAR_VALID_Msk | (MPU_RBAR_REGION_Msk & 0);
  MPU->RASR = (MPU_DEFS_RASR_SIZE_1MB         | MPU_DEFS_NORMAL_MEMORY_WT |
              MPU_DEFS_RASE_AP_FULL_ACCESS    | MPU_RASR_ENABLE_Msk); // Flash
  MPU->RBAR = 0x20000000 | MPU_RBAR_VALID_Msk | (MPU_RBAR_REGION_Msk & 1);
  MPU->RASR = (MPU_DEFS_RASR_SIZE_128KB       | MPU_DEFS_NORMAL_MEMORY_WT |
              MPU_DEFS_RASE_AP_FULL_ACCESS    | MPU_RASR_ENABLE_Msk); // SRAM
  MPU->RBAR = GPIOD_BASE | MPU_RBAR_VALID_Msk | (MPU_RBAR_REGION_Msk & 2);
  MPU->RASR = (MPU_DEFS_RASR_SIZE_1KB         | MPU_DEFS_SHARED_DEVICE    |
              MPU_DEFS_RASE_AP_FULL_ACCESS    | MPU_RASR_ENABLE_Msk); // GPIO D
  MPU->RBAR = RCC_BASE   | MPU_RBAR_VALID_Msk | (MPU_RBAR_REGION_Msk & 3);
  MPU->RASR = (MPU_DEFS_RASR_SIZE_1KB         | MPU_DEFS_SHARED_DEVICE    |
              MPU_DEFS_RASE_AP_FULL_ACCESS    | MPU_RASR_ENABLE_Msk); // RCC

  MPU->RBAR = MPU_RBAR_VALID_Msk | 4; // Clear and disable region 4 configuration
  MPU->RASR = 0;   // disable region
  MPU->RBAR = MPU_RBAR_VALID_Msk | 5; // Clear and disable region 5 configuration
  MPU->RASR = 0;   // disable region
  MPU->RBAR = MPU_RBAR_VALID_Msk | 6; // Clear and disable region 6 configuration
  MPU->RASR = 0;   // disable region
  MPU->RBAR = MPU_RBAR_VALID_Msk | 7; // Clear and disable region 7 configuration
  MPU->RASR = 0;   // disable region
  __DSB();  // Ensure MPU settings take effects
  __ISB();  // Sequence instruction fetches using update settings
  MPU->CTRL = MPU_CTRL_ENABLE_Msk; // Enable the MPU
  return 0; // No error
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
  GPIOD->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR12 |
                    GPIO_OSPEEDER_OSPEEDR13 |
                    GPIO_OSPEEDER_OSPEEDR14 |
                    GPIO_OSPEEDER_OSPEEDR15 );
  GPIOD->PUPDR = 0; // No pull up , no pull down  
  return;	
}
// -------------------------------------------------------------------------
void HardFault_Handler(void)
{
 puts ("[HardFault]");
 while (1);

}
// -------------------------------------------------------------------------
void MemManage_Handler(void)
{
 puts ("[MemManage]");
 while (1);
}
// -------------------------------------------------------------------------
