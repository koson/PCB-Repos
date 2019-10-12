//#include "delay.h"
#include "usart3.h"  // self
#include "stdarg.h"  // standard arguments, for u3_printf.
#include "stdio.h"	 // standard IO, for u3_printf.
#include "string.h"	 // string function, for u3_printf.
#include "timer.h"	 // for uart3 to detect  idle time

//串口发送缓存区 	
__align(8) u8 USART3_TX_BUF[USART3_MAX_SEND_LEN]; 	//发送缓冲,最大USART3_MAX_SEND_LEN字节
#ifdef USART3_RX_EN   								//如果使能了接收   	  
//串口接收缓存区 	
u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; 				//接收缓冲,最大USART3_MAX_RECV_LEN个字节.


//通过判断接收连续2个字符之间的时间差不大于150ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过150ms,则认为不是1次连续数据.也就是超过150ms没有接收到
//任何数据,则表示此次接收完毕.
//接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度
u16 USART3_RX_STA=0;
//void USART3_IRQHandler(void)
//{
//	u8 res;	    
//	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)//接收到数据
//	{	 

//		res =USART_ReceiveData(USART3);		
//		if((USART3_RX_STA&(1<<15))==0)//接收完的一批数据,还没有被处理,则不再接收其他数据
//		{ 
//			if(USART3_RX_STA<USART3_MAX_RECV_LEN)		//还可以接收数据
//			{
//				TIM_SetCounter(TIM7,0);//计数器清空        				 
//				if(USART3_RX_STA==0)		
//					TIM_Cmd(TIM7, ENABLE);  //使能定时器7 
//				USART3_RX_BUF[USART3_RX_STA++]=res;		//记录接收到的值	 
//			}else 
//			{
//				USART3_RX_STA|=1<<15;	//强制标记接收完成
//			} 
//		}  	
//	}										 
//}

//===============================================
//=================中断函数
//===============================================
void USART3_IRQHandler(void)
{
	static u8 data=0;
	static u8 step=0;
	//
	//溢出错误，必须处理,否则会导致死机
	if(0x0008&USART3->SR)
	{
		data=USART3->DR;
		step=0;
	}
	else if(0x0020&USART3->SR)
	{
		//读数据会自动清除中断标志位
		data=USART3->DR;
		//==================================================
		//==================================================
		F407USART3_buffWrite(data);  // write data to buffer
	}
}

#endif	
//初始化IO 串口3
//bound:波特率	  
void usart3_init(u32 bound)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); //使能GPIOD时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//使能USART3时钟

 	USART_DeInit(USART3);  //复位串口3
	
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_USART3); //GPIOD9复用为USART3
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_USART3); //GPIOD8复用为USART3	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9; //GPIOD8和GPIOD9初始化
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOD,&GPIO_InitStructure); //初始化GPIOD9，和GPIOD8
	
	USART_InitStructure.USART_BaudRate = bound;//波特率 
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  
	USART_Init(USART3, &USART_InitStructure); //初始化串口3
 
	USART_Cmd(USART3, ENABLE);               //使能串口 
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启中断   
	
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	TIM7_Int_Init(1500-1,8400-1);		//150ms中断
	USART3_RX_STA=0;		//清零
	TIM_Cmd(TIM7, DISABLE); //关闭定时器7
  	

}

void F407USART3_SendByte(u8 Data)
{ 
	while (!(USART3->SR & USART_FLAG_TXE)); 
	USART_SendData(USART3, Data); 
}
void F407USART3_SendBytes(u8 *Data,u16 leng)
{ 
	u16 i=0;
	for(i=0;i<leng;i++)
	{
		F407USART3_SendByte(*(Data+i));
	}
}
void F407USART3_SendString(char *str)
{
	u16 i=0;
	while(*(str+i)!=0)
	{
		F407USART3_SendByte(*(str+i));
		i++;
	}
}

#if(F407USART3_RECEIVE_BUFF_ENABLE==1)  // Reveive flag

u8 F407USART3_buff[F407USART3_RECEIVE_BUFF_SIZE];
volatile u16 F407USART3_buffHead=0;
volatile u16 F407USART3_buffEnd=0;
volatile u8 F407USART3_buffOverFlag=0;


u8 F407USART3_buffWrite(u8 data)
{
	if(F407USART3_buffHead!=F407USART3_buffEnd||F407USART3_buffOverFlag==0)
	{
		F407USART3_buff[F407USART3_buffEnd]=data;
		F407USART3_buffEnd++;
		if(F407USART3_buffEnd>=F407USART3_RECEIVE_BUFF_SIZE)
		{
			F407USART3_buffEnd=0;
			F407USART3_buffOverFlag=1;
		}
		return 1;
	}
	return 0;
}
u8 F407USART3_buffWrites(u8 *data,u16 length)
{
	u16 i=0;
	for(i=0;i<length;i++)
	{
		if(F407USART3_buffWrite(*(data+i))==0)
		{
			return 0;
		}
	}
	return i;
}
u8 F407USART3_buffRead(u8 *data)
{
	if(F407USART3_buffHead!=F407USART3_buffEnd||F407USART3_buffOverFlag==1)
	{
		*data=F407USART3_buff[F407USART3_buffHead];
		F407USART3_buffHead++;
		if(F407USART3_buffHead>=F407USART3_RECEIVE_BUFF_SIZE)
		{
			F407USART3_buffHead=0;
			F407USART3_buffOverFlag=0;
		}
		return 1;
	}
	return 0;
}
u16 F407USART3_buffReads(u8 *data,u16 length)
{
	u16 i=0;
	for(i=0;i<length;i++)
	{
		if(F407USART3_buffRead(data+i)==0)return 0;
	}
	return length;
}
u16 F407USART3_buffLength()
{
	return F407USART3_RECEIVE_BUFF_SIZE*F407USART3_buffOverFlag+F407USART3_buffEnd-F407USART3_buffHead;
}

#endif

//void U3_Send_Byte(char byte)
//{
//	USART3->DR=byte;  
//	while((USART3->SR&0X40)==0);//循环发送,直到发送完毕 
//}

//void U3_Send_Date(u8* dbuf,u16 len)
//{
//	u16 j;
//	for(j=0;j<len;j++)//循环发送数据
//	{
//		USART3->DR=dbuf[j]; 
//		while((USART3->SR&0X40)==0);//循环发送,直到发送完毕   
//		// 测试程序，观察数据变化		
//		USART1->DR=dbuf[j];  
//		while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
//	}	
//}


//串口3,printf 函数
//确保一次发送数据不超过USART3_MAX_SEND_LEN字节
void u3_printf(char* fmt,...)  
{  
	u16 i,j;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART3_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART3_TX_BUF);//此次发送数据的长度
	for(j=0;j<i;j++)//循环发送数据
	{
	  while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET);  //等待上次传输完成 
		USART_SendData(USART3,(uint8_t)USART3_TX_BUF[j]); 	 //发送数据到串口3 
	}
	
}
