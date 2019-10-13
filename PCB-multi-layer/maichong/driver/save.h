/******************** (C) COPYRIGHT 2017 CETC27-CETIIC-ZYGD ********************
* File Name          : filemap.h
* Author             : WZJ
* Version            : V1.0
* Date               : 2017/05/12
* Description        :
*******************************************************************************/

#ifndef __SAVE_H__
#define __SAVE_H__

#include "stm32f10x_lib.h"

extern u8 Data_Buf[][256];
extern u32 BaseAdress, WR_Adress;

void Read_Flash2Buffer(void);
void Write_Buffer2Flash(void);
#endif

/******************** (C) COPYRIGHT 2017 CETC27-CETIIC-ZYGD ********************/
