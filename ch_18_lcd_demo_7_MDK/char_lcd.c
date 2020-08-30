#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stdio.h"
#include "gpio_cfg_defs.h"

void GPIO_start_setup(GPIO_TypeDef* GPIOx);
void GPIO_pin_config(GPIO_TypeDef* GPIOx, uint32_t pin_num, uint32_t cfg, uint32_t val);
void GPIO_pin_set(GPIO_TypeDef* GPIOx, uint32_t pin_num);
void GPIO_pin_clear(GPIO_TypeDef* GPIOx, uint32_t pin_num);
void Delay(uint32_t count);

//------------------------------
// Character LCD functions
extern void     clcd_reset(void);
extern void     clcd_putc(char ch);
extern void     clcd_clear_screen(void);
extern void     clcd_cursor_home(void);
extern void     clcd_cursor_set(uint32_t x,uint32_t y);
extern void     clcd_interface_init(void);
extern void     clcd_set_cg(uint8_t ch, const char* pattern);
extern uint8_t  clcd_read_addr(void);

//------------------------------

int main(void)
{
  // Bit pattern for creating user character, in this case a smiley
  const char smiley[8] = { 0x00,  // 00000
                           0x11,  // 10001
                           0x00,  // 00000
                           0x00,  // 00000
                           0x11,  // 10001
                           0x0A,  // 01010
                           0x04,  // 00100
                           0x00}; // 00000
  
  SCB->CCR |= SCB_CCR_STKALIGN_Msk; // Enable double word stack alignment 
  //(recommended in r1p1, default in r2px)

  // Enable clocks to all GPIO ports for I/O port initialization
  RCC->APB2ENR |= 	RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                    RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
					RCC_APB2Periph_GPIOE;

  // Set all GPIO port pins to analog inputs 
  GPIO_start_setup(GPIOA);
  GPIO_start_setup(GPIOB);
  GPIO_start_setup(GPIOC);
  GPIO_start_setup(GPIOD);
  GPIO_start_setup(GPIOE);

  // Disable clocks to all GPIO ports B, D and E, leaving A and C for operations
  RCC->APB2ENR &= ~(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD |
                    RCC_APB2Periph_GPIOE);

  // Setup LED (GPIOC.8) as output, default value = 0
  GPIO_pin_config(GPIOC, 8,  
     (CFG_GPIO_PIN_MODE_OUTPUT_2MHZ| CFG_GPIO_PIN_CNF_OUTPUT_GPO_PUSHPULL),
     0);

  // Setup LED (GPIOC.9) as output, default value = 0
  GPIO_pin_config(GPIOC, 9,  
     (CFG_GPIO_PIN_MODE_OUTPUT_2MHZ| CFG_GPIO_PIN_CNF_OUTPUT_GPO_PUSHPULL),
     0);

  // Setup Push Button (GPIOA.0) as input, default value = 0
  GPIO_pin_config(GPIOA, 0,  
     (CFG_GPIO_PIN_MODE_INPUT| CFG_GPIO_PIN_CNF_INPUT_FLOATING_IN),
     0);
  
  clcd_interface_init();	    // Initialize I/O port C for connection to Character LCD
  clcd_reset();               // Reset Character LCD module
  clcd_clear_screen();		    // Clear the screen
  clcd_set_cg(0, &smiley[0]); // set first user character to smiley (max 8 user created characters
                              // at the end of this function, it automatically switch back to DD RAM
                              // with address = 0
  printf ("Hello World ");
  clcd_putc(0); 			  // Display smiley character created eariler

  clcd_cursor_set(0,1);		  // Set cursor location to 1st character of 2nd row
  printf ("0123456789012345");


  while (1) {
     Delay(2400000);
	 if  ((GPIOA->IDR & 0x01)==0) { // Key NOT pushed
	   if (GPIOC->IDR & 0x100) 	{GPIO_pin_clear(GPIOC, 8);} // toggle LED
	   else 	                  {GPIO_pin_set(GPIOC, 8);}

	   if (GPIOC->IDR & 0x200) 	{GPIO_pin_clear(GPIOC, 9);} // toggle LED
	   else 	                  {GPIO_pin_set(GPIOC, 9);}

	   }
	 else {
	   GPIO_pin_clear(GPIOC, 8); // LED off
	   GPIO_pin_clear(GPIOC, 9); // LED off
       }
  };
}

void GPIO_start_setup(GPIO_TypeDef* GPIOx)
{
  // Set all pins as analog input mode
  GPIOx->CRL = 0; // Bit 0 to 7, all set as analog input
  GPIOx->CRH = 0; // Bit 8 to 15, all set as analog input
  GPIOx->ODR = 0; // Default output value is 0
  return;
}

void GPIO_pin_config(GPIO_TypeDef* GPIOx, uint32_t pin_num, uint32_t cfg, uint32_t val)
{
  uint32_t  tmp_reg;	 // temporary variable
  uint32_t  shftval;	 // number of bit to shift
  if (pin_num > 7) {	 // Use CRH
    shftval  = (pin_num - 8) << 2; // multipile by 4 because each pin take 4 bits of config info
    tmp_reg  = GPIOx->CRH; // Read current configuration
    tmp_reg &= ~(0xF << shftval); // Clear old config
    tmp_reg |=   cfg << shftval;  // Set new config
    if (val != 0){ // If val is non-zero, set output data bit to 1
      GPIOx->BSRR = 1<< pin_num; // set bit
      }
    else {
      GPIOx->BRR  = 1<< pin_num;; // clear bit
      }
    GPIOx->CRH = tmp_reg;         // Write to config reg
    }
  else {				 // Use CRL
    shftval  = pin_num << 2; // multipile by 4 because each pin take 4 bits of config info
    tmp_reg  = GPIOx->CRL; // Read current configuration
    tmp_reg &= ~(0xF << shftval); // Clear old config
    tmp_reg |= ~(cfg << shftval); // Set new config
    if (val != 0){ // If val is non-zero, set output data bit to 1
      GPIOx->BSRR = 1<< pin_num; // set bit
      }
	else {
      GPIOx->BRR  = 1<< pin_num;; // clear bit
      }
    GPIOx->CRL = tmp_reg;         // Write to config reg
    }

  return;
}

void GPIO_pin_set(GPIO_TypeDef* GPIOx, uint32_t pin_num)
{
  GPIOx->BSRR = 1<< pin_num; // set bit
  return;
}
void GPIO_pin_clear(GPIO_TypeDef* GPIOx, uint32_t pin_num)
{
  GPIOx->BRR = 1<< pin_num; // clear bit
  return;
}
// Simple delay function
void Delay(uint32_t count)
{
  uint32_t  tmp;	 // temporary variable
  for (tmp = count; tmp > 0; tmp --) {
   __NOP();
   }
  return;
}
