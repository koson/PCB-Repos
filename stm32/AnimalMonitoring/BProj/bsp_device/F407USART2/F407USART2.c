#include "stm32f4xx.h"
#include "stm32f4xx_usart.h"
#include "misc.h"


#include "F407USART2.h"


#define F407USART2_SENDBYTE(x)		F407USART2_SendByte(x)







//===============================================
//=================硬件层板级函数
//===============================================
void F407USART2_IO_Init(void);
void F407USART2_NVIC_Init(void);

/*
功能：初始化串口2，其中包含了IO初始化外设初始化，中断初始化
参数：无
返回值：无
*/
void F407USART2_Init(void)
{
	USART_InitTypeDef USART_InitStructure;
	//
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	//
	F407USART2_IO_Init();
	//
	USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2,&USART_InitStructure);
	
	//

	F407USART2_NVIC_Init();
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	
	USART_Cmd(USART2,ENABLE);
}

/*
功能：初始化串口2IO
参数：无
返回值：无
*/
void F407USART2_IO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	//
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	//
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*
功能：初始化串口2中断
参数：无
返回值：无
*/
void F407USART2_NVIC_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}



/*
功能：向串口2发送一个字节数据
参数：将要发送的字节数据
返回值：无
*/
void F407USART2_SendByte(uint8 Data)
{ 
   while (!(USART2->SR & USART_FLAG_TXE)); 
   USART_SendData(USART2, Data); 
}

/*
功能：将一个十六位的数据转换为字符形式输出到串口
参数：value：十六位整形数据
返回值：无
*/
void F407USART2_SendInt_DEC(uint16 value)
{
	uint16 dt=10000;
	uint8 ch=0;
	while(value<dt)
	{
		dt/=10;
		if(dt==0)F407USART2_SENDBYTE('0');
	}
	while(dt)
	{
		ch=value/dt;
		F407USART2_SENDBYTE('0'+ch);
		value-=(ch*dt);
		dt/=10;
	}
}
/*
功能：向串口发送一个字符串数据
参数：*str：字符串 首地址
返回值：无
*/
void F407USART2_SendString(char *str)
{
	u16 i=0;
	while(*(str+i)!=0)
	{
		F407USART2_SENDBYTE(*(str+i));
		i++;
	}
}
/*
功能：将一个浮点型数据转换为字符形式输出，指定输出小数点位数
参数：value：浮点型数据
			point：指定输出小数点位数
返回值：无
*/
void F407USART2_SendDouble_DEC(double value,uint8 point)
{
	u8 i=0;
	double data=0;
	uint64 dat;
	if(value<1)
	{
		F407USART2_SENDBYTE('0');
	}
	for(data=1,i=0;data<value;data*=10,i++);
	//
	for(i+=point,data/=10;i>0;i--)
	{
		if(i==point)F407USART2_SENDBYTE('.');
		dat=value/data;
		data/=10;
		dat%=10;
		F407USART2_SENDBYTE('0'+dat);
	}
}








//===============================================
//=================中断函数
//===============================================
void USART2_IRQHandler(void)
{
	static uint8 data=0;
	static uint8 step=0;
	//
	//溢出错误，必须处理,否则会导致死机
	if(0x0008&USART2->SR)
	{
		data=USART2->DR;
		step=0;
	}
	else 	if(0x0020&USART2->SR)
	{
		//读数据会自动清除中断标志位
		data=USART2->DR;
		//==================================================
		#if(F407USART2_RECEIVE_BUFF_ENABLE==1)
			F407USART2_buffWrite(data);
		#endif
	}
	
}



#if(F407USART2_RECEIVE_BUFF_ENABLE==1)

uint8 F407USART2_buff[F407USART2_RECEIVE_BUFF_SIZE];
volatile uint16 F407USART2_buffHead=0;
volatile uint16 F407USART2_buffEnd=0;
volatile uint8 F407USART2_buffOverFlag=0;


uint8 F407USART2_buffWrite(uint8 data)
{
	if(F407USART2_buffHead!=F407USART2_buffEnd||F407USART2_buffOverFlag==0)
	{
		F407USART2_buff[F407USART2_buffEnd]=data;
		F407USART2_buffEnd++;
		if(F407USART2_buffEnd>=F407USART2_RECEIVE_BUFF_SIZE)
		{
			F407USART2_buffEnd=0;
			F407USART2_buffOverFlag=1;
		}
		return 1;
	}
	return 0;
}
uint8 F407USART2_buffWrites(uint8 *data,uint16 length)
{
	uint16 i=0;
	for(i=0;i<length;i++)
	{
		if(F407USART2_buffWrite(*(data+i))==0)
		{
			return 0;
		}
	}
	return i;
}

uint8 F407USART2_buffRead(uint8 *data)
{
	if(F407USART2_buffHead!=F407USART2_buffEnd||F407USART2_buffOverFlag==1)
	{
		*data=F407USART2_buff[F407USART2_buffHead];
		F407USART2_buffHead++;
		if(F407USART2_buffHead>=F407USART2_RECEIVE_BUFF_SIZE)
		{
			F407USART2_buffHead=0;
			F407USART2_buffOverFlag=0;
		}
		return 1;
	}
	return 0;
}

uint16 F407USART2_buffReads(uint8 *data,uint16 length)
{
	uint16 i=0;
	for(i=0;i<length;i++)
	{
		if(F407USART2_buffRead(data+i)==0)return 0;
	}
	return length;
}

uint16 F407USART2_buffLength()
{
	return F407USART2_RECEIVE_BUFF_SIZE*F407USART2_buffOverFlag+F407USART2_buffEnd-F407USART2_buffHead;
}


#endif

























