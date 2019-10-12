#ifndef _UART_TCPBUFF_H_
#define _UART_TCPBUFF_H_
#include "sys.h"

#define UART_TCP_BUFF_MAXSIZE 2048
// Local use
uint8_t UART_TCP_buffRead(char *data);
uint16_t UART_TCP_buffReads(char *data, uint16_t length);
uint16_t UART_TCP_buffLength(void);

// Global use
void waitStringOK_set(uint8_t value);
uint8_t waitStringOK_get(void);
uint8_t waitString_set(char *str, uint8_t strlen);
uint8_t WaitString_OKflag(uint16_t timeout);

void waitAppendOK_set(uint8_t value);
uint8_t WaitAppend_OKflag(uint16_t timeout);
uint8_t *waitAppend_set(uint8_t strlen);
uint8_t waitAppendOK_get(void);

uint8_t UART_TCPbuff_Run(uint8_t (*getbyte)(char *));
#endif
