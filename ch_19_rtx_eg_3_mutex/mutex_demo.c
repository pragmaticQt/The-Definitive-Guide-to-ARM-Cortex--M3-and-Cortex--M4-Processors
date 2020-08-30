#include "stm32f4xx.h"
#include <cmsis_os.h>
#include "stdio.h"

/* Thread IDs */
osThreadId t_blinky_id; // Declare a thread ID for blink
/* Declare Mutex */
osMutexDef(PrintLock);   // Declare a Mutex for printf control
/* Mutex IDs */
osMutexId  PrintLock_id; // Declare a Mutex ID for printf control

/* Function declaration */
void blinky(void const *argument); // Thread
void LedOutputCfg(void);           // LED output configuration

// --------------------------------------------------------
// Blinky 
// - toggle LED bit 12 
// - Unprivileged Thread
void blinky(void const *argument) {
  while(1) {
    if (GPIOD->IDR & (1<<12)) {
      GPIOD->BSRRH = (1<<12); // Clear bit 12
    } else {  
      GPIOD->BSRRL = (1<<12); // Set   bit 12
    }
    osDelay(50);       // delay 50 msec
    osMutexWait(PrintLock_id, osWaitForever);
    printf ("blinky is running\n");
    osMutexRelease(PrintLock_id);
    }
  }

  // define blinky_1 as thread function
  osThreadDef(blinky, osPriorityNormal, 1, 0);

  
// --------------------------------------------------------
// - toggle LED bit 13 
// - Unprivileged Thread
int main(void)
{
  LedOutputCfg();  // Initialize LED output
  // Create the printf control Mutex before starting blinky thread
  PrintLock_id = osMutexCreate(osMutex(PrintLock)); 
  osMutexWait(PrintLock_id, osWaitForever);
  printf ("\nMutex Demo\n");
  osMutexRelease(PrintLock_id);
	
  // Create a task "blinky" with priority normal
  t_blinky_id = osThreadCreate(osThread(blinky), NULL); 
    
  // main() itself is another thread
  while(1) {   
    if (GPIOD->IDR & (1<<13)) {
      GPIOD->BSRRH = (1<<13); // Clear bit 13
    } else {  
      GPIOD->BSRRL = (1<<13); // Set   bit 13
    }
    osDelay(50);       // delay 50 msec
    osMutexWait(PrintLock_id, osWaitForever);
    printf ("main() is running\n");
    osMutexRelease(PrintLock_id);
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
