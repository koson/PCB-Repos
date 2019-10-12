#include "power.h"
#include "delay.h"
#include "usart.h"
/*
#define VCC_DVC   PBout(13)	
#define VCC_GPS   PBout(14)	
#define VCC_USB   PBout(15)	

#define VCC_WIFI  PCout(4)	
#define VCC_4G    PCout(5)	
 */

/**
 * 模块电源控制
 */
void Power_Ctrl_Init(void)
{    	 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC, ENABLE);//使能GPIOC时钟

	//GPIOB13,GPIOB14,GPIOB15初始化设置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;     //普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;      //上拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);            //初始化
	GPIO_ResetBits(GPIOB,GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);  //GPIOB13,B14,B15设置低
	
	//GPIOC4,GPIOC5初始化设置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;     //普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;      //上拉
	GPIO_Init(GPIOC, &GPIO_InitStructure);            //初始化
	GPIO_ResetBits(GPIOC,GPIO_Pin_4 | GPIO_Pin_5);    //GPIOC0,C1,C2设置低
}


void Cam_Crtl_Init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//使能GPIOA时钟

  //GPIOF9,F10初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化
	
  GPIO_ResetBits(GPIOD,GPIO_Pin_0 | GPIO_Pin_1);//GPIOC8,C9设置高，灯灭
}

//拉高引脚通知相机拍照或录像（根据相机的设置），拉高时间0.5~1s，然后拉低
void USB_Photograph(void)
{	

	USB_PHO = 1;
	printf("拍摄...\r\n");
	delay_ms(1000);
	USB_PHO = 0;	
	printf("ing .....\r\n");

}

//控制相机是否处于连接状态，当电平拉高时，告知相机外部设备要接USB了，外部设备完成USB连接后可以把输入引脚3拉低，即可恢复相机
void USB_Connecting(u8 ctrl)
{	
	if(ctrl==1)
	{
		USB_CNT = 1;
		printf("连接相机...\r\n");
	}else
	{
		USB_CNT = 0;
		printf("恢复相机...\r\n");
	}
	
}

void USB_CONNECT_ON()
{
	VCC_USB = 1;
}
void USB_CONNECT_OFF()
{
	VCC_USB = 0;
}

u8 openUSB(void)
{
	USB_CONNECT_ON();
	delay_ms(100);
	USB_Connecting(1);
	delay_ms(10);
}

u8 closeUSB(void)
{
	delay_ms(10);
	USB_Connecting(0);
}


