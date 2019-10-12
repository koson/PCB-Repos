#ifndef _MYTYPEDEF_F4_H_
#define _MYTYPEDEF_F4_H_


#include "stm32f4xx_gpio.h"


#ifndef uint64
#define uint64 unsigned long long
#endif

#ifndef int64
#define int64 signed long long
#endif

#ifndef uint32
#define uint32 unsigned int 
#endif

#ifndef int32
#define int32 signed int 
#endif


#ifndef uint16
#define uint16 unsigned short
#endif

#ifndef uint8
#define uint8 unsigned char
#endif

#ifndef int16
#define int16 short
#endif

#ifndef int8
#define int8 char
#endif


volatile typedef struct 
{
	uint32_t RCCx;
	GPIO_TypeDef *GPIOx;
	uint32_t GPIO_Pin;
	GPIOMode_TypeDef GPIO_Mode;
	GPIOSpeed_TypeDef GPIO_Speed;
	GPIOOType_TypeDef GPIO_OType;
	GPIOPuPd_TypeDef GPIO_PuPd;
}F4XX_GPIO;



void F4XXGPIO_Init(F4XX_GPIO *GPIOx);
void F4XXGPIO_HIGH(F4XX_GPIO *GPIOx);
void F4XXGPIO_LOW(F4XX_GPIO *GPIOx);
void F4XXGPIO_NOT(F4XX_GPIO *GPIOx);
uint8 F205GPIO_Read(F4XX_GPIO *GPIOx);






#endif
