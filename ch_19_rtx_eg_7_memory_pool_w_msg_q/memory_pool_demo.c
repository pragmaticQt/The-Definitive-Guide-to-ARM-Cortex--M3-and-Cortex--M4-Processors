#include "stm32f4xx.h"
#include "stdio.h"
#include <cmsis_os.h>

typedef struct {
	uint32_t length;
	uint32_t width;
	uint32_t height;
} dimension_t;

/* Declare memory pool */
osPoolDef(mpool, 4, dimension_t);
osPoolId  mpool_id;

/* Declare message queue */
osMessageQDef(dimension_q, 4, dimension_t); // Declare a message queue
osMessageQId  dimension_q_id;    // Declare a ID for message queue
/* Note: Message queue has 4 entries, same as memory pool size */

/* Function declaration */
void receiver(void const *argument); // Thread
osThreadId t_receiver_id;/* Thread IDs */

// --------------------------------------------------------
// Receiver thread
void receiver(void const *argument) {
  while(1) {
    osEvent evt = osMessageGet(dimension_q_id, osWaitForever);
    if (evt.status == osEventMessage) { // message received
      dimension_t *rx_data = (dimension_t *) evt.value.p;
        //  ".p" indicate message as pointer
      printf ("Received data: (L) %d, (W), %d, (H) %d\n", 
        rx_data->length,rx_data->width,rx_data->height);
      osPoolFree(mpool_id, rx_data);
      }
  } // end while
}
  // define thread function
  osThreadDef(receiver, osPriorityNormal, 1, 0);
// --------------------------------------------------------
int main(void)
{
  uint32_t i=0;
  dimension_t *tx_data;
  // Create Message queue
  dimension_q_id = osMessageCreate(osMessageQ(dimension_q), NULL);

  // Create Memory pool
  mpool_id = osPoolCreate(osPool(mpool));

  // Create "receiver" thread
  t_receiver_id = osThreadCreate(osThread(receiver), NULL); 
    
  // main() itself is a thread that send out message
  while(1) {   
    osDelay(1000);          // delay 1000 msec
    i++; 
    tx_data = (dimension_t *) osPoolAlloc(mpool_id);
    tx_data->length	= i;    // fake data generation
    tx_data->width	= i + 1;
    tx_data->height	= i + 2;
    osMessagePut(dimension_q_id, (uint32_t)tx_data, osWaitForever);	
    }
} // end main
// --------------------------------------------------------
