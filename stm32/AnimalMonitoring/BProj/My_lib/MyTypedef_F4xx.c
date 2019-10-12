#include "MyTypedef_F4xx.h"


void F4XXGPIO_Init(F4XX_GPIO *GPIOx)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	//
	RCC_AHB1PeriphClockCmd(GPIOx->RCCx, ENABLE);
	//
	GPIO_InitStructure.GPIO_Pin = GPIOx->GPIO_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIOx->GPIO_Mode;
	GPIO_InitStructure.GPIO_OType = GPIOx->GPIO_OType;
	GPIO_InitStructure.GPIO_Speed = GPIOx->GPIO_Speed;
	GPIO_InitStructure.GPIO_PuPd =  GPIOx->GPIO_PuPd;
	GPIO_Init(GPIOx->GPIOx, &GPIO_InitStructure);
}

void F4XXGPIO_HIGH(F4XX_GPIO *GPIOx)
{
	GPIO_WriteBit(GPIOx->GPIOx,GPIOx->GPIO_Pin,1);
}

void F4XXGPIO_LOW(F4XX_GPIO *GPIOx)
{
	GPIO_WriteBit(GPIOx->GPIOx,GPIOx->GPIO_Pin,0);
}

void F4XXGPIO_NOT(F4XX_GPIO *GPIOx)
{
	GPIO_ToggleBits(GPIOx->GPIOx,GPIOx->GPIO_Pin);
}

uint8 F205GPIO_Read(F4XX_GPIO *GPIOx)
{
	return GPIO_ReadInputDataBit(GPIOx->GPIOx,GPIOx->GPIO_Pin);
}

