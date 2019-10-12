#include "includes.h"

// 基础板载硬件
#include "led.h"
#include "key.h"  
#include "power.h"

// 文件系统
#include "malloc.h"  
#include "sdio_sdcard.h"
#include "ff.h"  
#include "exfuns.h"  
#include "usbh_usr.h" 
 
// 测试
#include "draft.h"

// 网络
#include "usart3.h" 
#include "ec25.h" 
#include "UART_TCPbuff.h"
#include "mqtt.h"

#include "mqtt.h"
#include "MQTTPacket.h"
#include "transport.h"
/////////////////////////UCOSII任务设置///////////////////////////////////
//START 任务
//设置任务优先级
#define START_TASK_PRIO      			10 //开始任务的优先级设置为最低
//设置任务堆栈大小
#define START_STK_SIZE  				256
//任务堆栈	
OS_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *pdata);			

//主任务
//设置任务优先级
#define MAIN_TASK_PRIO       			5 
//设置任务堆栈大小
#define MAIN_STK_SIZE  					3000
//任务堆栈	
OS_STK MAIN_TASK_STK[MAIN_STK_SIZE];
//任务函数
void main_task(void *pdata);

//按键扫描任务
//设置任务优先级
#define WATCH_TASK_PRIO       			4 
//设置任务堆栈大小
#define WATCH_STK_SIZE  					3000
//任务堆栈	
OS_STK WATCH_TASK_STK[WATCH_STK_SIZE];
//任务函数
void watch_task(void *pdata);

//按键扫描任务
//设置任务优先级
#define SCAN_TASK_PRIO       			3
//设置任务堆栈大小
#define SCAN_STK_SIZE  					3000
//任务堆栈	
OS_STK SCAN_TASK_STK[SCAN_STK_SIZE];
//任务函数
void scan_task(void *pdata);

// 订阅消息
#define EC25_TASK_PRIO       			6
//设置任务堆栈大小
#define EC25_STK_SIZE  					3000
//任务堆栈	
OS_STK EC25_TASK_STK[EC25_STK_SIZE];
//任务函数
void ec25_task(void *pdata);

// 解析消息
#define MQTTRECEIVE_TASK_PRIO       			7
//设置任务堆栈大小
#define MQTTRECEIVE_STK_SIZE  					300
//任务堆栈	
OS_STK MQTTRECEIVE_TASK_STK[MQTTRECEIVE_STK_SIZE];
//任务函数
void MqttReveiveTask(void *pdata);
//////////////////////////////////////////////////////////////////////////////

u8 key1_down=0;
u8 key2_down=0;
u8 wkup_down=0;

int main(void)
{ 
	u8 state;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  //初始化延时函数
	uart_init(115200);		//初始化串口波特率为115200
	printf("uart_init init\r\n");	
	usart3_init(115200);		//初始化串口波特率为115200
	LED_Init();					//初始化LED 
	KEY_Init(); 				//按键初始化  
	ec25_Init();
	Power_Ctrl_Init();  // 电源初始化
	Cam_Crtl_Init();  // 相机控制引脚初始化
	//state=ec25_soft_start();  // 启动EC25
	//printf("ec25_soft_start state:%d\r\n", state);
#ifdef USB_ENABLE
	//初始化USB主机
	//USBH_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USB_Host,&USBH_MSC_cb,&USR_Callbacks);
#endif	
	
//	if(!SD_Init()) printf("SD_Init ok\r\n");//判断SD卡是否存在
//	else printf("SD_Init Error\r\n");	

//	mymem_init(SRAMIN);		//初始化内部内存池	
// 	exfuns_init();			//为fatfs相关变量申请内存 
//  	f_mount(fs[0],"0:",1); 	//挂载SD卡  
//  	f_mount(fs[1],"1:",1); 	//挂载U盘 	
//	
  	OSInit();  	 				//初始化UCOSII
  	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//创建起始任务
	OSStart();	    
}
///////////////////////////////////////////////////////////////////////////////////////////////////
//开始任务
void start_task(void *pdata)
{
    OS_CPU_SR cpu_sr=0;
	pdata = pdata; 		   			  
	OSStatInit();					//初始化统计任务.这里会延时1秒钟左右	
 	OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)    				   
 	OSTaskCreate(main_task,(void *)0,(OS_STK*)&MAIN_TASK_STK[MAIN_STK_SIZE-1],MAIN_TASK_PRIO);	 				   
 	OSTaskCreate(watch_task,(void *)0,(OS_STK*)&WATCH_TASK_STK[WATCH_STK_SIZE-1],WATCH_TASK_PRIO);	
	OSTaskCreate(scan_task,(void *)0,(OS_STK*)&SCAN_TASK_STK[SCAN_STK_SIZE-1],SCAN_TASK_PRIO);
	OSTaskCreate(MqttReveiveTask,(void *)0,(OS_STK*)&MQTTRECEIVE_TASK_STK[MQTTRECEIVE_STK_SIZE-1],MQTTRECEIVE_TASK_PRIO);	
	OSTaskCreate(ec25_task,(void *)0,(OS_STK*)&EC25_TASK_STK[EC25_STK_SIZE-1],EC25_TASK_PRIO);			
 	OSTaskSuspend(START_TASK_PRIO);	//挂起起始任务.
	OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
}	  
//主任务
void main_task(void *pdata)
{	
	u8 flag=0;
	u8 state;
	printf("main_task run\r\n");	
	while(1)
	{
//		if(key1_down==1 && flag==0)
//		{
//			printf("key1_down..openUSB...\r\n");
//			usb_state_set(1);  // 设置标志启动USB
//			usbapp_mode_set(); 
//			openUSB();
//			state=WaitDistinguishflag(30000);
//			if(state==1)
//			{
//				printf("WaitDistinguishflag...\r\n");
//				
//				mydump_file();
//			}else
//			{
//				printf("Fail WaitDistinguishflag...\r\n");
//			}
//			key1_down=0;
//			flag=1;
//			
//		}
//		else if(key1_down==1 && flag==1)
//		{
//			printf("key1_down..closeUSB...\r\n");
//						
//			closeUSB();
//			
//			state=WaitDisconnectflag(3000);
//			if(state==1)
//			{
//				printf("USB_CONNECT_OFF...\r\n");
//				USB_CONNECT_OFF();
//				usbapp_mode_stop();
//				
//				//mysend_allpic();
//			}
//			else
//			{
//				printf("Fail USB_CONNECT_OFF...\r\n");
//			}
//			
//			//mydump_file_auto();
//			key1_down=0;
//			flag=0;
//		}
//		
//		if(key2_down==1)
//		{
//			printf("key2_down..closeUSB...\r\n");
//			USB_Photograph();
//			key2_down = 0;
//		}
		delay_ms(10);
	}
} 
extern u8 memdevflag; // USB是否连接成功
//按键扫描任务
void watch_task(void *pdata)
{	
	u32 t;
	//uint8 sss=0;
	
	while(1)
	{
		t++;
		if(t%1000 == 200) 
		{
			LED_GREEN_OFF();
		}
		if(t%1000 == 0)
		{
			t=0;
			LED_GREEN_ON();
		}
		if((t%600)==0)//600ms左右检测1次
		{
			//U盘在位检测   
			if(usbx.hdevclass==1)
			{
				//printf("U disk is ON,memdevflag=%d\r\n",memdevflag);
				if(memdevflag==0)//U盘不在位?
				{
					fs[1]->drv=2;  //暂时认为也可以不加,我认为可能是重命名
					f_mount(fs[1],"1:",1); 	//重新挂载U盘
					USB_CON();
					usb_cflag_set(1);  // 状态标记连接成功
					memdevflag=1;	// 标记U盘在位	
				}  
			}else memdevflag=0;	//U盘被拔出了 
		}		
		while((usbx.bDeviceState&0XC0)==0X40)//USB设备插入了,但是还没连接成功,猛查询.
		{
			
			usbapp_pulling();	//轮询处理USB事务
			delay_us(1000);		//不能像HID那么猛...,U盘比较慢
			UART_TCPbuff_Run(F407USART3_buffRead);
		}
		if(t%10==0) 
			usbapp_pulling();//处理USB事务,1/ 10ms
		
		UART_TCPbuff_Run(F407USART3_buffRead);
		delay_ms(1);
	}
}

// 小周期扫描任务
static void scan_task(void *pdata)
{
	u8 key;		    						 
	while(1)
	{
		key=KEY_Scan(0);   
		switch(key)
		{
			case KEY1_PRES:		//按下KEY_UP键
				key1_down=1;
				printf("KEY1_PRES\r\n");
				break;
			case KEY2_PRES:		//按下KEY_UP键
				key2_down=1;
				printf("KEY2_PRES\r\n");
				break;
			case WKUP_PRES:		//按下KEY_UP键
				wkup_down=1;
				printf("WKUP_PRES\r\n");
				break;
			default:break;
		}
		//UART_TCPbuff_Run();  // 读取串口缓存区的数据
		//checkMQTTRECEIVE();
 		delay_ms(5);
	}
}

static void ec25_task(void *pdata)
{
	//OS_CPU_SR cpu_sr=0;
	
	//char* text="1234567890";
	u32 time=0;
	
	u8 sss=0;
	
	
	printf("ec25_task run\r\n");
	while(1)
	{
		time++;
		if((time%5)==0)// 1000ms左右检测1次
		{
			if(mqtt_state_get()==0)  // 这里需要再加一个判断，即判ec25是否正常工作。
			{
				//OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)    	
				printf("try mqtt_connect\r\n");
				mqtt_connect(MQTT_SEND_buf,MQTT_SEND_BUFF_MAXLENGTH);
				sss=0;
				//OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
			}
				
		}
			
		if(mqtt_state_get()==1)
		{
			LED_YELLOW_ON();
			//int mqtt_publish_s(char *pTopic,char *pMessage,u16 msglen,u8 dup, u8 qos,u8 retained,u8 *buf,u16 buflen)
			//printf("test mqtt_publish_s\r\n");
			//mqtt_publish_s("AAA",text,10,0,1,0,MQTT_DATA_buf,MQTT_DATA_BUFF_MAXLENGTH);
			if(sss==0)
			{
				mqtt_subscribe(MY_TOPIC_PICDOWN);
				mqtt_subscribe(MY_TOPIC_MSGDOWN);
				sss=1;	
			}
		}
		else 
		{
			LED_YELLOW_OFF();
		}
		printf("time cnt=%d\r\n",time);
		delay_ms(1000);
	}
}

static void MqttReveiveTask(void *pdata)
{
		//===========================
	s32 type;
	// unsigned short submsgid;
	// int subcount;
	// int granted_qos;
	u8 dup;
	s32 qos;
	//保留标志
	u8 retained;
	//包id
	u16 packetid;
	MQTTString topicName;
	//char cstring[20];
	//数据	
	u8 *payload;
	//数据长度
	s32 payloadlen;
	//==============================
	printf("MqttReveiveTask run\r\n");
	while(1)
	{
		if(UART_TCP_buffLength()!=0)
		{
			printf("UART_TCP_buffLength !=0\r\n");
			//F407USART1_SendString("UART_TCP received data\r\n");
			//处理接收到的MQTT消息，并根据不同的消息类型做不同的处理
			type=MQTTPacket_read(MQTT_Receivebuf, MQTT_RECEIVEBUFF_MAXLENGTH, transport_getdata);
			switch(type)
			{
//				case CONNECT:break;
				case CONNACK://连接请求响应
					mqtt_state_set(1);
					break;
				case PUBLISH://订阅的消息,由别人发布
						if(MQTTDeserialize_publish(&dup,&qos,&retained,&packetid,&topicName,&payload,&payloadlen,
								MQTT_Receivebuf,MQTT_RECEIVEBUFF_MAXLENGTH)==1)
						{
							
							//if(MystringSeek(topicName.lenstring.data,topicName.lenstring.len,MY_TOPIC_MSGDOWN,strlen(MY_TOPIC_MSGDOWN))!=-1)
							if(MystringSeek(topicName.lenstring.data,topicName.lenstring.len,MY_TOPIC_PICDOWN,strlen(MY_TOPIC_PICDOWN))!=-1)
							{//??????
								printf("\r\nTOPIC1=%s,payloadlen=%d\r\n",MY_TOPIC_MSGDOWN,payloadlen);
								picdown_Flag_set(1);
							}
							else if(MystringSeek(topicName.lenstring.data,topicName.lenstring.len,MY_TOPIC_MSGDOWN,strlen(MY_TOPIC_MSGDOWN))!=-1)
							{//JSON??
								printf("TOPIC2=%s,payloadlen=%d\r\n",MY_TOPIC_MSGDOWN,payloadlen);
							}	
						}
//					if(MQTTDeserialize_publish(&dup,&qos,&retained,&packetid,&topicName,&payload,&payloadlen,
//							MQTT_Receivebuf,MQTT_RECEIVEBUFF_MAXLENGTH)==1)
//					{
//						//将MQTT消息反序列为数据，并判断消息的主题
//						if(MystringSeek(topicName.lenstring.data,topicName.lenstring.len,MY_TOPIC_PICDOWN,strlen(MY_TOPIC_PICDOWN)))
//						{//图片消息响应
//							printf("TOPIC{%s} down\r\n",MY_TOPIC_PICDOWN);
//							for(i=0;i<payloadlen;i++)
//								F407USART3_SendByte(payload[i]);
//							picdown_Flag_set(1);
//						}
//						else if(MystringSeek(topicName.lenstring.data,topicName.lenstring.len,MY_TOPIC_MSGDOWN,strlen(MY_TOPIC_MSGDOWN))!=-1)
//						{//JSON消息
//							printf("TOPIC{%s} down\r\n",MY_TOPIC_MSGDOWN);
//							for(i=0;i<payloadlen;i++)
//								F407USART3_SendByte(payload[i]);
//						}	
//					}
					break;
				case PUBACK://发布消息响应，表示发布成功
					break;
				case PUBREC:break;
				case PUBREL:break;
				case PUBCOMP:break;
				case SUBSCRIBE:break;
				case SUBACK://订阅消息ack
					break;
				case UNSUBSCRIBE:break;
				case UNSUBACK:break;
				case PINGREQ:break;
				case PINGRESP://Pin返回
					break;
				case DISCONNECT:break;//由客户端发送给服务器，表示正常断开连接
				default:break;
			}
		}
		delay_ms(5);
	}
}

