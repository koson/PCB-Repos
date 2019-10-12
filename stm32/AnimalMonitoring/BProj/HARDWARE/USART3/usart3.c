#include "usart3.h"
//===============================================
//=================硬件层板级函数
//===============================================
/**
 * @description: 初始化串口2IO
 * @param none
 * @return: none
 */
void F407USART3_IO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_USART3);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}
/**
 * @description: 初始化串口3中断
 * @param {type} none
 * @return: none
 */
void F407USART3_NVIC_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
/**
 * @description: 初始化串口3，其中包含了IO初始化外设初始化，中断初始化
 * @param uint32_t btl set bound
 * @return: none
 */
void F407USART3_Init(uint32_t btl)
{
	USART_InitTypeDef USART_InitStructure;
	// 打开UART3时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	// 配置IO
	F407USART3_IO_Init();
	USART_DeInit(USART3);
	// 配置串口参数
	USART_InitStructure.USART_BaudRate = btl;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS_CTS;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);
	// 配置中断优先级
	F407USART3_NVIC_Init();
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

	USART_Cmd(USART3, ENABLE);
}
/**
 * @description: 向串口3发送一个字节数据
 * @param uint8_t Data
 * @return: none
 */
void F407USART3_SendByte(uint8_t Data)
{
	while (!(USART3->SR & USART_FLAG_TXE))
		;
	USART_SendData(USART3, Data);
}
/**
 * @description: 向串口3发送指定长度的字节
 * @param uint8_t * Data 数据地址; uint16_t leng 数据局长度  
 * @return: 
 */
void F407USART3_SendBytes(char *Data, uint16_t leng)
{
	uint16_t i = 0;
	for (i = 0; i < leng; i++)
	{
		F407USART3_SendByte(*(Data + i));
	}
}
/**
 * @description: 发送字符串
 * @param char *str string address
 * @return: 
 */
void F407USART3_SendString(char *str)
{
	u16 i = 0;
	while (*(str + i) != 0)
	{
		F407USART3_SendByte(*(str + i));
		i++;
	}
}

#if (F407USART3_RECEIVE_BUFF_ENABLE == 1)
uint8_t F407USART3_buff[F407USART3_RECEIVE_BUFF_SIZE];  //UART3缓存区
volatile uint16_t F407USART3_buffHead = 0; // 头指针
volatile uint16_t F407USART3_buffEnd = 0;  // 尾指针
volatile uint8_t F407USART3_buffOverFlag = 0;  // 覆盖标志
///////////////////////////写入
/**
 * @description: 向F407USART3_buff写入一个字节
 * @param uint8_t data 待写入字节
 * @return: 0 写入失败，1 写入成功
 */
uint8_t F407USART3_buffWrite(uint8_t data)
{
	if (F407USART3_buffHead != F407USART3_buffEnd || F407USART3_buffOverFlag == 0)
	{
		F407USART3_buff[F407USART3_buffEnd] = data;
		F407USART3_buffEnd++;
		if (F407USART3_buffEnd >= F407USART3_RECEIVE_BUFF_SIZE)
		{
			F407USART3_buffEnd = 0;
			F407USART3_buffOverFlag = 1;
		}
		return 1;
	}
	return 0;
}
/**
 * @description: 向F407USART3_buff写入指定长度字符
 * @param uint8_t *data 待写入字符地址, uint16_t length 待写入字符长度
 * @return:  成功向F407USART3_buff写入字符长度
 */
uint8_t F407USART3_buffWrites(uint8_t *data, uint16_t length)
{
	uint16_t i = 0;
	for (i = 0; i < length; i++)
	{
		if (F407USART3_buffWrite(*(data + i)) == 0)
		{
			return 0;
		}
	}
	return i;
}

///////////////////////////读取
/**
 * @description: 从F407USART3_buff中读取一个字节的数据
 * @param uint8_t *data 数据地址
 * @return: 1 成功，0 失败
 * @note 循环覆盖
 */
uint8_t F407USART3_buffRead(uint8_t *data)
{
	if (F407USART3_buffHead != F407USART3_buffEnd || F407USART3_buffOverFlag == 1)
	{
		*data = F407USART3_buff[F407USART3_buffHead];
		F407USART3_buffHead++;
		if (F407USART3_buffHead >= F407USART3_RECEIVE_BUFF_SIZE)
		{
			F407USART3_buffHead = 0;
			F407USART3_buffOverFlag = 0;
		}
		return 1;
	}
	return 0;
}
/**
 * @description: 从F407USART3_buff中读取指定长度的数据到指定地址
 * @param uint8_t *data 指定地址, uint16_t length 指定长度
 * @return: uint16_t 读取的长度
 */
uint16_t F407USART3_buffReads(uint8_t *data, uint16_t length)
{
	uint16_t i = 0;
	for (i = 0; i < length; i++)
	{
		if (F407USART3_buffRead(data + i) == 0)
			return 0;
	}
	return length;
}
/**
 * @description: 获缓存区长度
 * @param {type} none
 * @return: uint16_t 缓存区长度
 */
uint16_t F407USART3_buffLength(void)
{
	return F407USART3_RECEIVE_BUFF_SIZE * F407USART3_buffOverFlag + F407USART3_buffEnd - F407USART3_buffHead;
}
#endif // F407USART3_RECEIVE_BUFF_ENABLE

//===============================================
//=================中断函数
//===============================================
void USART3_IRQHandler(void)
{
	static uint8_t data = 0;
	static uint8_t step = 0;

	//溢出错误，必须处理,否则会导致死机
	if (0x0008 & USART3->SR)
	{
		data = USART3->DR;
		step = 0;
	}
	else if (0x0020 & USART3->SR)
	{
		//读数据会自动清除中断标志位
		data = USART3->DR;
		F407USART3_buffWrite(data);
	}
}
