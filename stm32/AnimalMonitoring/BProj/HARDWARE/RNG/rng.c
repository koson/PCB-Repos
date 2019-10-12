#include "rng.h"
#include "delay.h"
//初始化RNG
/**
 * @description: initial the rng
 * @param void
 * @return: 返回随机数状态，0->成功，1->异常
 */
u8 rng_Init(void)
{
	u16 retry = 0;

	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE); //开启RNG时钟,来自PLL48CLK

	RNG_Cmd(ENABLE); //使能RNG

	while (RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET && retry < 10000) //等待随机数就绪
	{
		retry++;
		delay_us(100);
	}
	if (retry >= 10000)
		return 1; //随机数产生器工作不正常
	return 0;
}
//得到随机数
//返回值:获取到的随机数
/**
 * @description: get random num
 * @param void
 * @return: random num -u32
 */
u32 rngGetRandomNum(void)
{
	while (RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET)
		;
	return RNG_GetRandomNumber();
}
/**
 * @description: 获取[min,max]范围的随机数
 * @param none
 * @return: 返回范围内的随机值
 */
int rngGetRandomRange(int min, int max)
{
	return rngGetRandomNum() % (max - min + 1) + min;
}
