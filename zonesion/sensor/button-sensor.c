
#include "contiki.h"
#include "sensors.h"
#include "dev/button-sensor.h"

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_exti.h"
#include "misc.h"

/*
 * pa0, pa1,
 */
static void EXTI0_1_Config(void)
{
  /* Enable GPIOA clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  
   GPIO_InitTypeDef GPIO_InitStructure;
  /* Configure PA.00 pin as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Enable AFIO clock */
  //RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

  /* Connect EXTI0 Line to PA.00 pin */
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);

  EXTI_InitTypeDef   EXTI_InitStructure;
  /* Configure EXTI0 line */
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
   EXTI_InitStructure.EXTI_Line = EXTI_Line1;
   EXTI_Init(&EXTI_InitStructure);

  NVIC_InitTypeDef NVIC_InitStructure;
  /* Enable and set EXTI0 Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
  NVIC_Init(&NVIC_InitStructure);
}
static /*CC_AT_DATA*/ struct timer debouncetimer;
static unsigned char bv = 0;
void EXTI0_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line0) != RESET)
  {
     if (Bit_RESET == GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)) {
      timer_set(&debouncetimer, CLOCK_SECOND / 20);
    } else if(timer_expired(&debouncetimer)) {
        bv = 1;
        sensors_changed(&button_sensor);
    }
    /* Clear the  EXTI line 0 pending bit */
    EXTI_ClearITPendingBit(EXTI_Line0);
  }
}
void EXTI1_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line1) != RESET)
  {
    if (Bit_RESET == GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1)) {
      timer_set(&debouncetimer, CLOCK_SECOND / 20);
    } else if(timer_expired(&debouncetimer)) {
        bv = 2;
        sensors_changed(&button_sensor);
    }
    /* Clear the  EXTI line 0 pending bit */
    EXTI_ClearITPendingBit(EXTI_Line1);
  }
}

/*---------------------------------------------------------------------------*/
static
int value(int type)
{
 /* int v;
  v = (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1)<<1) 
    | GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);
  */
  return bv;
}
/*---------------------------------------------------------------------------*/
static
int status(int type)
{
  switch (type) {
  case SENSORS_ACTIVE:
  case SENSORS_READY:
    return 1;
    }
  return 0;
}
/*---------------------------------------------------------------------------*/
static
int configure(int type, int value)
{
  switch(type) {
  case SENSORS_HW_INIT:
    EXTI0_1_Config();
    timer_set(&debouncetimer, 0);
    return 1;
  case SENSORS_ACTIVE:
    if(value) {
      
    } else {
   
    }
    return 1;
  }
  return 0;
}


SENSORS_SENSOR(button_sensor, BUTTON_SENSOR, value, configure, status);
