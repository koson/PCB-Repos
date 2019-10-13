/******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
* File Name          : main.h
* Author             : MCD Application Team
* Version            : V1.0
* Date               : 10/08/2007
* Description        : Header for main.c module
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#include "stm32f10x_lib.h"

#define LEAP 1
#define NOT_LEAP 0

#define RCC_APB2Periph_GPIO_CTRL          RCC_APB2Periph_GPIOC
#define GPIO_PIN_P485_Ctrl                GPIO_Pin_3
#define GPIO_PIN_S485_Ctrl                GPIO_Pin_4
#define GPIO_PIN_RUN_LED                  GPIO_Pin_6

void SysTick_Config(void);
void Delay(u32 nCount);
void Decrement_TimingDelay(void);
void ReadDate(void);
void RTC_Configuration(void);
void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
void USART_Configuration(void);
void IWDOG_Configuration(void);
void TIM_Configuration(void);
void ADC_Configuration(void);
void Trans_Data_Uart1(void);
void Trans_Data_Uart2(void);
void Trans_Data_Uart3(void);
void Set_Date(void);
void Read_Date(void);
void Send_Reset(void);
void Delay1s(u8 Xtime);
void GPRS_APN_Init(void);
void GPRS_IP_PORT_Set(void);
void GPRS_IP_PORT_Read(void);
void GPRS_APN_Set(void);
void GPRS_APN_Read(void);
u8 Check_Sum_Uart1(void);
int Check_Sum_Uart2(void);
int Check_Sum_Uart3(void);
void GPRS_IP_CHN_Set(void);
void GPRS_IP_CHN_Read(void);
u8 clear_data(void);
void Send_Calibration_Para(void);

int wakeup(void);
int collectdata(void);
u16 Organize_RawData(void);
void Computer_DeepData(void);
u16 Organize_DeepData(void);
void ADC_Read(void);

void sendagain_unixtime_correct(void);
void resistance_RawData(void);
#endif /* __MAIN_H */

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
