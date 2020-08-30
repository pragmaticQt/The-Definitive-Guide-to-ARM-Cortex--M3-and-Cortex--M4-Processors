#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "gpio_cfg_defs.h"

// CharLCD connections to GPIO port C
#define CLCD_RS_PIN  12
#define CLCD_RW_PIN  11
#define CLCD_E_PIN   10

#define CLCD_BUSY_FLAG 0x80

extern void GPIO_pin_set(GPIO_TypeDef* GPIOx, uint32_t pin_num);
extern void GPIO_pin_clear(GPIO_TypeDef* GPIOx, uint32_t pin_num);
extern void GPIO_pin_config(GPIO_TypeDef* GPIOx, uint32_t pin_num, uint32_t cfg, uint32_t val);

#define  DELAY_1 
#define  DELAY_2 __NOP();
#define  DELAY_3 __ISB();

/* Character LCD command level */
void     clcd_reset(void);
void     clcd_putc(char ch);
void     clcd_clear_screen(void);
void     clcd_cursor_home(void);
void     clcd_cursor_set(uint32_t x,uint32_t y);
void     clcd_set_cg_ram_addr(uint8_t addr);
void     clcd_set_dd_ram_addr(uint8_t addr);
uint8_t  clcd_read_addr(void);
void     clcd_set_cg(uint8_t ch, const char* pattern);

/* Character LCD interface level */
void     clcd_interface_init(void);  // Initialize Character LCD interface I/O configurations
void     clcd_write_ir(uint8_t cmd);
void     clcd_write_dr(uint8_t cmd);
uint32_t clcd_read_ir(void);
uint32_t clcd_read_dr(void);
void     clcd_wait_if_busy(void);

/* I/O pin level */
void     clcd_data_set_out(void); // Set data bus direction as output
void     clcd_data_set_in(void);  // Set data bus direction as input
void     clcd_data_out_wr(uint8_t val);// set data bus output register value
uint32_t clcd_data_read(void);         // read data bus
void     clcd_E_pin_set(void);   // set E pin to 1
void     clcd_E_pin_clear(void); // set E pin to 0
void     clcd_RS_pin_set(void);  // set RS pin to 1
void     clcd_RS_pin_clear(void);// set RS pin to 0
void     clcd_RW_pin_set(void);  // set RW pin to 1
void     clcd_RW_pin_clear(void);// set RW pin to 0

//-----------------------------
void  clcd_interface_init(void)
{
   clcd_data_set_in(); // Set data direction to input
   
   // Set E to output direction, starting value 0
   GPIO_pin_config(GPIOC, CLCD_E_PIN, 
     (CFG_GPIO_PIN_MODE_OUTPUT_50MHZ|CFG_GPIO_PIN_CNF_OUTPUT_GPO_PUSHPULL),0);

   // Set RS to output direction, starting value 0
   GPIO_pin_config(GPIOC, CLCD_RS_PIN, 
     (CFG_GPIO_PIN_MODE_OUTPUT_50MHZ|CFG_GPIO_PIN_CNF_OUTPUT_GPO_PUSHPULL),0);

   // Set RW to output direction, starting value 0
   GPIO_pin_config(GPIOC, CLCD_RW_PIN, 
     (CFG_GPIO_PIN_MODE_OUTPUT_50MHZ|CFG_GPIO_PIN_CNF_OUTPUT_GPO_PUSHPULL),0);

   return;
}
//-----------------------------
// I/O Level control
void  clcd_E_pin_set(void)
{
		GPIO_pin_set(GPIOC,CLCD_E_PIN);     // set E to 1
	  return;
}
void  clcd_E_pin_clear(void)
{
		GPIO_pin_clear(GPIOC,CLCD_E_PIN);   // set E to 0
	  return;
}
void  clcd_RS_pin_set(void)
{
		GPIO_pin_set(GPIOC,CLCD_RS_PIN);    // set RS to 1
	  return;
}
void  clcd_RS_pin_clear(void)
{
		GPIO_pin_clear(GPIOC,CLCD_RS_PIN);  // set RS to 0
	  return;
}
void  clcd_RW_pin_set(void)
{
		GPIO_pin_set(GPIOC,CLCD_RW_PIN);    // set RW to 1
	  return;
}
void  clcd_RW_pin_clear(void)
{
		GPIO_pin_clear(GPIOC,CLCD_RW_PIN);  // set RW to 0
	  return;
}
void  clcd_data_out_wr(uint8_t val) // Set data output register
{
   uint32_t  tmp_reg;	 // temporary variable
	 tmp_reg    = GPIOC->ODR & ~(0xFF); // Save output status of upper 8-bit
   GPIOC->ODR = tmp_reg | val;		    // Set output data register
   return;
}	
//-----------------------------
// Set lower byte data direction as input
void clcd_data_set_in(void)
{
  GPIOC->CRL = ((CFG_GPIO_PIN_MODE_INPUT | CFG_GPIO_PIN_CNF_INPUT_FLOATING_IN) << 28) |
               ((CFG_GPIO_PIN_MODE_INPUT | CFG_GPIO_PIN_CNF_INPUT_FLOATING_IN) << 24) |
               ((CFG_GPIO_PIN_MODE_INPUT | CFG_GPIO_PIN_CNF_INPUT_FLOATING_IN) << 20) |
               ((CFG_GPIO_PIN_MODE_INPUT | CFG_GPIO_PIN_CNF_INPUT_FLOATING_IN) << 16) |
               ((CFG_GPIO_PIN_MODE_INPUT | CFG_GPIO_PIN_CNF_INPUT_FLOATING_IN) << 12) |
               ((CFG_GPIO_PIN_MODE_INPUT | CFG_GPIO_PIN_CNF_INPUT_FLOATING_IN) <<  8) |
               ((CFG_GPIO_PIN_MODE_INPUT | CFG_GPIO_PIN_CNF_INPUT_FLOATING_IN) <<  4) |
               ((CFG_GPIO_PIN_MODE_INPUT | CFG_GPIO_PIN_CNF_INPUT_FLOATING_IN) <<  0) ; 
  return;
}

//-----------------------------
// Set lower byte data direction as output
void clcd_data_set_out(void)
{
  GPIOC->CRL = ((CFG_GPIO_PIN_MODE_OUTPUT_50MHZ | CFG_GPIO_PIN_CNF_OUTPUT_GPO_PUSHPULL) << 28) |
               ((CFG_GPIO_PIN_MODE_OUTPUT_50MHZ | CFG_GPIO_PIN_CNF_OUTPUT_GPO_PUSHPULL) << 24) |
               ((CFG_GPIO_PIN_MODE_OUTPUT_50MHZ | CFG_GPIO_PIN_CNF_OUTPUT_GPO_PUSHPULL) << 20) |
               ((CFG_GPIO_PIN_MODE_OUTPUT_50MHZ | CFG_GPIO_PIN_CNF_OUTPUT_GPO_PUSHPULL) << 16) |
               ((CFG_GPIO_PIN_MODE_OUTPUT_50MHZ | CFG_GPIO_PIN_CNF_OUTPUT_GPO_PUSHPULL) << 12) |
               ((CFG_GPIO_PIN_MODE_OUTPUT_50MHZ | CFG_GPIO_PIN_CNF_OUTPUT_GPO_PUSHPULL) <<  8) |
               ((CFG_GPIO_PIN_MODE_OUTPUT_50MHZ | CFG_GPIO_PIN_CNF_OUTPUT_GPO_PUSHPULL) <<  4) |
               ((CFG_GPIO_PIN_MODE_OUTPUT_50MHZ | CFG_GPIO_PIN_CNF_OUTPUT_GPO_PUSHPULL) <<  0) ;
  return;
}
//-----------------------------
// read data bus
uint32_t clcd_data_read(void)
{
	return (GPIOC->IDR & 0xFF);
}

//-----------------------------
// Write to instruction register
void  clcd_write_ir(uint8_t cmd)
{
   clcd_wait_if_busy();
   clcd_RW_pin_clear();    // RW set to 0
   clcd_RS_pin_clear();    // RS set to 0
	
   clcd_data_out_wr(cmd);  // Set data output
   clcd_data_set_out();    // Set data direction to output   
   DELAY_1
   clcd_E_pin_set();       // set E to 1, minimum 220 ns
   DELAY_2
   clcd_E_pin_clear();     // set E to 0
   DELAY_1
   clcd_data_set_in();     // Set data direction to input   

   return;
}
//-----------------------------
// Write to data register
void  clcd_write_dr(uint8_t cmd)
{
   clcd_wait_if_busy();
   clcd_RW_pin_clear();    // RW set to 0
   clcd_RS_pin_set();      // RS set to 1
	
   clcd_data_out_wr(cmd);  // Set data output
   clcd_data_set_out();    // Set data direction to output   
   DELAY_1
   clcd_E_pin_set();       // set E to 1, minimum 220 ns
   DELAY_2
   clcd_E_pin_clear();     // set E to 0
   DELAY_1

   clcd_data_set_in();     // Set data direction to input   
   clcd_RS_pin_clear();    // RS set to 0

   return;
}
//-----------------------------
// Read instruction register
uint32_t clcd_read_ir(void)
{
   uint32_t  tmp_reg;	     // temporary variable

   clcd_RW_pin_set();      // RW set to 1
   clcd_RS_pin_clear();    // RS set to 0
   clcd_data_set_in();     // Set data direction to input   
   DELAY_1
   clcd_E_pin_set();       // set E to 1, minimum 220 ns
   DELAY_2
   tmp_reg = clcd_data_read(); // Sample data
   clcd_E_pin_clear();     // set E to 0
   DELAY_1
   clcd_RW_pin_clear();    // RW set to 0

   return (tmp_reg);
}
//-----------------------------
// Read data register
uint32_t clcd_read_dr(void)
{
   uint32_t  tmp_reg;	     // temporary variable

   clcd_RW_pin_set();      // RW set to 1
   clcd_RS_pin_set();      // RS set to 1
   clcd_data_set_in();     // Set data direction to input   
   DELAY_1
   clcd_E_pin_set();       // set E to 1, minimum 220 ns
   DELAY_2
   tmp_reg = clcd_data_read(); // Sample data
   clcd_E_pin_clear();     // set E to 0
   DELAY_1
   clcd_RW_pin_clear();    // RW set to 0
   clcd_RS_pin_clear();    // RS set to 0
   return (tmp_reg);
}

//-----------------------------
void clcd_wait_if_busy(void)
{
  while (clcd_read_ir() & CLCD_BUSY_FLAG); // Wait if busy is 1
  return;
}
//-----------------------------
void clcd_reset(void)
{
  clcd_write_ir(0x38); // Set function : 
                       // bit 5 (func)=1 ,
                       // bit 4 (DL)  = 1 (8-bit mode) ,
                       // bit 3 (N)   = 1 (2 lines),
                       // bit 2 (F)   = 0 (5x7 matrix size)
  clcd_write_ir(0x0D); // Set display on/off control : 
                       // bit 3 (ctrl)= 1,
                       // bit 2 (D)   = 1 (display on)
                       // bit 1 (C)   = 0 (cursor off)
                       // bit 0 (B)   = 1 (blinky on)
  clcd_write_ir(0x06); // Set entry mode :
                       // bit 2(entry)= 1
                       // bit 1 (I/D) = 1 (increment)
                       // bit 0 (S)   = 0 (cursor move off)
  return;
}
//-----------------------------
// Send a character to Character LCD
void clcd_putc(char ch)
{
  clcd_write_dr(ch);
}
//-----------------------------
void clcd_clear_screen(void)
{
  clcd_write_ir(0x01);  // command to clear screen is 0x01
}
//-----------------------------
void clcd_cursor_home(void)
{
  clcd_write_ir(0x02);  // command to set cursor home is 0x02
}
//-----------------------------
void clcd_cursor_set(uint32_t x,uint32_t y)
{
  const uint8_t mempos[4] = {0x00, 0x40, 0x14, 0x54}; // starting address of each row
  if ((x<4) & (y<20)) { // if input values are valid
    clcd_write_ir((0x80 | (mempos[y] + x)));
	}
  return;
}
//-----------------------------
void clcd_set_cg_ram_addr(uint8_t addr)
{
  clcd_write_ir(0x40 | (addr & 0x3F));  // command to set CG RAM address is 0x40
}
//-----------------------------
void clcd_set_dd_ram_addr(uint8_t addr)
{
  clcd_write_ir(0x80 | (addr & 0x7F));  // command to set DD RAM address is 0x80
}
//-----------------------------
uint8_t clcd_read_addr(void)
{
  return (clcd_read_ir() & 0x7F);
}
//-----------------------------
void clcd_set_cg(uint8_t ch, const char*  pattern)
{
  int i;
  // ch      : character code range 0 to 7
  // pattern : address of pattern (8 bytes)
  clcd_set_cg_ram_addr((ch & 0x7)<<3);
  for (i=0;i<8;i++) {
    clcd_write_dr(pattern[i]);
	}
  clcd_set_dd_ram_addr(0);
  return;
}
