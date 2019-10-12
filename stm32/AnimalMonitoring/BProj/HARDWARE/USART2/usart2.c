#include "sys.h"
#include "usart2.h"	  


//串口发送缓存区 	
__align(8) u8 USART2_TX_BUF[USART2_MAX_SEND_LEN]; 	//发送缓冲,最大USART2_MAX_SEND_LEN字节
//串口接收缓存区 	
u8 USART2_RX_BUF[USART2_MAX_RECV_LEN]; 				//接收缓冲,最大USART2_MAX_RECV_LEN个字节.


//通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
//任何数据,则表示此次接收完毕.
//接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度
vu16 USART2_RX_STA=0;   	 
void USART2_IRQHandler(void)
{
	u8 res;	    
	//OSIntEnter();    
	if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) != RESET)//接收到数据
	{	 
		res=USART_ReceiveData(USART2); 	
		printf("%c",res);
		if((USART2_RX_STA&0x8000)==0)//接收未完成
		{	
			//if(res==0x3C)USART2_RX_STA=0;  //开始接收
			
			if(res=='>')
			{
				USART2_RX_BUF[USART2_RX_STA&0X3FFF]=res ;
				USART2_RX_STA++;
				USART2_RX_STA|=0x8000;
			}
			else
			{
				USART2_RX_BUF[USART2_RX_STA&0X3FFF]=res ;
				USART2_RX_STA++;
				if(USART2_RX_STA>(USART2_MAX_RECV_LEN-1))USART2_RX_STA=0;//接收数据错误,重新开始接收	
			}		 
		}  		
	}  											 
	//OSIntExit();  											 
}   
//初始化IO 串口3
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率 
void USART2_init(u32 bound)
{  	
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
 
	USART_DeInit(USART2);  //复位串口2
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//使能USART2时钟
	
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //GPIOA2和GPIOA3初始化
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化GPIOA2，和GPIOA3
	
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); //GPIOA2复用为USART2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); //GPIOA3复用为USART2	  
	
	USART_InitStructure.USART_BaudRate = bound;//波特率一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART2, &USART_InitStructure); //初始化串口3
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启中断  
		
	USART_Cmd(USART2, ENABLE);                    //使能串口 
	
 
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
	USART2_RX_STA=0;				//清零 
}

/**
 * @description: 向串口2发送一个字节数据
 * @param uint8_t Data
 * @return: none
 */
void F407USART2_SendByte(uint8_t Data)
{
	while (!(USART2->SR & USART_FLAG_TXE))
		;
	USART_SendData(USART2, Data);
}

/**
 * @description: 向串口2发送指定长度的字节
 * @param uint8_t * Data 数据地址; uint16_t leng 数据局长度  
 * @return: 
 */
void F407USART2_SendBytes(char *Data, uint16_t leng)
{
	uint16_t i = 0;
	for (i = 0; i < leng; i++)
	{
		F407USART2_SendByte(*(Data + i));
	}
}

/**
 * @description: 发送字符串
 * @param char *str string address
 * @return: 
 */
void F407USART2_SendString(char *str)
{
	u16 i = 0;
	while (*(str + i) != 0)
	{
		F407USART2_SendByte(*(str + i));
		i++;
	}
}
