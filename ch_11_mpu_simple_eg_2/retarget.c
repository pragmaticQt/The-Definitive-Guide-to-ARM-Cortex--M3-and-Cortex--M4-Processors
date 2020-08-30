/*
 *-----------------------------------------------------------------------------
 * The confidential and proprietary information contained in this file may
 * only be used by a person authorised under and to the extent permitted
 * by a subsisting licensing agreement from ARM Limited.
 *
 *            (C) COPYRIGHT 2010-2012 ARM Limited.
 *                ALL RIGHTS RESERVED
 *
 * This entire notice must be reproduced on all copies of this file
 * and copies of this file may only be made by a person if such person is
 * permitted to do so under the terms of a subsisting license agreement
 * from ARM Limited.
 *
 *-----------------------------------------------------------------------------
 */


#if defined ( __CC_ARM   )
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

volatile int32_t ITM_RxBuffer;

int fputc(int ch, FILE *f) {
  return (ITM_SendChar(ch));
}

int fgetc(FILE *f) {
  return (ITM_SendChar(ITM_ReceiveChar()));
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

#elif defined ( __ICCARM__ )
/******************************************************************************/
/* Retarget functions for IAR Embedded Workbench for ARM Embedded Processors  */
/******************************************************************************/
#include "stm32f4xx.h"

int putchar( int character )
{
  return(ITM_SendChar(character));
}


#elif defined ( __GNUC__ )


/******************************************************************************/
/* Retarget functions for GNU Tools for ARM Embedded Processors               */
/******************************************************************************/
#include <stdio.h>
#include <sys/stat.h>
#include "stm32f4xx.h"

int i;


int _write_r(void *reent, int fd, char *ptr, size_t len)
{
  size_t i;
  for (i=0; i<len;i++) {
    ITM_SendChar(ptr[i]); // call character output function in uart_io.c
    }
  return len;
}

#endif
