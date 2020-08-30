#include "stm32f4xx.h"
#include <cmsis_os.h>

/* Thread IDs */
osThreadId t_blinky_id1;
osThreadId t_blinky_id2;
osThreadId t_blinky_id3;
osThreadId t_blinky_id4;
/* Declare Semaphore */
osSemaphoreDef(two_LEDs);   // Declare a Semaphore for LED control
/* Semaphore IDs */
osSemaphoreId two_LEDs_id;  // Declare a Semaphore ID for LED control

/* Function declaration */
void blinky(void const *argument); // Thread
void LedOutputCfg(void);           // LED output configuration

// --------------------------------------------------------
// Blinky_1 - toggle LED bit 12 
void blinky_1(void const *argument) {
  while(1) {
    // LED on
    osSemaphoreWait(two_LEDs_id, osWaitForever);
    GPIOD->BSRRL = (1<<12); // Set   bit 12
    osDelay(500);           // delay 500 msec
    GPIOD->BSRRH = (1<<12); // Clear bit 12
    osSemaphoreRelease(two_LEDs_id);

    // LED off
    osDelay(500);           // delay 500 msec    
    }
  }
// --------------------------------------------------------
// Blinky_2 - toggle LED bit 13 
void blinky_2(void const *argument) {
  while(1) {
    // LED on
    osSemaphoreWait(two_LEDs_id, osWaitForever);
    GPIOD->BSRRL = (1<<13); // Set   bit 13
    osDelay(600);           // delay 600 msec
    GPIOD->BSRRH = (1<<13); // Clear bit 13
    osSemaphoreRelease(two_LEDs_id);

    // LED off
    osDelay(600);           // delay 600 msec    
    }
  }
// --------------------------------------------------------
// Blinky_3 - toggle LED bit 14 
void blinky_3(void const *argument) {
  while(1) {
    // LED on
    osSemaphoreWait(two_LEDs_id, osWaitForever);
    GPIOD->BSRRL = (1<<14); // Set   bit 14
    osDelay(700);           // delay 700 msec
    GPIOD->BSRRH = (1<<14); // Clear bit 14
    osSemaphoreRelease(two_LEDs_id);

    // LED off
    osDelay(700);           // delay 700 msec    
    }
  }
// --------------------------------------------------------
// Blinky_4 - toggle LED bit 15 
void blinky_4(void const *argument) {
  while(1) {
    // LED on
    osSemaphoreWait(two_LEDs_id, osWaitForever);
    GPIOD->BSRRL = (1<<15); // Set   bit 15
    osDelay(800);           // delay 800 msec
    GPIOD->BSRRH = (1<<15); // Clear bit 15
    osSemaphoreRelease(two_LEDs_id);

    // LED off
    osDelay(800);           // delay 800 msec    
    }
  }
// --------------------------------------------------------
  // define thread functions
  osThreadDef(blinky_1, osPriorityNormal, 1, 0);
  osThreadDef(blinky_2, osPriorityNormal, 1, 0);
  osThreadDef(blinky_3, osPriorityNormal, 1, 0);
  osThreadDef(blinky_4, osPriorityNormal, 1, 0); 
// --------------------------------------------------------
// - toggle LED bit 13 
// - Unprivileged Thread
int main(void)
{
  LedOutputCfg();  // Initialize LED output
  // Create Semaphore with 2 tokens
  two_LEDs_id = osSemaphoreCreate(osSemaphore(two_LEDs), 2);

  // Create "blinky" threads
  t_blinky_id1 = osThreadCreate(osThread(blinky_1), NULL); 
  t_blinky_id2 = osThreadCreate(osThread(blinky_2), NULL); 
  t_blinky_id3 = osThreadCreate(osThread(blinky_3), NULL); 
  t_blinky_id4 = osThreadCreate(osThread(blinky_4), NULL); 
    
  // main() itself is another thread
  while(1) {   
    osDelay(osWaitForever); // delay 
    }
} // end main
// --------------------------------------------------------
void LedOutputCfg(void)
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
} // end LedOutputCfg
