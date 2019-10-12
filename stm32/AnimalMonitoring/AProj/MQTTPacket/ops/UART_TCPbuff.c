#include "UART_TCPbuff.h"  // self
#include "usart3.h"  // from
#include "usart.h"  // uart debug
/**
* @name Wait part
* @{
*/
#define WAITSTRINGBUFF_LENGTH		50
u8 waitStringBuff[WAITSTRINGBUFF_LENGTH];
u8 waitStringBuffLen=0;
u8 waitStringBuffStep=0;
u8 waitStringOK=0;
/** @} */
/**
  * @brief  set param of waiting string
  * @param  waiting string
  * @param  the length of waiting string
  *
  * @retval length of waiting string
 */
u8 waitString_set(s8 *str,u8 strlen)
{
	u8 i=0;
	for(i=0;i<strlen&&i<WAITSTRINGBUFF_LENGTH;i++)
	{
		waitStringBuff[i]=str[i];
	}
	waitStringBuffLen=i;
	waitStringOK=0;
	return i;
}
// set waiting flag
void waitStringOK_set(u8 value)
{
	waitStringOK=value;
}
// clear waiting flag
void waitStringOK_Cleare()
{
	waitStringOK=0;
}
// get waiting flag
u8 waitStringOK_get()
{
	return waitStringOK;
}


/**
* @name Uart buf
* @{
*/
u8 UART_TCP_buff[UART_TCP_BUFF_MAXSIZE];
vu16 UART_TCP_buffHead=0;
vu16 UART_TCP_buffEnd=0;
vu8 UART_TCP_buffOverFlag=0;
/** @} */


/**
  * @brief  read uart3 buf and circle write them into 'UART_TCP_buff' one by one.
  * @param  data from 'UART_TCP_buff2', and 'UART_TCP_buff2' directly get from uart3 buf
  *
  * @retval 
  * 	-<em>1</em> success write one byte
  * 	-<em>0</em> fail
 */
u8 UART_TCP_buffWrite(u8 data)
{
	if(UART_TCP_buffHead!=UART_TCP_buffEnd||UART_TCP_buffOverFlag==0)
	{
		UART_TCP_buff[UART_TCP_buffEnd]=data;
		UART_TCP_buffEnd++;
		if(UART_TCP_buffEnd>=UART_TCP_BUFF_MAXSIZE)
		{
			UART_TCP_buffEnd=0;
			UART_TCP_buffOverFlag=1;
		}
		return 1;
	}
	return 0;
}
/**
  * @brief  continuous write data into 'UART_TCP_buff'
  * @param  data from 'UART_TCP_buff2'
  * @param  length from 'UART_TCP_buff2'
  *
  * @retval 
  * 	-<em>n</em> success write n bytes
  * 	-<em>0</em> fail
 */
u8 UART_TCP_buffWrites(u8 *data,u16 length)
{
	u16 i=0;
	for(i=0;i<length;i++)
	{
		if(UART_TCP_buffWrite(*(data+i))==0)
		{
			return 0;
		}
	}
	return i;
}


/**
* 读取队列中指定1个字节的数据
*/
u8 UART_TCP_buffRead(u8 *data)
{
	if(UART_TCP_buffHead!=UART_TCP_buffEnd||UART_TCP_buffOverFlag==1)
	{
		*data=UART_TCP_buff[UART_TCP_buffHead];
		//F407USART1_SendByte(*data);
		UART_TCP_buffHead++;
		if(UART_TCP_buffHead>=UART_TCP_BUFF_MAXSIZE)
		{
			UART_TCP_buffHead=0;
			UART_TCP_buffOverFlag=0;
		}
		return 1;
	}
	return 0;
}
/**
* 读取队列中指定长度的数据
*/
u16 UART_TCP_buffReads(u8 *data,u16 length)
{
	u16 i=0;
	for(i=0;i<length;i++)
	{
		if(UART_TCP_buffRead(data+i)==0)return 0;
	}
	return length;
}
/**
* 获取队列中数据的长度
*/
u16 UART_TCP_buffLength()
{
	return UART_TCP_BUFF_MAXSIZE*UART_TCP_buffOverFlag+UART_TCP_buffEnd-UART_TCP_buffHead;
}



///**
//* 轮询缓存区是否有数据，并将数据保存到队列中
//*/
//void UART_TCPbuff_Run(void)
//{
//	if(USART3_RX_STA&0X8000)//是否接收到期待的应答结果
//	{
//		UART_TCP_buffWrites(USART3_RX_BUF,USART3_RX_STA&0X7FFF);
//		USART3_RX_STA=0;
//	} 	
//}

//void UART_TCP_buffSends(u8 *data,u16 length)
//{
//	
//	U3_Send_Date(data,length);
//}

u8 UART_TCP_buff2[UART_TCP_BUFF_MAXSIZE];

u8 UART_TCPbuff_Run(u8 (*getbyte)(u8*))
{
	u8 data;
	u8 test_test[20];
	static u8 datalengArry[6];
	static u8 datalengArrycount=0;
	static u16 dataleng=0;
	static u8 step=0;
	static u16 countError=0;
	static u16 dataReceiveIndex=0;
	extern void F407USART1_SendByte(u8 Data);
	while(getbyte(&data)==1)
	{
		F407USART1_SendByte(data);  // to check reveive data
		if(waitStringOK==0&&waitStringBuff[waitStringBuffStep]==data)
		{
			waitStringBuffStep++;
			if(waitStringBuffStep>=waitStringBuffLen)
			{
				waitStringOK=1;
			}
		}
		else 
		{
			// 这部分不太理解
			if(waitStringBuff[0]==data)
			{
				waitStringBuffStep=1;
			}
			else 
			{
				waitStringBuffStep=0;
			}
		}
		switch(step)
		{
			// check recv
			case 0:
				if(data=='r') step=2;
			break;
			case 2:
				if(data=='e') step=4;
				else if(data!='r')step=0;
			break;
			case 4:
				if(data=='c') step=6;
				else if(data=='r')step=2;
				else step=0;
			break;
			case 6:
				if(data=='v') step=8;
				else if(data=='r') step=2;
				else step=0;
			break;
			case 8:
				if(data=='"') step=9;
				else if(data=='r')step=2;
				else step=0;
			break;
			case 9:
				if(data==',') step=10;
				else if(data=='r')step=2;
				else step=0;			
				countError=0;
			break;  // it is a very effective way to extract the information.
			// ignore useless characters
			case 10:
				countError++;
				if(countError>=10)
				{
					step=0;break;
				}
				if(data==',')  // the second comma
				{
					step=23;
					datalengArrycount=0;
				}
				break;
				
			case 23:
				if(data=='\r')step=24;
				else   // get the length of the input
				{
					datalengArry[datalengArrycount]=data;
					datalengArrycount++;
					if(datalengArrycount>=6)step=0;
				}
				break;
			case 24:
				if(data=='\n')
				{
					dataleng=0;
					dataReceiveIndex=0;
					for(;datalengArrycount>0;datalengArrycount--)
					{
						dataleng*=10;
						dataleng+=(datalengArry[dataReceiveIndex]-'0');
						dataReceiveIndex++;
					}
					dataReceiveIndex=0;
					if(dataleng<=UART_TCP_BUFF_MAXSIZE)step=30;
					else step=0;
				}
				else step=0;
				break;				
			
			case 30:
				//F407USART1_SendString("reveive test ok\r\n");  // to check reveive data
				UART_TCP_buff2[dataReceiveIndex]=data;
				dataReceiveIndex++;
				if(dataReceiveIndex>=dataleng)
				{
					#if UART_DEBUG_ENABLE
					printf("success reveive data from server\r\n");  // to check reveive data
					#endif
					UART_TCP_buffWrites(UART_TCP_buff2,dataleng);
					step=0;
				}				
				break;
			default:break;
		}
	}
	return 1;	
}





