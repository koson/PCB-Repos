#ifndef _HELLO_LED_H_
#define _HELLO_LED_H_
#include "sys.h" 

#define ledGREEN  PFout(9)	
#define ledYELLOW PFout(10)	
#define ledBLUE   PFout(10)	 

void HELLO_LEDInit(void);
void HELLO_LED1_ON(void);
void HELLO_LED1_OFF(void);
void HELLO_LED1_NOT(void);
void HELLO_LED2_ON(void);
void HELLO_LED2_OFF(void);
void HELLO_LED2_NOT(void);
void BLUE_LED_ON(void);
void BLUE_LED_OFF(void);
void BLUE_LED_NOT(void);

#endif
