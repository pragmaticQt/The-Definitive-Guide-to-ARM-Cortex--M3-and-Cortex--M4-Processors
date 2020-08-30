#include "stm32f4xx.h"
#include "stdio.h"
#include <cmsis_os.h>

/* Declare message queue */
osMessageQDef(numseq_q, 4, uint32_t); // Declare a Message queue
osMessageQId numseq_q_id;             // Declare a ID for message queue

/* Function declaration */
void receiver(void const *argument); // Thread
/* Thread IDs */
osThreadId t_receiver_id;

// --------------------------------------------------------
// Receiver thread
void receiver(void const *argument) {
  while(1) {
    osEvent evt = osMessageGet(numseq_q_id, osWaitForever);
    if (evt.status == osEventMessage) { // message received
      printf ("%d\n", evt.value.v); // ".v" indicate message as 32-bit value
      }
  } // end while
}
  // define thread function
  osThreadDef(receiver, osPriorityNormal, 1, 0);
// --------------------------------------------------------
int main(void)
{
  uint32_t i=0;
  // Create Message queue
  numseq_q_id = osMessageCreate(osMessageQ(numseq_q), NULL);

  // Create "receiver" thread
  t_receiver_id = osThreadCreate(osThread(receiver), NULL); 
    
  // main() itself is a thread that send out message
  while(1) {   
    i++;
    osMessagePut(numseq_q_id, i, osWaitForever);	
    osDelay(1000);          // delay 1000 msec
    }
} // end main
// --------------------------------------------------------
