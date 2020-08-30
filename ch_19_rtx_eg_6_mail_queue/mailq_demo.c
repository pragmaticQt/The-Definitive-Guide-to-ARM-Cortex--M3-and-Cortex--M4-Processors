#include "stm32f4xx.h"
#include "stdio.h"
#include <cmsis_os.h>

typedef struct {
	uint32_t length;
	uint32_t width;
	uint32_t height;
} dimension_t;

/* Declare message queue */
osMailQDef(dimension_q, 4, dimension_t); // Declare a Mail queue
osMailQId  dimension_q_id;    // Declare a ID for Mail queue

/* Function declaration */
void receiver(void const *argument); // Thread
/* Thread IDs */
osThreadId t_receiver_id;

// --------------------------------------------------------
// Receiver thread
void receiver(void const *argument) {
  while(1) {
    osEvent evt = osMailGet(dimension_q_id, osWaitForever);
    if (evt.status == osEventMail) { // mail received
      dimension_t *rx_data = (dimension_t *) evt.value.p;
        //  ".p" indicate message as pointer
      printf ("Received data: (L) %d, (W), %d, (H) %d\n", 
        rx_data->length,rx_data->width,rx_data->height);
      osMailFree(dimension_q_id, rx_data);
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
  dimension_q_id = osMailCreate(osMailQ(dimension_q), NULL);

  // Create "receiver" thread
  t_receiver_id = osThreadCreate(osThread(receiver), NULL); 
    
  // main() itself is a thread that send out message
  while(1) {   
    osDelay(1000);          // delay 1000 msec
    i++; 
    tx_data = (dimension_t *) osMailAlloc(dimension_q_id, osWaitForever);
    tx_data->length	= i;    // fake data generation
    tx_data->width	= i + 1;
    tx_data->height	= i + 2;
    osMailPut(dimension_q_id, tx_data);	
    }
} // end main
// --------------------------------------------------------
