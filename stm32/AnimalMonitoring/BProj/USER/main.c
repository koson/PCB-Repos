#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "misc.h"
#include "os_include.h"
#include "adc.h"
#include "malloc.h"
#include "usb_app.h"

#include "usart3.h"
#include "UART_TCPbuff.h"

#include "mqttBase.h"
#include "mqttApp.h"
#include "MQTTPacket.h"
#include "mqttTransport.h"

#include "MyFunction_C.h"

#include "usart2.h"

#include "key.h"
#include "led.h"
#include "ec25.h"

#include "power.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"

#include "malloc.h"  
#include "sdio_sdcard.h"
#include "ff.h"  
#include "exfuns.h"  
#include "usbh_usr.h" 

#include "cjsontest.h"
#include "usmart.h"

#include "draft.h"

#include "string.h"
#include "fattester.h"	
//sensors
#include "myiic_sht20.h"
#include "myiic_max44009.h"
#include "SHT2x.h"
#include "max44009.h" 
#include "ms5611.h"
#include "sensor.h"  
#include "usart2.h"
#include "rtc.h"
#include "iwdg.h"
//#include "wwdg.h"

#include "usbh_usr.h" 
#include "queue.h" 
#include "rng.h"

#define MQTT_RECEIVEBUFF_MAXLENGTH		2048
uint8_t MQTT_Receivebuf[MQTT_RECEIVEBUFF_MAXLENGTH];

#define MQTT_SEND_BUFF_MAXLENGTH		12000
extern uint8_t MQTT_SEND_buf[MQTT_SEND_BUFF_MAXLENGTH];

#define MQTT_DATA_BUFF_MAXLENGTH		12000
extern uint8_t MQTT_DATA_buf[MQTT_DATA_BUFF_MAXLENGTH];

void NVIC_Configuration(void);
void RCC_Config(void);

// <s>USB_CON
//USBH_HOST  USB_Host;
//USB_OTG_CORE_HANDLE  USB_OTG_Core;
extern USBH_HOST USB_Host;
extern USB_OTG_CORE_HANDLE USB_OTG_Core;


//系统任务相关参数配置和声明
#define SYSTASK_PRIO									20
//任务堆栈大小32bit
#define CPU_STK_SYSTASK_SIZE							1000
//时间片长度
#define SYSTASK_TICK_LEN								0
static  OS_TCB											SysTaskTCB;
static	CPU_STK											SysTaskStk[CPU_STK_SYSTASK_SIZE];
static void  SysTask(void* p_arg);

/* 系统主任务 */
#define MAIN_TASK_PRIO									21
//任务堆栈大小32bit
#define CPU_STK_MAIN_SIZE								1000
//时间片长度
#define MAIN_TICK_LEN									0
static  OS_TCB											MainTaskTCB;
static	CPU_STK											MainTaskStk[CPU_STK_SYSTASK_SIZE];
static void  MainTask(void* p_arg);



/* 接收MQTT指令 */
#define MQTTRECEIVE_PRIO								22
//任务堆栈大小32bit
#define CPU_STK_MQTTRECEIVE_SIZE						1000
//时间片长度
#define MQTTRECEIVE_TICK_LEN							0
static  OS_TCB											MQTTRECEIVEtcb;
static	CPU_STK											MQTTRECEIVEstk[CPU_STK_MQTTRECEIVE_SIZE];
static void  MQTTRECEIVEtask(void* p_arg);

/* 发送MQTT指令 */
#define SIM7100_PRIO									23
//任务堆栈大小32bit
#define CPU_STK_SIM7100_SIZE							1000
//时间片长度
#define SIM7100_TICK_LEN								0
static  OS_TCB											SIM7100tcb;
static	CPU_STK											SIM7100stk[CPU_STK_SIM7100_SIZE];
static void  SIM7100task(void* p_arg);

/* 键盘扫描 */
#define KeyScan_TASK_PRIO								24
#define CPU_STK_KeyScan_TASK_SIZE						300
#define KeyScan_TICK_LEN								0
static  OS_TCB											KeyScan_TCB;
static	CPU_STK											KeyScan_Stk[CPU_STK_KeyScan_TASK_SIZE];

/* Watch task */
#define HELLO_LED_PRIO									25
#define CPU_STK_HELLO_LED_SIZE							2000
#define HELLO_LED_TICK_LEN								0
static  OS_TCB											HELLO_LEDtcb;
static	CPU_STK											HELLO_LEDstk[CPU_STK_HELLO_LED_SIZE];
static void  HELLO_LEDtask(void* p_arg);

#define  SystemDatasBroadcast_PRIO                      30 // 统计任务优先级最低，我这里是12，已经低于其他任务的优先级了
#define  SystemDatasBroadcast_STK_SIZE                  200 // 任务的堆栈大小，做统计一般够了，统计结果出来后不够再加..
#define  SystemDatasBroadcast_LED_TICK_LEN              0
static   OS_TCB                                         SystemDatasBroadcast_TCB;		                // 定义统计任务的TCB
static   CPU_STK                                        SystemDatasBroadcast_STK [SystemDatasBroadcast_STK_SIZE];// 开辟数组作为任务栈给任务使用
static void  SystemDatasBroadcast (void *p_arg);

void SoftReset(void)
{
    __set_FAULTMASK(1); // 关闭所有中断
    NVIC_SystemReset(); // 复位
}

static vu16 watchdog_f;
static vu16 function_f;

extern int32_t BaroPressure;
extern float BaroAlt;

extern SqQueue Q_stage;   // 建立缓存区队列
extern SqQueue Q_resent;  // 建立重发区队列
int main()
{
	//u8 sta;
    OS_ERR err;
    CPU_SR_ALLOC();
	
    SystemInit();
    RCC_Config();
    NVIC_Configuration();
	InitQueue(&Q_stage);  // 初始化队列 
	InitQueue(&Q_resent);  // 初始化队列 
	watchdog_f=0;
	function_f=0;
    delay_init(168); //时钟初始化
    uart_init(921600);
	USART2_init(9600);
	
//	while(1)
//	{
//		printf("test\r\n");
//		delay_ms(1000);
//		if(USART2_RX_STA&0x8000)
//		{
//			printf("test+++++\r\n");
//			USART2_RX_STA=0;
//		}
//	}
	//IWDG_Init(4,500);1s  64/32*500
	IWDG_Init(6,2500); //256/32=8   2500/500=5
    F407USART1_SendString("usart is ok:");
    LED_Init();   // LED初始化
    Power_Ctrl_Init(); // 电源初始化
    Cam_Crtl_Init();   // 相机控制引脚初始化
	adc_Init();
	usbapp_init();
	rng_Init();
//    SHT2x_Init();  			//SHT20初始化
//    max44009_initialize();  //MAX44009初始化
//	MS5611_Init();  //MS5611初始化
//	MS5611_PROM_READ();
//		
//	SHT2x_Init();  //SHT20初始化
//	MS5611_Init();  //MS5611初始化
//	MS5611_PROM_READ();
//	max44009_initialize();  //MAX44009初始化
//	delay_ms(500);
//	MS5611_Temperature_Get();
//	MS5611_Pressure_Get();
//	BaroPressure=MS5611_Compensation_Calcu();
//	BaroAlt=getEstimatedAltitude(BaroPressure);

    My_RTC_Init();  //初始化RTC
	calendar_get_time(&calendar);
	calendar_get_date(&calendar);
	printf("DATA:%d-%d-%d	Time:%d:%d:%d\r\n",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);
    //RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits,600*2);		//配置WAKE UP中断,1秒钟中断一次
    
    //初始化USB主机
    USBH_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USB_Host,&USBH_MSC_cb,&USR_Callbacks);
	delay_ms(1000);
    if (!SD_Init())
        printf("SD_Init ok\r\n"); //判断SD卡是否存在
    else
        printf("SD_Init Error\r\n");

    mymem_init(SRAMIN);      //初始化内部内存池
    exfuns_init();           //为fatfs相关变量申请内存
    f_mount(fs[0], "0:", 1); //挂载SD卡
    f_mount(fs[1], "1:", 1); //挂载U盘
    
//	sta = SIM7100_Init();
//    if(sta)
//		F407USART1_SendString("SIM7100_Init OK\r\n");
//	else
//		F407USART1_SendString("SIM7100_Init ERROR\r\n");
	IWDG_Feed();//喂狗
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//进入系统
    OSInit(&err);        //用于初始化 uC/OS-III
    OS_CRITICAL_ENTER(); //进入临界区
	printf("OS_CRITICAL_ENTER\r\n");
    //
    OSTaskCreate((OS_TCB *)&SysTaskTCB,                               //该任务堆栈的开始地址
                 (CPU_CHAR *)"SysTask",                               //任务分配名字
                 (OS_TASK_PTR)SysTask,                                //指向任务代码的指针
                 (void *)0,                                           //指针，第一次执行任务时传递给，任务实体的指针参数*p_arg
                 (OS_PRIO)SYSTASK_PRIO,                               //优先级设置	 参数值越小优先级越高
                 (CPU_STK *)&SysTaskStk[0],                           //任务堆栈的基地址。基地址通常是分配给该任务的堆栈的最低内存位置
                 (CPU_STK_SIZE)CPU_STK_SYSTASK_SIZE / 10,             //第七个参数是地址“水印” ，当堆栈生长到指定位置时就不再允许其生长
                 (CPU_STK_SIZE)CPU_STK_SYSTASK_SIZE,                  //任务的堆栈大小
                 (OS_MSG_QTY)0,                                       //
                 (OS_TICK)SYSTASK_TICK_LEN,                           //设置任务拥有多少个时间片，当采用时间片轮询调度任务时有效
                 (void *)0,                                           //
                 (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //
                 (OS_ERR *)&err);                                     //返回当前函数执行的结果OS_ERR_NONE表示成功
    OS_CRITICAL_EXIT();                                               //退出临界区
    OSStart(&err);                                                    //开始多任务处理
}
   
static void SysTask(void *p_arg) // == appCreat
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;
	
	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//统计任务                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
	 //使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	
	OS_CRITICAL_ENTER();	//进入临界区
    // stackMonitoring
    OSTaskCreate((OS_TCB *)&SystemDatasBroadcast_TCB,
                 (CPU_CHAR *)"SystemDatasBroadcast",
                 (OS_TASK_PTR)SystemDatasBroadcast,
                 (void *)0,
                 (OS_PRIO)SystemDatasBroadcast_PRIO,
                 (CPU_STK *)&SystemDatasBroadcast_STK[0],
                 (CPU_STK_SIZE)SystemDatasBroadcast_STK_SIZE / 10, /*栈溢出临界值我设置在栈大小的90%处*/
                 (CPU_STK_SIZE)SystemDatasBroadcast_STK_SIZE,
                 (OS_MSG_QTY)0,
                 (OS_TICK)0,
                 (void *)0,
                 (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR *)&err);
    //


    //
    OSTaskCreate((OS_TCB *)&MQTTRECEIVEtcb,
                 (CPU_CHAR *)"MQTTRECEIVE",
                 (OS_TASK_PTR)MQTTRECEIVEtask,
                 (void *)0,
                 (OS_PRIO)MQTTRECEIVE_PRIO,
                 (CPU_STK *)&MQTTRECEIVEstk[0],
                 (CPU_STK_SIZE)CPU_STK_MQTTRECEIVE_SIZE / 10,
                 (CPU_STK_SIZE)CPU_STK_MQTTRECEIVE_SIZE,
                 (OS_MSG_QTY)0,
                 (OS_TICK)MQTTRECEIVE_TICK_LEN,
                 (void *)0,
                 (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR *)&err);
    //
    OSTaskCreate((OS_TCB *)&SIM7100tcb,
                 (CPU_CHAR *)"SIM7100",
                 (OS_TASK_PTR)SIM7100task,
                 (void *)0,
                 (OS_PRIO)SIM7100_PRIO,
                 (CPU_STK *)&SIM7100stk[0],
                 (CPU_STK_SIZE)CPU_STK_SIM7100_SIZE / 10,
                 (CPU_STK_SIZE)CPU_STK_SIM7100_SIZE,
                 (OS_MSG_QTY)0,
                 (OS_TICK)SIM7100_TICK_LEN,
                 (void *)0,
                 (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR *)&err);

    // KeyScan
    OSTaskCreate((OS_TCB *)&KeyScan_TCB,
                 (CPU_CHAR *)"KeyScan",
                 (OS_TASK_PTR)KeyScan_Task,
                 (void *)0,
                 (OS_PRIO)KeyScan_TASK_PRIO,
                 (CPU_STK *)&KeyScan_Stk[0],
                 (CPU_STK_SIZE)CPU_STK_KeyScan_TASK_SIZE / 10,
                 (CPU_STK_SIZE)CPU_STK_KeyScan_TASK_SIZE,
                 (OS_MSG_QTY)0,
                 (OS_TICK)KeyScan_TICK_LEN,
                 (void *)0,
                 (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR *)&err);
    // watchTask
    OSTaskCreate((OS_TCB *)&HELLO_LEDtcb,
                 (CPU_CHAR *)"HELLO_LED",
                 (OS_TASK_PTR)HELLO_LEDtask,
                 (void *)0,
                 (OS_PRIO)HELLO_LED_PRIO,
                 (CPU_STK *)&HELLO_LEDstk[0],
                 (CPU_STK_SIZE)CPU_STK_HELLO_LED_SIZE / 10,
                 (CPU_STK_SIZE)CPU_STK_HELLO_LED_SIZE,
                 (OS_MSG_QTY)0,
                 (OS_TICK)HELLO_LED_TICK_LEN,
                 (void *)0,
                 (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR *)&err);
				 
    OSTaskCreate((OS_TCB *)&MainTaskTCB,                              // 该任务堆栈的开始地址
                 (CPU_CHAR *)"MainTask",                              // 任务分配名字
                 (OS_TASK_PTR)MainTask,                               // 指向任务代码的指针
                 (void *)0,                                           // 指针，第一次执行任务时传递给，任务实体的指针参数*p_arg
                 (OS_PRIO)MAIN_TASK_PRIO,                             // 优先级设置	 参数值越小优先级越高
                 (CPU_STK *)&MainTaskStk[0],                          // 任务堆栈的基地址。基地址通常是分配给该任务的堆栈的最低内存位置
                 (CPU_STK_SIZE)CPU_STK_MAIN_SIZE / 10,                // 第七个参数是地址“水印” ，当堆栈生长到指定位置时就不再允许其生长
                 (CPU_STK_SIZE)CPU_STK_MAIN_SIZE,                     // 任务的堆栈大小
                 (OS_MSG_QTY)0,                                       //
                 (OS_TICK)MAIN_TICK_LEN,                              // 设置任务拥有多少个时间片，当采用时间片轮询调度任务时有效
                 (void *)0,                                           //
                 (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //
                 (OS_ERR *)&err);                                     // 返回当前函数执行的结果OS_ERR_NONE表示成功		 
		
	OS_CRITICAL_EXIT();	//退出临界区
	OSTaskDel((OS_TCB*)0,&err);	//删除start_task任务自身
}

// <s>HELLO_LEDtask
// check usb = watch_LEDtask
//static u8 memdevflag=0;
//void  HELLO_LEDtask(void *pdata)
//{
//	OS_ERR err;
//	u16 t;
//	//u16 cnt = 0;
//	printf("HELLO_LED task run\r\n");
//	while(1)
//	{
//		t++;
//			
//		// 只有在需要使用USB的时候开启即可
//		if(1)
//		{
//			if((t%60)==0)//600ms左右检测1次
//			{
//				if (usbx.hdevclass == 1)
//				{
//					if (memdevflag == 0) //U盘不在位?
//					{
//						fs[1]->drv=2;  //暂时认为也可以不加,我认为可能是重命名
//						f_mount(fs[1],"1:",1); 	//重新挂载U盘
//						usbapp_user_app();
//						usbConnectStateSet(1);
//						memdevflag = 1;
//					}
//				}
//				else
//					memdevflag = 0; // U盘被拔出了
//			}
//			// U盘模式,才处理
//			while ((usbx.bDeviceState & 0XC0) == 0X40) //USB设备插入了,但是还没连接成功,猛查询.
//			{
//				usbapp_pulling(); //轮询处理USB事务
//				delay_us(1000);   //不能像HID那么猛...,U盘比较慢
//			}
//			usbapp_pulling(); //处理USB事务
//		}
//		UART_TCPbuff_Run(F407USART3_buffRead);
//		OSTimeDly(8,OS_OPT_TIME_DLY,&err);
//	}									 
//}
// <e>HELLO_LEDtask

static u8 memdevflag=0;
void  HELLO_LEDtask(void *pdata)
{
	OS_ERR err;
	u16 t;
	//u16 cnt = 0;
	printf("HELLO_LED task run\r\n");
	while(1)
	{
		t++;

		// 只有在需要使用USB的时候开启即可
		if(usbConnectSwitchGet())
		{
			if((t%60)==0)
			{
				if(usbx.hdevclass==1)
				{
					//printf("U disk is ON,memdevflag=%d\r\n",memdevflag);
					if(memdevflag==0) // U盘不在位?
					{
						fs[1]->drv=2; // 暂时认为也可以不加,我认为可能是重命名
						f_mount(fs[1],"1:",1); 	// 重新挂载U盘
						usbapp_user_app();

						usbConnectStateSet(1);						
						memdevflag=1;
					}  
				}else memdevflag=0; // U盘被拔出了
			}
			
			while((usbx.bDeviceState&0XC0)==0X40) // USB设备插入了,但是还没连接成功,猛查询.
			{
				usbapp_pulling(); // 轮询处理USB事务
				delay_ms(1); // 不能像HID那么猛...,U盘比较慢
				#if (EN_log_print >= 2)
				printf(".");
				#endif // EN_log_print
			}
			usbapp_pulling();//????USB????
			#if (EN_log_print >= 2)
			printf("\\");
			#endif // EN_log_print
		}
		UART_TCPbuff_Run(F407USART3_buffRead);
		OSTimeDly(5,OS_OPT_TIME_DLY,&err);
	}									 
}

// <s>MainTask
void openUSB(void);
void closeUSB(void);
u8 bufmessages[1500];
static void MainTask(void *p_arg) // test fun
{
    OS_ERR err;
    u8 res;

    while (1)
    {
		// 发送数据
		// 将会以1Hz频率闪烁两次
        if (function_f == 1)
        {
            if (key1_down == 0 && key2_down==0)
            {
				//u8 res;
				//u8* messages;
				
				//messages = mymalloc(SRAMIN, 1500);
				//if(messages)
					//F407USART1_SendString("mymalloc\r\n");
				
				LED_GREEN_OFF();
				
				//#if USART1_DEBUG
				F407USART1_SendString("Data_Packing_sens\r\n");
				//#endif
				Data_Packing_sens((char *)bufmessages);
				mysend_data((char *)bufmessages);
				//mysend_data("123456\r\n");
				delay_ms(500);
				LED_GREEN_OFF();
				delay_ms(500);
				
				LED_GREEN_OFF();
				//Data_Packing_gps((char *)bufmessages);
				//#if USART1_DEBUG
				F407USART1_SendString("Data_Packing_gps\r\n");
				//#endif
				//mysend_data((char *)bufmessages);

				delay_ms(500);
				LED_GREEN_OFF();
				
				//myfree(SRAMIN, messages);		

				//mycheck_Queue();
				function_f = 0;
            }
        }
// 打开扫描模式 模拟插入USB 		
        if (key1_down == 1)
        {
            F407USART1_SendString("key1_down..openUSB...\r\n");
            // 初始化USB
			usbConnectSwitchSet(1);  // 理论上需要先打开扫描模式
			usbapp_mode_set();  // 设置设置USB模式，复位USB
			// 连接相机
			usbConnectStateSet(0);  // 清空标志位
			openUSB();  // 打开相机的USB可电源
			// 等待连接成功
            res = waitUsbConnectFlag(10000);
            if (res == 1) // 正常打开相机
            {
				delay_ms(2000);
				printf("successful find usb，open camera!\r\n");
				mf_scan_files("1:DCIM/100IMAGE");
				mf_dcopy("1:DCIM/100IMAGE","0:INBOX",1);
				mf_scan_files("1:DCIM/100IMAGE");
				mf_scan_files("0:INBOX");
            }
            else
            {
				//mysend_log("Fail find camera");
                F407USART1_SendString("Fail WaitDistinguishflag...\r\n");
            }
			closeUSB();  // close usb power
			res = waitUsbDisonnectFlag(5000);
			if (res == 1) // 正常关闭相机
            {
				F407USART1_SendString("success closeUSB...\r\n");
			}
			else
			{
				F407USART1_SendString("closeUSB..fail\r\n");
			}
			usbapp_mode_stop();
			usbConnectSwitchSet(0);
			key1_down = 0;
		}
		// take photo
		if(key_wkup_down == 1)
		{
			if(usbConnectSwitchGet() == 0)
			{
				USB_Photograph();
				key_wkup_down = 0;
			}
		}
		// 发送图片
        if (key2_down == 1)
        {
			u8 i;
            F407USART1_SendString("key2_down..\r\n");
			//2Hz 2.5S
			for(i=0;i<3;i++)  
			{
				LED_GREEN_OFF();
				delay_ms(250);
				LED_GREEN_OFF();
				delay_ms(250);
			}
			
			mf_scan_files("0:INBOX");
			if(mqtt_state_get() == 1)
			{
				//mqtt_state_set(5); // 繁忙状态，禁止ping和发送数据
				mf_send_pics("0:INBOX","0:ARCH",1);
				mf_scan_files("0:INBOX");
				mf_scan_files("0:ARCH");
				//mqtt_state_set(1);
			}
			else
			{
			
			}

            key2_down = 0;
			//mqtt_state_set(1);  // 回复正常模式
            
//			if(usb_state_get() == 0)
//				USB_Photograph();
//			else
//				printf("warning:USB is connected\r\n");
        }
		

        OSTimeDly(1, OS_OPT_TIME_DLY, &err);
    }
}
// <e>MainTask

//	InitQueue(&Q_stage);  // 初始化队列 
//	InitQueue(&Q_resent);  // 初始化队列
void check_response(u8* load, int len)
{
	u8 i,cnt;
	u32 uid=0;
	QElemType elem;
	if(load[0]==0xa5)
	{
		if(load[1]==0x96)
		{
			uid = (load[2]<<24) +  (load[3]<<16) + (load[4]<<8) +  load[5];
			// printf(">>>>>>>>>>>>>>>>>>>>>>>>R*UID:%0X\r\n",uid);
			printf(">>>>>RUID:[%3d,%3d,%3d],Q-UID:%08X,pid:%04X\r\n", 0, 0, 0, uid, 0XFFFF);
			// visit Q_stage squeue
			cnt=0;
			i=Q_stage.front;
			while(i!=Q_stage.rear && cnt++<90)
			{
				// printf("<VUID:%0X,%3d,pic=%3d\r\n",Q_stage.data[i].uid, i, Q_stage.data[i].pic_id);
				printf("<<<<<VUID:[%3d,%3d,%3d],Q-UID:%08X,pid:%04X\r\n", i,Q_stage.rear,(Q_stage.rear-i+MAXSIZE)%MAXSIZE,Q_stage.data[i].uid,Q_stage.data[i].pack_id);
				if(Q_stage.data[i].uid==uid)
				{
					while(--cnt)  //弹出所有的多多余的缓存
					{
						DeQueue(&Q_stage, &elem);
						printf("*-Q_stage:[%3d,%3d,%3d],Q-UID:%08X,pid:%04X\r\n",Q_stage.front,Q_stage.rear,QueueLength(Q_stage),elem.uid,elem.pack_id);
						EnQueue(&Q_resent,elem);
						//printf("*+Q_resnt:[%3d,%3d,%3d],Q+UID:%08X,pid:%04X\r\n", Q_resent.front, Q_resent.rear, QueueLength(Q_resent), elem.uid,elem.pack_id);	
						printf("* Q_resnt:[%3d,%3d,%3d],Q*UID:%08X,pid:%04X\r\n", Q_resent.front, Q_resent.rear, QueueLength(Q_resent), Q_resent.data[Q_resent.front].uid, Q_resent.data[Q_resent.front].pack_id);						
					}
					
					DeQueue(&Q_stage, &elem);
					printf("- Q_stage:[%3d,%3d,%3d],Q-UID:%08X,pid:%04X\r\n",Q_stage.front,Q_stage.rear,QueueLength(Q_stage),elem.uid,elem.pack_id);
					// printf("* Q_resnt:[%3d,%3d,%3d],Q+UID:%08X,pid:%04X\r\n",Q_resent.front,Q_resent.rear,QueueLength(Q_resent),elem.uid,elem.pack_id);
					printf("* Q_resnt:[%3d,%3d,%3d],Q*UID:%08X,pid:%04X\r\n", Q_resent.front, Q_resent.rear, QueueLength(Q_resent), Q_resent.data[Q_resent.front].uid, Q_resent.data[Q_resent.front].pack_id);
					break;		
				}
				i=(i+1)%MAXSIZE;
				
			}
//			if(cnt==1)
//			{
//				DeQueue(&Q_stage, &elem);
//				printf("Q_stage->front=%d,Q_stage->rear=%d,cnt=%d\r\n",Q_stage.front,Q_stage.rear,cnt);
//			}
//			else if(cnt<89)
//			{
//				cnt--;
//				while(cnt--)
//				{
//					printf("DeQueue & EnQueue cnt=%d\r\n",cnt);
//					DeQueue(&Q_stage, &elem);
//					printf("Q_resent->front=%d,Q_stage->rear=%d\r\n",Q_stage.front,Q_stage.rear);
//					EnQueue(&Q_resent,elem);
//					printf("Q_resent->front=%d,Q_resent->rear=%d\r\n",Q_resent.front,Q_resent.rear);					
//				}
//				DeQueue(&Q_stage, &elem);
//				printf("Q_stage->front=%d,Q_stage->rear=%d,cnt=%d\r\n",Q_stage.front,Q_stage.rear,cnt);
//			}
		}
	}
}
// <e>MQTTRECEIVEtask
// 接收MQTT服务器的指令
static void MQTTRECEIVEtask(void *p_arg)
{
    OS_ERR err;
	
	

    int type; // 解析接收的数据值
    //===========================
    unsigned char dup;
    int qos;
    // 保留标志
    unsigned char retained;
    // 包id
    unsigned short packetid;
    // 主题名
    MQTTString topicName;

    // 数据
    unsigned char *payload;
    // 数据长度
    int payloadlen;
	//
	int test_cnt=0;
    //==============================
    while (1)
    {
        if (UART_TCP_buffLength() != 0)
        {
			#if (EN_log_print >= 3)
            F407USART1_SendString("+UART_TCP\r\n");
			#endif // EN_log_print	
            //处理接收到的MQTT消息，并根据不同的消息类型做不同的处理
            type = MQTTPacket_read(MQTT_Receivebuf, MQTT_RECEIVEBUFF_MAXLENGTH, transport_getdata);
            switch (type)
            {
            case CONNECT:
                break;
            case CONNACK:          //连接请求响应
				// mqtt_state_set(1); // 设置连接成功
				mqtt_subscribe(MY_TOPIC_MSGDOWN);
				printf("%s\r\n",MY_TOPIC_MSGDOWN);
                F407USART1_SendString("MQTT Connect CONNACK\r\n");
                break;
            case PUBLISH: //订阅的消息,由别人发布
                if (MQTTDeserialize_publish(&dup, &qos, &retained, &packetid, &topicName, &payload, &payloadlen,
                                            MQTT_Receivebuf, MQTT_RECEIVEBUFF_MAXLENGTH) == 1)
                {					
//					// 解析接收到的字符串
//					if (MystringSeek((uint8_t *)(topicName.lenstring.data), topicName.lenstring.len, (uint8_t *)MY_TOPIC_PICDOWN, strlen(MY_TOPIC_PICDOWN)) != -1)
//					{ 
//						printf("\r\nTOPIC1=%s,payloadlen=%d\r\n", MY_TOPIC_PICDOWN, payloadlen);
//					}
					
					#if (EN_log_print >= 3)
					int i;
					F407USART1_SendString("payload:");
					for (i = 0; i < payloadlen; i++)
					{
						printf("%0X",payload[i]);
						// 打印缓存区域的内容
					}
					F407USART1_SendString("\r\n");
					#endif // EN_log_print	
					
//					test_cnt++;
//					if(test_cnt%10!=1)
//					{
						check_response(payload,payloadlen);
//						F407USART1_SendString("miss one and try resend\r\n");
//					}
                }
                break;
            case PUBACK: //发布消息响应，表示发布成功
                break;
            case PUBREC:
                break;
            case PUBREL:
                break;
            case PUBCOMP:
                break;
            case SUBSCRIBE:
                break;
            case SUBACK: //订阅消息ack	
				printf("MQTT subscrible SUBACK\r\n ");
				mqtt_state_set(1); // 设置连接成功
                break;
            case UNSUBSCRIBE:
                break;
            case UNSUBACK:
                break;
            case PINGREQ:
                break;
            case PINGRESP: //Ping返回
				mqtt_ping_state_set(1);
                break;
            case DISCONNECT:
                break; //由客户端发送给服务器，表示正常断开连接
            default:
                break;
            }
        }
        OSTimeDly(5, OS_OPT_TIME_DLY, &err);
    }
}
// <e>MQTTRECEIVEtask
//static vu16 my_heart=0;
// <s>SIM7100task
// 如何在使用的时候才创建任务，不用的时候不创建
u8 f_reconnect=0; 
//uint8_t sss = 0;
static void SIM7100task(void *p_arg)
{
    OS_ERR err;

    uint16_t time = 0;
    
	//MY_MQTT_ERR mqtterr;
	static u8 mqtt_connect_flag=0;
    F407USART1_SendString("SIM7100task run\r\n");
	ec25_Init();
    while (1)
    {        
        if (time % 10 == 0 && f_reconnect == 0)
        { //如果连接失败，每隔5秒会重新尝试连接一次
            if (mqtt_state_get() == 0)
            {
                mqtt_connect();
				printf("Try mqtt_connect f:%d\r\n",f_reconnect);
				
				//f_reconnect=0;
				mqtt_connect_flag++;  // 累计尝试连接次数
				if(mqtt_connect_flag*5>120)
				{
					SoftReset();
					delay_ms(1000);
				}
				
				//sss = 0;
				
				LED_YELLOW_OFF();
            }
        }
		if (time % 10 == 1)  // mqtt ping
		{
			if (mqtt_state_get() == 1 && function_f == 0 && key1_down==0 && key2_down==0)
            {
				u16 cnt=0;
				mqtt_Ping();
				printf("ping mqtt .....\r\n");
				while(!mqtt_ping_state_get())
				{
					cnt++;
					OSTimeDly(10,OS_OPT_TIME_DLY,&err);
					if(cnt>=500) // 5s
					{
						printf("none ping back .....\r\n");
						
						mqtt_state_set(0);
						LED_YELLOW_OFF();
						break;
					}
				}
				printf("receive ping back .....\r\n");
				mycheck_Queue();
				mqtt_ping_state_set(0);  // 清空标志位
            }
		}
		

        if (mqtt_state_get() == 1)
        {
//            if (sss == 0)
//            {
//				//如果连接MQTT服务器成功，会订阅一个picdown主题消息，可以根据需要决定是否判断响应，判断是否订阅成功
////                mqtt_subscribe(MY_TOPIC_PICDOWN);
////				delay_ms(10);
////                mqtt_subscribe(MY_TOPIC_MSGDOWN);
////				delay_ms(10);
////				//mysend_log((char*)"{\"log\":\"mqtt connect\"}");
////				sss = 1;
////				printf("mqtt_subscribe \r\n ");
//            }
			mqtt_connect_flag=0; // 一旦成功则清零
			LED_YELLOW_ON();
        }
        else
        {
			//sss=0;
            LED_YELLOW_OFF();
        }
		time++;
        time %= 600;
		//printf("time=%d ..... cflag%d\r\n",time,mqtt_connect_flag);
        OSTimeDly(1000, OS_OPT_TIME_DLY, &err);
    }
}
// <e>SIM7100task

// <s>SystemDatasBroadcast
void  SystemDatasBroadcast (void *p_arg)
{
  OS_ERR err;
  CPU_STK_SIZE free,used;
  (void)p_arg;
  
  while(DEF_TRUE)
  {
    IWDG_Feed();//喂狗
	OSTaskStkChk (&SystemDatasBroadcast_TCB,&free,&used,&err);
	
	#if USART1_DEBUG
	printf("SystemDatasBroadcast  used/free:%d/%d  usage:%%%d\r\n",used,free,(used*100)/(used+free));
    #endif
	OSTaskStkChk (&MainTaskTCB,&free,&used,&err);
	#if USART1_DEBUG
	printf("MainTaskTCB             used/free:%d/%d  usage:%%%d\r\n",used,free,(used*100)/(used+free));
	#endif	
	OSTaskStkChk (&MQTTRECEIVEtcb,&free,&used,&err);
	#if USART1_DEBUG
	printf("MQTTRECEIVEtcb             used/free:%d/%d  usage:%%%d\r\n",used,free,(used*100)/(used+free));
	#endif	
	OSTaskStkChk (&SIM7100tcb,&free,&used,&err);
	#if USART1_DEBUG
	printf("SIM7100tcb              used/free:%d/%d  usage:%%%d\r\n",used,free,(used*100)/(used+free));
	#endif	
	OSTaskStkChk (&KeyScan_TCB,&free,&used,&err);
	#if USART1_DEBUG
	printf("KeyScan_TCB          used/free:%d/%d  usage:%%%d\r\n",used,free,(used*100)/(used+free));
	#endif	
	OSTaskStkChk (&HELLO_LEDtcb,&free,&used,&err);
	#if USART1_DEBUG
	printf("HELLO_LEDtcb             used/free:%d/%d  usage:%%%d\r\n",used,free,(used*100)/(used+free));
	#endif
	if(watchdog_f*10>7200) //1h,SoftReset
	{
		printf("\r\n\r\n\r\nsleep\r\n\r\n");
		SoftReset();
	}
	//if((watchdog_f*10)%180 == 10)  // 5min过60s开始发送床拿起数据
	{
		function_f=1;
	}
	
	if((watchdog_f*10)%1800 == 70) // 30min过70s 开始扫描相机
	{
		key_wkup_down = 1;
	}
	if((watchdog_f*10)%1800 == 100) // 30min过70s 开始扫描相机
	{
		key1_down = 1;
	}
	if((watchdog_f*10)%1800 == 150) // 30min过130s 开始上传图片
	{
		key2_down = 1;
	}

	watchdog_f++;
	printf("<%d>-heart f:%d,ka:%d,kb:%d\r\n\r\n\r\n",watchdog_f,function_f,key1_down,key2_down);
	OSTimeDlyHMSM(0,0,10,0,(OS_OPT)OS_OPT_TIME_DLY,(OS_ERR*)&err);  
   }
}
// <e>SystemDatasBroadcast

//============================================================
//系统时钟168M
void RCC_Config(void)
{
    RCC_DeInit(); 
    RCC_HSEConfig(RCC_HSE_ON);  
    if(RCC_WaitForHSEStartUp() == SUCCESS) 
    {
        RCC_PLLCmd(DISABLE); 
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); 
        RCC_HCLKConfig(RCC_SYSCLK_Div1);  
        RCC_PCLK1Config(RCC_HCLK_Div4);  
        RCC_PCLK2Config(RCC_HCLK_Div2); 
        RCC_PLLConfig(RCC_PLLSource_HSE,8,336,2,7);
        RCC_PLLCmd(ENABLE); 
        while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
    }
}
//功能： 系统定时器和系统定时中断配置
//参数： void
//返回： void
//说明：
void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;	
	//
	//SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
	SysTick->CTRL&=~(1<<2);									//SYSTICK使用外部时钟源,不分频
	//
	SysTick->CTRL|=1<<1;   									//开启SYSTICK中断
	SysTick->LOAD=20000;   								//当系统时钟为120M时，产生1ms中断,大约
	//
	NVIC_InitStructure.NVIC_IRQChannel =RCC_IRQn;							//中断通道为系统时钟中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;		//先占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=3;					//从优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;						//使能中断
	NVIC_Init(&NVIC_InitStructure);	
	//
	SysTick->CTRL|=1<<0;   		//开启SYSTICK
}//*/
