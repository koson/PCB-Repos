/**
  ******************************************************************************
  * @file    timer.c
  * $Author: 飞鸿踏雪 $
  * $Revision: 17 $
  * $Date:: 2014-10-25 11:16:48 +0800 #$
  * @brief   串口相关函数.
  ******************************************************************************
  * @attention
  *
  *<h3><center>&copy; Copyright 2009-2012, EmbedNet</center>
  *<center><a href="http:\\www.embed-net.com">http://www.embed-net.com</a></center>
  *<center>All Rights Reserved</center></h3>
  * 
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _TIMER_H
#define _TIMER_H
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stm32f4xx.h>

/* Exported Functions --------------------------------------------------------*/
void Timer_Config(void);
void Timer_Start(void);
void Timer_Stop(void);
#endif /*_TIMER_H*/

/*********************************END OF FILE**********************************/
