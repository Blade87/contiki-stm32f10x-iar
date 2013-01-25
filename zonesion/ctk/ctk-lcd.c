#include "contiki.h"
#include "ctk/ctk-draw.h"

#define CHW   5
#define CHH   8
#define SCREEN_WIDTH   160/CHW
#define SCREEN_HEIGHT  128/CHH



/******************************************************************************/
unsigned char ctk_arch_isprint(ctk_arch_key_t key)
{
  return  (key>=0x20 && key<=0x7e);
}

/* This function must be implemented specifically for the
   architecture: */
void ctk_arch_draw_char(char c,
			unsigned char xpos,
			unsigned char ypos,
			unsigned char reversedflag,
			unsigned char color)
{
    int x, y;
    char buf[2];
    buf[0] = c;
    buf[1] = 0;
    x = xpos * CHW;
    y = ypos * CHH;
   // printf("dot (%u,%u) %c[%u] color:%u\r\n", xpos, ypos, c, c, color);
    
    if (c == ' ') {
      if (!reversedflag) color = 0x0000;
      Display_Clear_Rect(x, y, CHW, CHH, color);
    } else {
      if (reversedflag){
        Display_Clear_Rect(x, y, CHW, CHH, color);
        color = 0x0000;
      }
      Display_ASCII5X8(x, y, color, buf);
    }
}

unsigned char ctk_arch_keyavail() 
{
  return 0;
}

unsigned char ctk_arch_getkey() 
{
  return 0; 
}