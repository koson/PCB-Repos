#ifndef __RTC_H
#define __RTC_H	 
#include "sys.h" 
 //////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//RTC 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/5
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
//********************************************************************************
//修改说明
//V1.1 20140726
//新增:RTC_Get_Week函数,用于根据年月日信息,得到星期信息.
////////////////////////////////////////////////////////////////////////////////// 

//#define HEART  0
//#define DATA_ACQ  1 
//#define DATA_UDT  2
//#define PICR_DUP  3
//#define AUTO_PHO  4
//#define SCAN_CAM  5

//#define SUSPEND  0
//#define TOWORK   1
//typedef struct Tast_Set
//{
//	u16 no;
//	u16 work_flag;
//	u32 delta_time;
//	u32 remain_time;
//}tast_set; //定义结构体  


//tast_set MY_Task(6);
//u32 BUF[18];  // 72个字节
//memcpy(BUF,MY_Task,72);  // 将结构体中的数据复制到缓存区中
 
/**
 * 从Flash中读出参数，我的想法是直接把结构体写入Flash,度的时候直接从Flash中都出来
 */
//void Task_Init(void)
//{
//	Tast_Set[HEART] -> no = HEART;
//	Tast_Set[HEART] -> work_flag = TOWORK;
//	Tast_Set[HEART] -> delta_time = 100;
//	Tast_Set[HEART] -> delta_time = 50;
//}

//时间结构体
typedef struct 
{
	u8 hour;
	u8 min;
	u8 sec;			
	vu16 msec;
	//公历日月年周
	u16 w_year;
	u8  w_month;
	u8  w_date;
	u8  week;	//1~7,代表周1~周日	
}_calendar_obj;					 
extern _calendar_obj calendar;	//日历结构体

u8 My_RTC_Init(void);						//RTC初始化
ErrorStatus RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm);			//RTC时间设置
ErrorStatus RTC_Set_Date(u8 year,u8 month,u8 date,u8 week); 		//RTC日期设置
void RTC_Set_AlarmA(u8 week,u8 hour,u8 min,u8 sec);		//设置闹钟时间(按星期闹铃,24小时制)
void RTC_Set_WakeUp(u32 wksel,u16 cnt);					//周期性唤醒定时器设置

void Sys_Enter_Standby(void);

void RTC_Get_Time(u8 *hour,u8 *min,u8 *sec,u8 *ampm);
void RTC_Get_Date(u8 *year,u8 *month,u8 *date,u8 *week);
void calendar_get_time(_calendar_obj *calendarx);
void calendar_get_date(_calendar_obj *calendarx);
#endif

















