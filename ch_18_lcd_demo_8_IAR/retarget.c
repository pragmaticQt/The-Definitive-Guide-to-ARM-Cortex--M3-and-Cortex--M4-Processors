
#include <stdio.h>

extern void clcd_putc(char ch);

size_t __write(int handle,const unsigned char *buf,size_t bufSize)
{
   size_t i;
   for (i=0; i<bufSize;i++)
      {
      clcd_putc(buf[i]);
      }
   return i;
}

