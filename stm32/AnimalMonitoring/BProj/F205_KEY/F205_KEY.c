/*
功能：基于单个IO的按键程序
时间：2017-03-05
作者：Lhuan
更新记录：
1）2018-03-06：长按事件函数1个返回值变为两个返回值，value：按键ID，value2：长按时间

*/

#include "F205_KEY.h"


#ifdef KEY1
F4XX_GPIO KEY1_IO=KEY1_IOCOF;
#endif
#ifdef KEY2
F4XX_GPIO KEY2_IO=KEY2_IOCOF;
#endif
#ifdef KEY3
F4XX_GPIO KEY3_IO=KEY3_IOCOF;
#endif
#ifdef KEY4
F4XX_GPIO KEY4_IO=KEY4_IOCOF;
#endif


KeyEventCallback pup=0,plongdown=0,DownTime=0,DownCount=0;     								///*定义事件响应指针*/

u8 F205_KEYScan(void);



void F205_KEYInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOE, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;  // KEY_WKUP
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

u8 F205_KEYScan(void)
{
	u8 keyvalue=0;
	#ifdef KEY1
	if(GPIO_ReadInputDataBit(KEY1_IO.GPIOx,KEY1_IO.GPIO_Pin)==0)
	{
		keyvalue|=KEY1;
	}
	#endif
	#ifdef KEY2
	if(GPIO_ReadInputDataBit(KEY2_IO.GPIOx,KEY2_IO.GPIO_Pin)==0)
	{
		keyvalue|=KEY2;
	}
	#endif
	#ifdef KEY3
	if(GPIO_ReadInputDataBit(KEY3_IO.GPIOx,KEY3_IO.GPIO_Pin)==0)
	{
		keyvalue|=KEY3;
	}
	#endif	
	#ifdef KEY4
	if(GPIO_ReadInputDataBit(KEY4_IO.GPIOx,KEY4_IO.GPIO_Pin)==0)
	{
		keyvalue|=KEY4;
	}
	#endif	
	return keyvalue;
	
}

void F205_KEYRun(uint16 divtime)
{
	static u8 firstrun=0;
	static char historyvalue=0;
	//定义长按时间变量，记录长按时间
	#ifdef KEY1
	static uint16 K1_Downtime=0,K1_Uptime=0,K1_count=0;
	#endif
	#ifdef KEY2
	static uint16 K2_Downtime=0,K2_Uptime=0,K2_count=0;
	#endif	
	#ifdef KEY3
	static uint16 K3_Downtime=0,K3_Uptime=0;
	#endif	
	#ifdef KEY4
	static uint16 K4_Downtime=0,K4_Uptime=0;
	#endif
	
	char value=0,upvalue=0,downvalue=0;
	// 获得键值
	value=F205_KEYScan();
	// 首次运行函数，因为没有参考，所以首次扫描值不做处理
	if(firstrun==0)
	{
		firstrun=1;
		historyvalue=value;
		return ;
	}	
	//============================================
	if(value^historyvalue)//按键值有变化
	{
		upvalue=(value^historyvalue)&historyvalue;
		downvalue=(value^historyvalue)&value;
		//使用完毕后将本次扫描数据再次记录为历史值
		historyvalue=value;
		//
		#ifdef KEY1
		if(upvalue&KEY1)  //如果松开，代表按压结束
		{
			// 如果定了松开回调函数 则执行
			if(pup)pup(KEY1,K1_Downtime);
			K1_Uptime=0;
		}
		if(downvalue&KEY1) // 如果按压，代表松开结束
		{
			if(DownTime)DownTime(downvalue,K1_Uptime);
			if(K1_count==0)
			{
				K1_count++;
			}
			else if(K1_Uptime<=SHORT_DOWN_TIME_MS)
			{
					K1_count++;
			}
			else K1_count=1;
			K1_Downtime=0;
		}
		#endif
		#ifdef KEY2
		if(upvalue&KEY2)
		{
			if(pup)pup(KEY2,K2_Downtime);
			K2_Uptime=0;
		}
		if(downvalue&KEY2)
		{
			if(DownTime)DownTime(downvalue,K2_Uptime);
			if(K2_count==0)
			{
				K2_count++;
			}
			else if(K2_Uptime<=SHORT_DOWN_TIME_MS)
			{
					K2_count++;
			}
			else K2_count=1;
			K2_Downtime=0;
		}
		#endif		
  }
	//================================================
	#ifdef KEY1
	if(value&KEY1)
	{
		// 判断是否长按
		if(K1_Downtime<60000)K1_Downtime+=divtime;
		if(plongdown)plongdown(KEY1,K1_Downtime);
	}
	else 
	{
		// 双击或多击
		if(K1_Uptime<30000)K1_Uptime+=divtime;
		if(K1_count!=0&&K1_Uptime>=SHORT_DOWN_TIME_MS)
		{
			if(DownCount&&K1_Downtime<=CLICK_TIME_OUT)DownCount(KEY1,K1_count);
			K1_count=0;
		}
	}
	#endif
	#ifdef KEY2
	if(value&KEY2)
	{
		if(K2_Downtime<60000)K2_Downtime+=divtime;
		if(plongdown)plongdown(KEY2,K2_Downtime);
	}
	else 
	{
		if(K2_Uptime<30000)K2_Uptime+=divtime;
		if(K2_count!=0&&K2_Uptime>=SHORT_DOWN_TIME_MS)
		{
			if(DownCount&&K2_Downtime<=CLICK_TIME_OUT)DownCount(KEY2,K2_count);
			K2_count=0;
		}
	}
	#endif	
}


