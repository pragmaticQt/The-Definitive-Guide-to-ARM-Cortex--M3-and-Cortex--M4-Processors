#include <LPC43xx.h>

void Delay(uint32_t nCount);

const uint32_t led_mask[] = { 1UL << 11,                /* GPIO1.11           */
                              1UL << 12 };              /* GPIO1.12           */

/* Clock Control Unit register bits */
#define CCU_CLK_CFG_RUN   (1 << 0)
#define CCU_CLK_CFG_AUTO  (1 << 1)
#define CCU_CLK_STAT_RUN  (1 << 0)
#define LOOP_COUNT 0x3FFFFF	

int main (void) {
  SCB->CCR |= SCB_CCR_STKALIGN_Msk; // Enable double word stack alignment 
  //(recommended in Cortex-M3 r1p1, default in Cortex-M3 r2px and Cortex-M4)

  // Configure LED outputs
    // Enable clock and init GPIO outputs */
    LPC_CCU1->CLK_M4_GPIO_CFG  = CCU_CLK_CFG_AUTO | CCU_CLK_CFG_RUN;
    while (!(LPC_CCU1->CLK_M4_GPIO_STAT & CCU_CLK_STAT_RUN));

  LPC_SCU->SFSP2_11  =  0;                              /* GPIO1[11]          */
  LPC_SCU->SFSP2_12  =  0;                              /* GPIO1[12]          */

  LPC_GPIO_PORT->DIR[1] |= (led_mask[0] | led_mask[1]);
  LPC_GPIO_PORT->SET[1]  = (led_mask[0] | led_mask[1]); /* switch LEDs off    */

  while(1){
    LPC_GPIO_PORT->CLR[1] = led_mask[0];// Set   LED 0
    Delay(LOOP_COUNT);
    LPC_GPIO_PORT->SET[1] = led_mask[0];// Clr   LED 0
    LPC_GPIO_PORT->CLR[1] = led_mask[1];// Set   LED 1
    Delay(LOOP_COUNT);
    LPC_GPIO_PORT->SET[1] = led_mask[1];// Clr   LED 1
  };
}
void Delay(uint32_t nCount)
{
  while(nCount--)
  {
		__NOP();
  }
}

