#include "UART_TCPbuff.h"
#include "usart.h"
#include "usart.h"
///////////////////Verify received characters
#define WAITSTRINGBUFF_LENGTH 50
uint8_t waitStringBuff[WAITSTRINGBUFF_LENGTH];
static uint8_t waitStringBuffLen = 0;  // 本地计数
static uint8_t waitStringBuffStep = 0;  // 本地计数2
static uint8_t waitStringOK = 0;  // status flag

/**
 * @description: 设置等待指定字符，并在UART_TCPbuff_Run函数中进行判断
 * @param int8_t *str 指定字符串地址, uint8_t strlen 指定字符串长度
 * @return: 识别字符长度
 */
uint8_t waitString_set(char *str, uint8_t strlen)
{
	uint8_t i = 0;
	for (i = 0; i < strlen && i < WAITSTRINGBUFF_LENGTH; i++)
	{
		waitStringBuff[i] = str[i];
	}
	waitStringBuffLen = i;
	waitStringOK = 0;
	return i;
}
/**
 * @description: waitStringOK set function
 * @param uint8_t value set value
 * @return: none
 */
void waitStringOK_set(uint8_t value)
{
	waitStringOK = value;
}
/**
 * @description: 获取waitStringOK的值
 * @param {type}  none
 * @return: none
 */
uint8_t waitStringOK_get(void)
{
	return waitStringOK;
}
/**
 * @description: 等待一个设置过的命令出现
 * @param {type} 
 * @return: 1 成功，0 失败
 */
uint8_t WaitString_OKflag(uint16_t timeout)
{
	OS_ERR err;
	uint16_t time = 0;
	while (!waitStringOK_get())
	{
		OSTimeDly(1, OS_OPT_TIME_DLY, &err);
		time += 1;
		if (time >= timeout)
		{
			return 0;
		}
	}
	return 1;
}

////////////////////设置缓存区2
#define WAITAPPENDBUFF_LENGTH 30
uint8_t waitAppendBuff[WAITAPPENDBUFF_LENGTH];
static uint8_t waitAppendBuffLen = 0;  // 本地计数
static uint8_t waitAppendBuffStep = 0; // 本地计数2
static uint8_t waitAppendOK = 0;	   // status flag
/**
 * @description: 设置带接收缓存区大小
 * @param uint8_t strlen 接收长度
 * @return: none
 */
uint8_t *waitAppend_set(uint8_t strlen)
{
	if (strlen <= WAITAPPENDBUFF_LENGTH)
	{
		waitAppendBuffLen = strlen;
	}
	else
	{
		waitAppendBuffLen = WAITAPPENDBUFF_LENGTH;
	}
	waitAppendOK = 0;
	return waitAppendBuff;
}
/**
 * @description: waitAppendOK set function
 * @param uint8_t value set value
 * @return: none
 */
void waitAppendOK_set(uint8_t value)
{
	waitAppendOK = value;
}
/**
 * @description: 获取waitAppendOK的值
 * @param {type}  none
 * @return: none
 */
uint8_t waitAppendOK_get(void)
{
	return waitAppendOK;
}
/**
 * @description: 等待一个设置过的命令出现
 * @param {type} 
 * @return: 1 成功，0 失败
 */
uint8_t WaitAppend_OKflag(uint16_t timeout)
{
	OS_ERR err;
	uint16_t time = 0;
	while (!waitAppendOK_get())
	{
		OSTimeDly(1, OS_OPT_TIME_DLY, &err);
		time += 1;
		if (time >= timeout)
		{
			time = 0;
			return 0;
		}
	}
	return 1;
}

/////////////////// UART_TCP_buff 缓存区处理程序
uint8_t UART_TCP_buff[UART_TCP_BUFF_MAXSIZE];  //
volatile uint16_t UART_TCP_buffHead = 0;
volatile uint16_t UART_TCP_buffEnd = 0;
volatile uint8_t UART_TCP_buffOverFlag = 0;
/**
 * @description: 向UART_TCP_buff缓存区写入一个字符
 * @param uint8_t data 待写入的字符
 * @return: 0 写入失败，1 写入成功
 */
uint8_t UART_TCP_buffWrite(char data)
{
	if (UART_TCP_buffHead != UART_TCP_buffEnd || UART_TCP_buffOverFlag == 0)
	{
		UART_TCP_buff[UART_TCP_buffEnd] = data;
		UART_TCP_buffEnd++;
		if (UART_TCP_buffEnd >= UART_TCP_BUFF_MAXSIZE)
		{
			UART_TCP_buffEnd = 0;
			UART_TCP_buffOverFlag = 1;
		}
		return 1;
	}
	return 0;
}
/**
 * @description: 向UART_TCP_buff缓存区写入指定长度的字符
 * @param uint8_t *data 字符地址, uint16_t length 字符长度
 * @return: n 写入字符长度， 0 表示写入失败
 */
uint8_t UART_TCP_buffWrites(char *data, uint16_t length)
{
	uint16_t i = 0;
	for (i = 0; i < length; i++)
	{
		if (UART_TCP_buffWrite(*(data + i)) == 0)
		{
			return 0;
		}
	}
	return i;
}
/**
 * @description: 从UART_TCP_buff缓存区读出一个字符
 * @param uint8_t *data 读取的字符存放地址
 * @return: 1 读取成功，0 读取失败
 */
uint8_t UART_TCP_buffRead(char *data)
{
	if (UART_TCP_buffHead != UART_TCP_buffEnd || UART_TCP_buffOverFlag == 1)
	{
		*data = UART_TCP_buff[UART_TCP_buffHead];
		// F407USART1_SendByte(*data);
		UART_TCP_buffHead++;
		if (UART_TCP_buffHead >= UART_TCP_BUFF_MAXSIZE)
		{
			UART_TCP_buffHead = 0;
			UART_TCP_buffOverFlag = 0;
		}
		return 1;
	}
	return 0;
}
/**
 * @description: 从UART_TCP_buff缓存区读出指定长度字符
 * @param uint8_t *data 读取字符存放位置, uint16_t length 读取字符长度
 * @return: n 成功读取的长度， 0 表示读取失败
 */
uint16_t UART_TCP_buffReads(char *data, uint16_t length)
{
	uint16_t i = 0;
	for (i = 0; i < length; i++)
	{
		if (UART_TCP_buffRead(data + i) == 0)
			return 0;
	}
	return length;
}
/**
 * @description: 获取从UART_TCP_buff缓存区长度
 * @param {type} none
 * @return: 缓存区长度
 */
uint16_t UART_TCP_buffLength(void)
{
	return UART_TCP_BUFF_MAXSIZE * UART_TCP_buffOverFlag + UART_TCP_buffEnd - UART_TCP_buffHead;
}

/////////////////// UART_TCP_buff 缓存区处理程序
char UART_TCP_buff2[UART_TCP_BUFF_MAXSIZE]; // 从ec25中提取的有效数据暂存位置
/**
 * @description: 循环获取UART3缓存的数据，解析，并写入到UART_TCP_buff缓存中
 * @param uint8_t (*getbyte)(uint8_t *) 回调函数通过函数，获取指定位置的数据
 * @return: 1 读取完毕
 */
uint8_t UART_TCPbuff_Run(uint8_t (*getbyte)(char *))
{
	char data, temp;
	uint8_t test_test[20];
	static uint8_t datalengArry[6];
	static uint8_t datalengArrycount = 0;
	static uint16_t dataleng = 0;
	static uint8_t step = 0;
	static uint16_t countError = 0;
	static uint16_t dataReceiveIndex = 0;
	// extern void F407USART1_SendByte(uint8_t Data);
	while (getbyte(&data) == 1) // 此处调用的是F407USART3_buffRead
	{
		// check指定字符串后接的内容
		if (waitAppendOK == 0 && waitStringOK == 1)
		{
			#if (EN_log_print >= 2)
			F407USART1_SendByte(data);
			#endif // EN_log_print			
			waitAppendBuff[waitAppendBuffStep++] = data; 
			if ((waitAppendBuffStep >= waitAppendBuffLen) || (temp == '\r' && data == '\n'))
			{
				waitAppendOK_set(1);
			}
			temp = data;
		}
		else
		{
			waitAppendBuffStep = 0;
		}

		// check 指定字符串
		if (waitStringOK == 0 && waitStringBuff[waitStringBuffStep] == data)
		{
			waitStringBuffStep++;
			if (waitStringBuffStep >= waitStringBuffLen)
			{
				waitStringOK_set(1);
			}
		}
		else
		{
			if (waitStringBuff[0] == data)
			{
				waitStringBuffStep = 1;
			}
			else
			{
				waitStringBuffStep = 0;
			}
		}

		// extract valid information
		switch (step)
		{
		// check recv
		case 0:
			if (data == 'r')
				step = 2;
			break;
		case 2:
			if (data == 'e')
				step = 4;
			else if (data != 'r')
				step = 0;
			break;
		case 4:
			if (data == 'c')
				step = 6;
			else if (data == 'r')
				step = 2;
			else
				step = 0;
			break;
		case 6:
			if (data == 'v')
				step = 8;
			else if (data == 'r')
				step = 2;
			else
				step = 0;
			break;
		case 8:
			if (data == '"')
				step = 9;
			else if (data == 'r')
				step = 2;
			else
				step = 0;
			break;
		case 9:
			if (data == ',')
				step = 10;
			else if (data == 'r')
				step = 2;
			else
				step = 0;
			countError = 0;
			break; 
		// it is a very effective way to extract the information.
		// ignore useless characters
		case 10:
			countError++;
			if (countError >= 10)
			{
				step = 0;
				break;
			}
			if (data == ',') // the second comma
			{
				step = 23;
				datalengArrycount = 0;
			}
			break;

		case 23:
			if (data == '\r')
				step = 24;
			else // get the length of the input
			{
				datalengArry[datalengArrycount] = data;
				datalengArrycount++;
				if (datalengArrycount >= 6)
					step = 0;
			}
			break;
		case 24:
			if (data == '\n')
			{
				dataleng = 0;
				dataReceiveIndex = 0;
				for (; datalengArrycount > 0; datalengArrycount--)
				{
					dataleng *= 10;
					dataleng += (datalengArry[dataReceiveIndex] - '0');
					dataReceiveIndex++;
				}
				dataReceiveIndex = 0;
				if (dataleng <= UART_TCP_BUFF_MAXSIZE)
					step = 30;
				else
					step = 0;
			}
			else
				step = 0;
			break;

		case 30:
			UART_TCP_buff2[dataReceiveIndex] = data;
			dataReceiveIndex++;
			if (dataReceiveIndex >= dataleng)
			{
				// printf("ok %d,%d\r\n",dataReceiveIndex,dataleng);
				UART_TCP_buffWrites(UART_TCP_buff2, dataleng);
				step = 0;
			}
			break;
		default:
			break;
		}
	}
	return 1;
}
