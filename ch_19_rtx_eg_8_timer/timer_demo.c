#include "stm32f4xx.h"
#include <cmsis_os.h>
/* Function declaration */
void toggle_led(void const *argument); // Toggle LED
void LedOutputCfg(void);               // LED output configuration

/* Declare Semaphore */
osTimerDef(LED_1, toggle_led);   // Declare a Timer for LED control
osTimerDef(LED_2, toggle_led);   // Declare a Timer for LED control
osTimerDef(LED_3, toggle_led);   // Declare a Timer for LED control
osTimerDef(LED_4, toggle_led);   // Declare a Timer for LED control
osTimerDef(LED_5, toggle_led);   // Declare a Timer for LED control

/* Timer IDs */
osTimerId LED_1_id, LED_2_id, LED_3_id, LED_4_id,LED_5_id ;
// --------------------------------------------------------
// For each round this function get executed 5 times, 
// with argument = 1,2,3,4,5
void toggle_led(void const *argument)
{
  switch ((int)argument){
    case 1: 
       GPIOD->BSRRL = (1<<12); // Set    bit 12
       osTimerStart(LED_2_id, 500);
      break;
    case 2: 
       GPIOD->BSRRH = (1<<12); // Clear  bit 12
       GPIOD->BSRRL = (1<<13); // Set    bit 13
       osTimerStart(LED_3_id, 500);
       break;
    case 3: 
       GPIOD->BSRRH = (1<<13); // Clear  bit 13
       GPIOD->BSRRL = (1<<14); // Set    bit 14
       osTimerStart(LED_4_id, 500);
       break;
    case 4: 
       GPIOD->BSRRH = (1<<14); // Clear  bit 14
       GPIOD->BSRRL = (1<<15); // Set    bit 15
       osTimerStart(LED_5_id, 500);
       break;
    default:	
       GPIOD->BSRRH = (1<<15); // Clear  bit 15
	}	
}
// --------------------------------------------------------
int main(void)
{
  LedOutputCfg();  // Initialize LED output

  // Timers
  LED_1_id = osTimerCreate(osTimer(LED_1), osTimerPeriodic, (void *)1);
  LED_2_id = osTimerCreate(osTimer(LED_2), osTimerOnce,     (void *)2);
  LED_3_id = osTimerCreate(osTimer(LED_3), osTimerOnce,     (void *)3);
  LED_4_id = osTimerCreate(osTimer(LED_4), osTimerOnce,     (void *)4);
  LED_5_id = osTimerCreate(osTimer(LED_5), osTimerOnce,     (void *)5);

  osTimerStart(LED_1_id, 3000); // Start first timer
    
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
