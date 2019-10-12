#ifndef _UART_TCPBUFF_H_
#define _UART_TCPBUFF_H_
#include "sys.h"

#define UART_TCP_BUFF_MAXSIZE				2048

u8 UART_TCP_buffRead(u8 *data);
u16 UART_TCP_buffReads(u8 *data,u16 length);
u16 UART_TCP_buffLength(void);
u8 UART_TCPbuff_Run(u8 (*getbyte)(u8*));
//void UART_TCP_buffSends(u8 *data,u16 length);

u8 waitString_set(s8 *str,u8 strlen);
void waitStringOK_set(u8 value);
void waitStringOK_Cleare(void);
u8 waitStringOK_get(void);

#endif
