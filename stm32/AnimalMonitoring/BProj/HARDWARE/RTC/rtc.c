#include "rtc.h"
#include "delay.h"
#include "usart.h" 
#include "power.h"
//////////////////////////////////////////////////////////////////////////////////	 
//修改说明
//V1.1 20140726
//新增:RTC_Get_Week函数,用于根据年月日信息,得到星期信息.
////////////////////////////////////////////////////////////////////////////////// 

_calendar_obj calendar;	//日历结构体


NVIC_InitTypeDef   NVIC_InitStructure;

//RTC时间设置
//hour,min,sec:小时,分钟,秒钟
//ampm:@RTC_AM_PM_Definitions  :RTC_H12_AM/RTC_H12_PM
//返回值:SUCEE(1),成功
//       ERROR(0),进入初始化模式失败 
ErrorStatus RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm)
{
	RTC_TimeTypeDef RTC_TimeTypeInitStructure;
	
	RTC_TimeTypeInitStructure.RTC_Hours=hour;
	RTC_TimeTypeInitStructure.RTC_Minutes=min;
	RTC_TimeTypeInitStructure.RTC_Seconds=sec;
	RTC_TimeTypeInitStructure.RTC_H12=ampm;
	
	return RTC_SetTime(RTC_Format_BIN,&RTC_TimeTypeInitStructure);
	
}
//RTC日期设置
//year,month,date:年(0~99),月(1~12),日(0~31)
//week:星期(1~7,0,非法!)
//返回值:SUCEE(1),成功
//       ERROR(0),进入初始化模式失败 
ErrorStatus RTC_Set_Date(u8 year,u8 month,u8 date,u8 week)
{
	
	RTC_DateTypeDef RTC_DateTypeInitStructure;
	RTC_DateTypeInitStructure.RTC_Date=date;
	RTC_DateTypeInitStructure.RTC_Month=month;
	RTC_DateTypeInitStructure.RTC_WeekDay=week;
	RTC_DateTypeInitStructure.RTC_Year=year;
	return RTC_SetDate(RTC_Format_BIN,&RTC_DateTypeInitStructure);
}

//RTC初始化
//返回值:0,初始化成功;
//       1,LSE开启失败;
//       2,进入初始化模式失败;
u8 My_RTC_Init(void)
{
	RTC_InitTypeDef RTC_InitStructure;
	u16 retry=0X1FFF; 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//使能PWR时钟
	PWR_BackupAccessCmd(ENABLE);	//使能后备寄存器访问 
	printf("rtc init\r\n");
//	if(RTC_ReadBackupRegister(RTC_BKP_DR0)!=0x5050)		//是否第一次配置?
//	{
//		//RCC_SYSCLKSource_HSI
//		RCC_LSICmd(ENABLE);//LSE 开启   
//		//RCC_LSEConfig(RCC_LSE_ON);//LSE 开启   
//		printf("rtc RCC_LSEConfig1\r\n");
//		//while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)	//检查指定的RCC标志位设置与否,等待低速晶振就绪
//		while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
//		{
//			retry++;
//			delay_ms(10);
//		}
//		if(retry==0)return 1;		//LSE 开启失败. 
//		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
//		//RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//设置RTC时钟(RTCCLK),选择LSE作为RTC时钟    
//		RCC_RTCCLKCmd(ENABLE);	//使能RTC时钟 

//		RTC_InitStructure.RTC_AsynchPrediv = 0x7F;//RTC异步分频系数(1~0X7F)
//		RTC_InitStructure.RTC_SynchPrediv  = 0xFF;//RTC同步分频系数(0~7FFF)
//		RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;//RTC设置为,24小时格式
//		RTC_Init(&RTC_InitStructure);
// 
//		RTC_Set_Time(23,36,0,RTC_H12_AM);  //设置时间
//		RTC_Set_Date(19,9,18,3);		//设置日期
//		printf("rtc RCC_LSEConfig2\r\n");
//		RTC_WriteBackupRegister(RTC_BKP_DR0,0x5050);	//标记已经初始化过了
//	} 
	// 外部
	if(RTC_ReadBackupRegister(RTC_BKP_DR0)!=0x5050)		//是否第一次配置?
	{
		//RCC_SYSCLKSource_HSI
		//RCC_LSICmd(ENABLE);//LSE 开启   
		RCC_LSEConfig(RCC_LSE_ON);//LSE 开启   
		printf("rtc RCC_LSEConfig1\r\n");
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)	//检查指定的RCC标志位设置与否,等待低速晶振就绪
		//while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
		{
			retry++;
			delay_ms(10);
		}
		if(retry==0)return 1;		//LSE 开启失败. 
		//RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//设置RTC时钟(RTCCLK),选择LSE作为RTC时钟    
		RCC_RTCCLKCmd(ENABLE);	//使能RTC时钟 

		RTC_InitStructure.RTC_AsynchPrediv = 0x7F;//RTC异步分频系数(1~0X7F)
		RTC_InitStructure.RTC_SynchPrediv  = 0xFF;//RTC同步分频系数(0~7FFF)
		RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;//RTC设置为,24小时格式
		RTC_Init(&RTC_InitStructure);
 
		RTC_Set_Time(18,17,0,RTC_H12_AM);  //设置时间
		RTC_Set_Date(19,9,23,1);		//设置日期
		printf("rtc RCC_LSEConfig2\r\n");
		RTC_WriteBackupRegister(RTC_BKP_DR0,0x5050);	//标记已经初始化过了
	} 
 
 
	return 0;
}

//设置闹钟时间(按星期闹铃,24小时制)
//week:星期几(1~7) @ref  RTC_Alarm_Definitions
//hour,min,sec:小时,分钟,秒钟
void RTC_Set_AlarmA(u8 week,u8 hour,u8 min,u8 sec)
{ 
	EXTI_InitTypeDef   EXTI_InitStructure;
	RTC_AlarmTypeDef RTC_AlarmTypeInitStructure;
	RTC_TimeTypeDef RTC_TimeTypeInitStructure;
	
	RTC_AlarmCmd(RTC_Alarm_A,DISABLE);  // 关闭闹钟A 
	
	RTC_TimeTypeInitStructure.RTC_Hours=hour;  // 小时
	RTC_TimeTypeInitStructure.RTC_Minutes=min;  // 分钟
	RTC_TimeTypeInitStructure.RTC_Seconds=sec;  // 秒
	RTC_TimeTypeInitStructure.RTC_H12=RTC_H12_AM;
  
	RTC_AlarmTypeInitStructure.RTC_AlarmDateWeekDay=week;  // 星期
	RTC_AlarmTypeInitStructure.RTC_AlarmDateWeekDaySel=RTC_AlarmDateWeekDaySel_WeekDay;  // 按星期闹
	RTC_AlarmTypeInitStructure.RTC_AlarmMask=RTC_AlarmMask_None;  // 精确匹配星期，时分秒
	RTC_AlarmTypeInitStructure.RTC_AlarmTime=RTC_TimeTypeInitStructure;
	RTC_SetAlarm(RTC_Format_BIN,RTC_Alarm_A,&RTC_AlarmTypeInitStructure);
 
	
	RTC_ClearITPendingBit(RTC_IT_ALRA);  // 清除RTC闹钟A的标志
	EXTI_ClearITPendingBit(EXTI_Line17);  // 清除LINE17上的中断标志位 
	
	RTC_ITConfig(RTC_IT_ALRA,ENABLE);  // 开启闹钟A中断
	RTC_AlarmCmd(RTC_Alarm_A,ENABLE);  // 开启闹钟A 
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line17;  // LINE17
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;  // 中断事件
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;   // 上升沿触发 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;  // 使能LINE17
	EXTI_Init(&EXTI_InitStructure);  // 配置

	NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;  // 抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;  // 子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  // 使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);  // 配置
}

//周期性唤醒定时器设置  
/*wksel:  @ref RTC_Wakeup_Timer_Definitions
#define RTC_WakeUpClock_RTCCLK_Div16        ((uint32_t)0x00000000)
#define RTC_WakeUpClock_RTCCLK_Div8         ((uint32_t)0x00000001)
#define RTC_WakeUpClock_RTCCLK_Div4         ((uint32_t)0x00000002)
#define RTC_WakeUpClock_RTCCLK_Div2         ((uint32_t)0x00000003)
#define RTC_WakeUpClock_CK_SPRE_16bits      ((uint32_t)0x00000004)
#define RTC_WakeUpClock_CK_SPRE_17bits      ((uint32_t)0x00000006)
*/
//cnt:自动重装载值.减到0,产生中断.
void RTC_Set_WakeUp(u32 wksel,u16 cnt)
{ 
	EXTI_InitTypeDef   EXTI_InitStructure;
	
	RTC_WakeUpCmd(DISABLE);  // 关闭WAKE UP
	
	RTC_WakeUpClockConfig(wksel);  // 唤醒时钟选择
	
	RTC_SetWakeUpCounter(cnt);  // 设置WAKE UP自动重装载寄存器
	
	
	RTC_ClearITPendingBit(RTC_IT_WUT);  // 清除RTC WAKE UP的标志
    EXTI_ClearITPendingBit(EXTI_Line22);  // 清除LINE22上的中断标志位 
	 
	RTC_ITConfig(RTC_IT_WUT,ENABLE);  // 开启WAKE UP定时器中断
	RTC_WakeUpCmd( ENABLE);  // 开启WAKE UP定时器　
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line22;  // LINE22
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;  // 中断事件
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  // 上升沿触发 
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;  // 使能LINE22
    EXTI_Init(&EXTI_InitStructure);  // 配置
 
 
	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;  // 抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;  // 子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  // 使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);  // 配置
}

//char wakeup_flag;
// RTC闹钟中断服务函数
void RTC_Alarm_IRQHandler(void)
{    
	if(RTC_GetFlagStatus(RTC_FLAG_ALRAF)==SET)//ALARM A中断
	{
		RTC_ClearFlag(RTC_FLAG_ALRAF);//清除中断标志

		calendar_get_time(&calendar);
		calendar_get_date(&calendar);
		RTC_Set_AlarmA(4,calendar.hour,calendar.min,calendar.sec+20);
		//printf("DATA:%d-%d-%d	Time:%d:%d:%d\r\n",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);
  
	}   
	EXTI_ClearITPendingBit(EXTI_Line17);	//清除中断线17的中断标志 											 
}

// RTC WAKE UP中断服务函数
void RTC_WKUP_IRQHandler(void)
{    
	if(RTC_GetFlagStatus(RTC_FLAG_WUTF)==SET)  // WK_UP中断
	{ 
		RTC_ClearFlag(RTC_FLAG_WUTF);  // 清除中断标志
//		uart_init(115200);
//		printf("RTC wake up\r\n");
//		LED0 = 0;LED1 = 0;
//		delay_ms(1000);
		//LED1=!LED1; 
		//wakeup_flag = 2;
	}   
	EXTI_ClearITPendingBit(EXTI_Line22); //  清除中断线22的中断标志 								
}
 
//系统进入待机模式
void Sys_Enter_Standby(void)
{		
	
	RCC_AHB1PeriphResetCmd(0X04FF,ENABLE);//复位所有IO口
	/*关闭所有外设*/
	POWER_4G =0;
	POWER_USB=0;
	POWER_CAM=0;
	POWER_OUT3V3=0;
	POWER_OUT5=0;
	//
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//使能PWR时钟		 
	 
	PWR_BackupAccessCmd(ENABLE);//后备区域访问使能

	//这里我们就直接关闭相关RTC中断
	RTC_ITConfig(RTC_IT_TS|RTC_IT_WUT|RTC_IT_ALRB|RTC_IT_ALRA,DISABLE);//关闭RTC相关中断，可能在RTC实验打开了。/*1*/
	RTC_ClearITPendingBit(RTC_IT_TS|RTC_IT_WUT|RTC_IT_ALRB|RTC_IT_ALRA);//清楚RTC相关中断标志位。/*2*/
	
	PWR_ClearFlag(PWR_FLAG_WU);//清除Wake-up 标志/*3*/
	RTC_ITConfig(RTC_IT_WUT,ENABLE);//开启自动唤醒单元/*4*/
	PWR_WakeUpPinCmd(ENABLE);//进用于设置WKUP用于唤醒
	
	calendar_get_time(&calendar);
	calendar_get_date(&calendar);
	RTC_Set_AlarmA(4,calendar.hour,calendar.min,calendar.sec+20);
		//printf("DATA:%d-%d-%d	Time:%d:%d:%d\r\n",calendar.w_year,calend
	
	PWR_EnterSTANDBYMode();	//进入待机模式/*5*/
//	delay_ms(1000);
//	PWR_EnterSTANDBYMode();
//	delay_ms(1000);
//	PWR_EnterSTANDBYMode();
}

//获取RTC时间
//*hour,*min,*sec:小时,分钟,秒钟 
//*ampm:@RTC_AM_PM_Definitions  :RTC_H12_AM/RTC_H12_PM.
void RTC_Get_Time(u8 *hour,u8 *min,u8 *sec,u8 *ampm)
{
	RTC_TimeTypeDef RTC_TimeTypeInitStructure;
	
	RTC_GetTime(RTC_Format_BIN,&RTC_TimeTypeInitStructure);
	
	*hour=RTC_TimeTypeInitStructure.RTC_Hours;
	*min=RTC_TimeTypeInitStructure.RTC_Minutes;
	*sec=RTC_TimeTypeInitStructure.RTC_Seconds;
	*ampm=RTC_TimeTypeInitStructure.RTC_H12;
}
//获取RTC日期
//*year,*mon,*date:年,月,日
//*week:星期
void RTC_Get_Date(u8 *year,u8 *month,u8 *date,u8 *week)
{
	RTC_DateTypeDef  RTC_DateStruct;
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);

	*year=RTC_DateStruct.RTC_Year;
	*month=RTC_DateStruct.RTC_Month;
	*date=RTC_DateStruct.RTC_Date;
	*week=RTC_DateStruct.RTC_WeekDay; 
}

 //得到时间
//calendarx:日历结构体
void calendar_get_time(_calendar_obj *calendarx)
{
	u8 ampm;
	RTC_Get_Time(&calendarx->hour,&calendarx->min,&calendarx->sec,&ampm);//得到时间	
}
//得到日期
//calendarx:日历结构体
void calendar_get_date(_calendar_obj *calendarx)
{
	u8 year;
	RTC_Get_Date(&year,&calendarx->w_month,&calendarx->w_date,&calendarx->week);
	calendar.w_year=year+2000;//从2000年开始算起
}



