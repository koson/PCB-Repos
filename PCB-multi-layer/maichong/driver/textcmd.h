/******************** (C) COPYRIGHT 2018 CETC27-CETIIC-ZYGD ********************
* File Name          : textcmd.h
* Author             : WZJ
* Version            : V1.0
* Date               : 2018/11/29
* Description        :
*******************************************************************************/

#ifndef __TEXTCMD_H__
#define __TEXTCMD_H__

#include "stm32f10x_lib.h"

#define textcmd_length 256

extern char textcmd_buf[];
//20190510
extern vu16 textcmd_cnt;
extern vu8 termination_flag;

void text_cmd(void);
#endif

/******************** (C) COPYRIGHT 2018 CETC27-CETIIC-ZYGD ********************/
