#include "wwdg.h"

//保存WWDG计数器的设置值,默认为最大. 
u8 WWDG_CNT=0X7F;
//初始化窗口看门狗 	
//tr   :T[6:0],计数器值 
//wr   :W[6:0],窗口值 
//fprer:分频系数（WDGTB）,仅最低2位有效 
//Fwwdg=PCLK1/(4096*2^fprer). 一般PCLK1=42Mhz
void WWDG_Init(u8 tr,u8 wr,u32 fprer)
{
 
	NVIC_InitTypeDef NVIC_InitStructure;
 
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG,ENABLE); //使能窗口看门狗时钟
	
	WWDG_CNT=tr&WWDG_CNT;   //初始化WWDG_CNT. 
	WWDG_SetPrescaler(fprer); //设置分频值
	WWDG_SetWindowValue(wr); //设置窗口值
//	WWDG_SetCounter(WWDG_CNT);//设置计数值
	WWDG_Enable(WWDG_CNT);  //开启看门狗
	
	NVIC_InitStructure.NVIC_IRQChannel=WWDG_IRQn;  //窗口看门狗中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x02;  //抢占优先级为2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03;					//子优先级为3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;  //使能窗口看门狗
	NVIC_Init(&NVIC_InitStructure);
	
	WWDG_ClearFlag();//清除提前唤醒中断标志位
  WWDG_EnableIT();//开启提前唤醒中断
}



//窗口看门狗中断服务程序 
void WWDG_IRQHandler(void)
{
	WWDG_SetCounter(WWDG_CNT); //重设窗口看门狗值
	WWDG_ClearFlag();//清除提前唤醒中断标志位
	LED1=!LED1;
}
