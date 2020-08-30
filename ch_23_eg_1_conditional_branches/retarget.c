/******************************************************************************/
/* Retarget functions for ARM DS-5 Professional / Keil MDK                                 */
/******************************************************************************/

#include <stdio.h>
#include <time.h>
#include <rt_misc.h>
#pragma import(__use_no_semihosting_swi)
#include "stm32f4xx.h"

struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;

volatile int32_t ITM_RxBuffer=0x5AA55AA5; // Initialize as EMPTY

int fputc(int ch, FILE *f) {
  return (ITM_SendChar(ch));
}

int fgetc(FILE *f) {
	char tmp;
	while (ITM_CheckChar()==0); // Wait if buffer is empty
	tmp = ITM_ReceiveChar();
	if (tmp==13) tmp = 10;
  return (ITM_SendChar(tmp));
}

int ferror(FILE *f) {
  /* Your implementation of ferror */
  return EOF;
}

void _ttywrch(int ch) {
  ITM_SendChar (ch);
}

void _sys_exit(int return_code) {
label:  goto label;  /* endless loop */
}

