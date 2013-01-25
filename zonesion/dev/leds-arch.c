#include "contiki.h"
#include "dev/leds.h"

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

void leds_arch_init(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Configure GPIO LED pin6 and pin7 as Output push-pull --------------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
}
unsigned char leds_arch_get(void)
{
  unsigned char st = 0;
  st |= 0x01 & ~GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_8);
  st |= 0x02 & ~(GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_9)<<1);
  
  return st;
}
void leds_arch_set(unsigned char leds)
{
  if (leds & 0x01) GPIO_ResetBits(GPIOB, GPIO_Pin_8);
  else GPIO_SetBits(GPIOB, GPIO_Pin_8);
  if (leds & 0x02) GPIO_ResetBits(GPIOB, GPIO_Pin_9);
  else GPIO_SetBits(GPIOB, GPIO_Pin_9);
}