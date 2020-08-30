#include "stm32f4xx.h"
#include "stdio.h"

/* Semi-hosting functions */
extern char SH_GetChar();
extern void SH_SendChar(int ch);

/* A simple gets implementation with buffer length limit */
int my_gets(char dest[], int length)
{
unsigned int textlen=0; // Current text length
char ch; // current character
do {
  ch = SH_GetChar(); // Get a character from Semihosting
  switch (ch)	{
    case 8: // Back space
     if (textlen>0) {
       textlen--;
       SH_SendChar(ch); // Back space
       SH_SendChar(' '); // Replace last character with space on console
       SH_SendChar(ch); // Back space again to adjust cursor position
       }
      break;
    case 13: // Enter is pressed
     dest[textlen] = 0; // null terminate
     SH_SendChar(ch); // echo typed character
     break;
    case 27: // ESC is pressed
     dest[textlen] = 0; // null terminate
     SH_SendChar('\n');
     break;
    default: // if input length is within limit and input is valid
      if ((textlen<length) &
        ((ch >= 0x20) & (ch < 0x7F))) // valid characters
         {
         dest[textlen] = ch; // append character to buffer
         textlen++;
         SH_SendChar(ch); // echo typed character
         }
      break;
     } // end switch
  } while ((ch!=13) && (ch!=27));
  if (ch==27) {
    return 1; // ESC key pressed
  } else {
    return 0; // Return key pressed
    }
}

int main(void)
{
  char TextBuffer[40];

  SCB->CCR |= SCB_CCR_STKALIGN_Msk; // Enable double word stack alignment
  //(recommended in Cortex-M3 r1p1, default in Cortex-M3 r2px and Cortex-M4)
  printf("Hello world!\n");
  while (1){
    printf("\nEnter text:\n");
    my_gets(&TextBuffer[0], sizeof(TextBuffer)-1);
    printf("\n%s\n", &TextBuffer[0]);
  }
}


