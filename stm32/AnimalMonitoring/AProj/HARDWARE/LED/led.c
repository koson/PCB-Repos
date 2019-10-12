#include "led.h" 
/**
* 适用于Vision4.0
* LED1 PE2 绿灯
* LED2 PE3 黄灯
*/

//初始化PF9和PF10为输出口.并使能这两个口的时钟		    
//LED IO初始化
void LED_Init(void)
{    	 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//使能GPIOE时钟

	// GPIOE2,E3初始化设置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化

	GPIO_SetBits(GPIOE,GPIO_Pin_2 | GPIO_Pin_3);//GPIOE2,E3设置高，灯灭
}

void LED_YELLOW_ON(void)
{
	LED_Y=0;
}
void LED_YELLOW_OFF(void)
{
	LED_Y=1;
}

void LED_YELLOW_NOT(void)
{
	LED_Y=!LED_Y;
}

void LED_GREEN_ON(void)
{
	LED_G=0;
}
void LED_GREEN_OFF(void)
{
	LED_G=1;
}

void LED_GREEN_NOT(void)
{
	LED_G=!LED_G;
}





