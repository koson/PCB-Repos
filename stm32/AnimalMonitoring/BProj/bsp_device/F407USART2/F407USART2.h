#ifndef _F407USART2_H_
#define _F407USART2_H_



#include "usertypedef.h"



//===============================================
//=================Ó²¼þ²ã°å¼¶º¯Êý
//===============================================
void F407USART2_Init(void);

void F407USART2_SendByte(uint8 Data);
void F407USART2_SendInt_DEC(uint16 value);
void F407USART2_SendString(char *str);
void F407USART2_SendDouble_DEC(double value,uint8 point);


#define F407USART2_RECEIVE_BUFF_ENABLE			1


#if(F407USART2_RECEIVE_BUFF_ENABLE==1)

#define F407USART2_RECEIVE_BUFF_SIZE		2048


uint8 F407USART2_buffWrite(uint8 data);
uint8 F407USART2_buffWrites(uint8 *data,uint16 length);
uint8 F407USART2_buffRead(uint8 *data);
uint16 F407USART2_buffReads(uint8 *data,uint16 length);
uint16 F407USART2_buffLength();


#endif


































#endif
