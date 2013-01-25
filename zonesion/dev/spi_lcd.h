/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : SPI_LCD.h
* Author             : MCD Application Team
* Version            : V2.0.1
* Date               : 06/13/2008
* Description        : Header for SPI_LCD.c file.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SPI_LCD_H
#define __SPI_LCD_H

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Uncomment the line corresponding to the STMicroelectronics evaluation board
   used to run the example */

/* Exported functions ------------------------------------------------------- */
/*----- High layer function -----*/
void SPI_LCD_Init(void);

void Display_Clear(unsigned int co);
void Display_Clear_Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int co);
void Output_Pixel(unsigned  int x,unsigned  int y, unsigned int co);
void Display_ASCII8X16(unsigned int x,unsigned  int y, unsigned int co, unsigned char *s);
void Display_ASCII5X8(unsigned  int x,unsigned  int y, unsigned int co, unsigned char *s);

#endif /* __SPI_LCD_H */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
