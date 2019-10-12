#ifndef __LED_H
#define __LED_H
#include "sys.h"

//LED端口定义
#define LED_G PEout(2)	// 绿灯	 
#define LED_Y PEout(3)	// 黄灯

void LED_Init(void);//初始化		 	

void LED_YELLOW_ON(void);
void LED_YELLOW_OFF(void);
void LED_YELLOW_NOT(void);
void LED_GREEN_ON(void);
void LED_GREEN_OFF(void);
void LED_GREEN_NOT(void);

#endif
