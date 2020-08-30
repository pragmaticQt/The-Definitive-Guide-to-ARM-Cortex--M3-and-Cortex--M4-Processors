#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stdio.h"

#define CFG_GPIO_PIN_MODE_INPUT                 0
#define CFG_GPIO_PIN_MODE_OUTPUT_10MHZ          1
#define CFG_GPIO_PIN_MODE_OUTPUT_2MHZ           2
#define CFG_GPIO_PIN_MODE_OUTPUT_50MHZ          3
#define CFG_GPIO_PIN_CNF_INPUT_ANALOG         0x0
#define CFG_GPIO_PIN_CNF_INPUT_FLOATING_IN    0x4
#define CFG_GPIO_PIN_CNF_INPUT_PULL_UP_DOWN   0x8
#define CFG_GPIO_PIN_CNF_OUTPUT_GPO_PUSHPULL  0x0
#define CFG_GPIO_PIN_CNF_OUTPUT_GPO_OPENDRAIN 0x4
#define CFG_GPIO_PIN_CNF_OUTPUT_AFO_PUSHPULL  0x8
#define CFG_GPIO_PIN_CNF_OUTPUT_AFO_OPENDRAIN 0xC

//------------------------------
// Function declarations
void GPIO_start_setup(GPIO_TypeDef* GPIOx);
void GPIO_pin_config(GPIO_TypeDef* GPIOx, uint32_t pin_num, uint32_t cfg, uint32_t val);

void UART_config(USART_TypeDef* USARTx, uint32_t BaudDiv);
void UART1_putc(char ch);
char UART1_getc(void);

int main(void)
{
  SCB->CCR |= SCB_CCR_STKALIGN_Msk; // Enable double word stack alignment 
  //(recommended in r1p1, default in r2px)

  // Enable clocks to all GPIO ports for I/O port initialization
  // Also enable clock to USART1
  RCC->APB2ENR |= 	RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                    RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
					RCC_APB2Periph_GPIOE | RCC_APB2Periph_USART1;

  // Set all GPIO port pins to analog inputs 
  GPIO_start_setup(GPIOA);
  GPIO_start_setup(GPIOB);
  GPIO_start_setup(GPIOC);
  GPIO_start_setup(GPIOD);
  GPIO_start_setup(GPIOE);

  // Disable clocks to GPIO ports B, C, D and E, leaving A for operations
  RCC->APB2ENR &= 	~(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | 
                      RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE);

  // Initialize USART1
  UART_config(USART1, 625);  // 24MHz / 38400 = 625

  // Setup USART1 TXD (GPIOA.9) as output, use alternate function
  GPIO_pin_config(GPIOA, 9,  
     (CFG_GPIO_PIN_MODE_OUTPUT_10MHZ| CFG_GPIO_PIN_CNF_OUTPUT_AFO_PUSHPULL),
     1);

  // Setup USART1 RXD (GPIOA.10) as input
  GPIO_pin_config(GPIOA, 10,  
     (CFG_GPIO_PIN_MODE_INPUT| CFG_GPIO_PIN_CNF_INPUT_FLOATING_IN),
     1);
  
  printf ("Hello World\n\r");

  while(1){ // echo user inputs
	UART1_putc(UART1_getc());
	}
}

// Initialize USART1 to simple polling mode (no interrupt)
void UART_config(USART_TypeDef* USARTx, uint32_t BaudDiv)
{
  USARTx->CR1 = 0;       // Disable UART during reprogramming
  USARTx->BRR = BaudDiv; // Set baud rate
  USARTx->CR2 = 0;       // 1 stop bit
  USARTx->CR3 = 0;       // interrupts and DMA disabled
  USARTx->CR1 = USART_WordLength_8b | USART_Parity_No | 
                USART_Mode_Tx | USART_Mode_Rx | (1<<13); // Enable UART
  return;   
}

// Output a character to USART1
void UART1_putc(char ch)
{ /* Wait until Transmit Empty flag is set */
  while ((USART1->SR & USART_FLAG_TXE) == 0);
  USART1->DR =  ch;  // send a character
  return;

}
// Read a character from USART. If no data received yet, wait
char UART1_getc(void)
{ /* Wait until Receive Not Empty flag is set */
  while ((USART1->SR & USART_FLAG_RXNE) == 0);
  return USART1->DR;
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
    shftval  = (pin_num - 8) << 2; /* multiplies by 4 because each pin take 4 bits of config info */
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
    shftval  = pin_num << 2; /* multiplies by 4 because each pin take 4 bits of config info */
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

