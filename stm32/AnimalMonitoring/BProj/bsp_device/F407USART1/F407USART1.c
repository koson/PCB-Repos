#include "stm32f4xx.h"
#include "stm32f4xx_usart.h"
//#include "misc.h"
//#include "stdio.h"
//#include "stdarg.h"
#include "F407USART1.h"


#define F407USART1_SENDBYTE(x)		F407USART1_SendByte(x)

void F407USART1_Init(void)
{
}

/*
功能：向串口2发送一个字节数据
参数：将要发送的字节数据
返回值：无
*/
void F407USART1_SendByte(uint8 Data)
{ 
   while (!(USART1->SR & USART_FLAG_TXE)); 
   USART_SendData(USART1, Data); 
}

/*
功能：将一个十六位的数据转换为字符形式输出到串口
参数：value：十六位整形数据
返回值：无
*/
void F407USART1_SendInt_DEC(uint16 value)
{
	uint16 dt=10000;
	uint8 ch=0;
	while(value<dt)
	{
		dt/=10;
		if(dt==0)F407USART1_SENDBYTE('0');
	}
	while(dt)
	{
		ch=value/dt;
		F407USART1_SENDBYTE('0'+ch);
		value-=(ch*dt);
		dt/=10;
	}
}
/*
功能：向串口发送一个字符串数据
参数：*str：字符串 首地址
返回值：无
*/
void F407USART1_SendString(char *str)
{
	u16 i=0;
	while(*(str+i)!=0)
	{
		F407USART1_SENDBYTE(*(str+i));
		i++;
	}
}
/*
功能：将一个浮点型数据转换为字符形式输出，指定输出小数点位数
参数：value：浮点型数据
			point：指定输出小数点位数
返回值：无
*/
void F407USART1_SendDouble_DEC(double value,uint8 point)
{
	u8 i=0;
	double data=0;
	uint64 dat;
	if(value<1)
	{
		F407USART1_SENDBYTE('0');
	}
	for(data=1,i=0;data<value;data*=10,i++);
	//
	for(i+=point,data/=10;i>0;i--)
	{
		if(i==point)F407USART1_SENDBYTE('.');
		dat=value/data;
		data/=10;
		dat%=10;
		F407USART1_SENDBYTE('0'+dat);
	}
}





























