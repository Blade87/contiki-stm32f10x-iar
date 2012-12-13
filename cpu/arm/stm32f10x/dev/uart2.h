#ifndef __UART2_H__
#define __UART2_H__

void uart2_int(unsigned int bpr);
void uart1_putc(unsigned char x);
void uart2_set_input(int(*fun)(unsigned char));
#endif