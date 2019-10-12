#ifndef _USART3_H_
#define _USART3_H_
#include "sys.h"
//===============================================
//=================硬件初始化
//===============================================
// Global use
void F407USART3_Init(uint32_t btl);
void F407USART3_SendByte(uint8_t Data);
void F407USART3_SendBytes(char *Data,uint16_t leng);
void F407USART3_SendString(char *str);

// Just for UART_TCPbuff
#define F407USART3_RECEIVE_BUFF_ENABLE		1
#if (F407USART3_RECEIVE_BUFF_ENABLE==1)
#define F407USART3_RECEIVE_BUFF_SIZE		2048
uint8_t F407USART3_buffWrite(uint8_t data);
uint8_t F407USART3_buffWrites(uint8_t *data,uint16_t length);

uint8_t F407USART3_buffRead(uint8_t *data);
uint16_t F407USART3_buffReads(uint8_t *data,uint16_t length);
uint16_t F407USART3_buffLength(void);
#endif // F407USART3_RECEIVE_BUFF_ENABLE

#endif
