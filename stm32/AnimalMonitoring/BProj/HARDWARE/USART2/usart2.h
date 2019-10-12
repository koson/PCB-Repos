#ifndef __USART2_H
#define __USART2_H	 
#include "sys.h"  

#define USART2_MAX_RECV_LEN		400					
#define USART2_MAX_SEND_LEN		400				
#define USART2_RX_EN 			1				

extern u8  USART2_RX_BUF[USART2_MAX_RECV_LEN]; 	
extern u8  USART2_TX_BUF[USART2_MAX_SEND_LEN]; 
extern vu16 USART2_RX_STA;   					

void USART2_init(u32 bound);	
void F407USART2_SendByte(uint8_t Data);
void F407USART2_SendBytes(char *Data, uint16_t leng);
void F407USART2_SendString(char *str);
#endif

