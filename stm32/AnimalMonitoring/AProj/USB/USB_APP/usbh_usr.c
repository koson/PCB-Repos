#include "usbh_usr.h" 
#include "exfuns.h"  
#include "usart.h" 
#include "delay.h"
#include "power.h"
#include "includes.h"   
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//USBH-USR 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/7/22
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//*******************************************************************************
//修改信息
//无
////////////////////////////////////////////////////////////////////////////////// 	   

static u8 AppState;
USBH_HOST  USB_Host;
USB_OTG_CORE_HANDLE  USB_OTG_Core;


_usb_app usbx;//USB APP控制器 


unsigned char  usb_state=0;
unsigned char  usb_state_get(void)
{
	return usb_state;
}
void usb_state_set(unsigned char state)
{
	usb_state=state;
}


unsigned char  usb_cflag=0;
unsigned char  usb_cflag_get(void)
{
	return usb_cflag;
}
void usb_cflag_set(unsigned char state)
{
	usb_cflag=state;
}

u8 memdevflag=0;
//用户测试主程序
//返回值:0,正常
//       1,有问题
u8 USB_CON(void)
{ 
	u32 total,free;
	u8 res=0;
	printf("设备连接成功!\r\n");
	res=exf_getfree("1:",&total,&free);
	if(res==0)
	{
		printf("FATFS OK!\r\n");
		printf("U Disk Total Size:%d MB\r\n",total);
		printf("U Disk  Free Size:%d MB\r\n",free);
	} 
	else
	{
		printf("01:Failed to get capacity\r\n");
	}
	return res;
} 

//USB OTG 中断服务函数
//处理所有USB中断
void OTG_FS_IRQHandler(void)
{
	OSIntEnter();  
  	USBH_OTG_ISR_Handler(&USB_OTG_Core);
	OSIntExit();  
} 

//USB结束当前工作模式
void usbapp_mode_stop(void)
{		
	RCC->AHB2RSTR|=1<<7;	//USB OTG FS 复位
	delay_ms(5);
	RCC->AHB2RSTR&=~(1<<7);	//复位结束   
	memset(&USB_OTG_Core,0,sizeof(USB_OTG_CORE_HANDLE));
	memset(&USB_Host,0,sizeof(USB_Host));
	usbx.bDeviceState=0;
	usbx.hdevclass=0; 
	usb_cflag_set(0);
}

//设置USB工作模式
//mode:0,USB HOST MSC模式(默认模式,接U盘)
//	   1,USB HOST HID模式(驱动鼠标键盘等)
//	   2,USB Device MSC模式(USB读卡器) 
void usbapp_mode_set(void)
{
	USB_CONNECT_ON();  // 打开USB电源
	usbapp_mode_stop();  // 先停止当前USB工作模式
	
	USBH_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USB_Host,&USBH_MSC_cb,&USR_Callbacks);  

}

//USB枚举状态死机检测,防止USB枚举失败导致的死机
//phost:USB_HOST结构体指针
//返回值:0,没有死机
//       1,死机了,外部必须重新启动USB连接.
u8 USBH_Check_EnumeDead(USBH_HOST *phost)
{
	static u16 errcnt=0;
	//这个状态,如果持续存在,则说明USB死机了.
	if(phost->gState==HOST_CTRL_XFER&&(phost->EnumState==ENUM_IDLE||phost->EnumState==ENUM_GET_FULL_DEV_DESC))
	{
		errcnt++;
		if(errcnt>2000)//死机了
		{ 
			errcnt=0;
			RCC->AHB2RSTR|=1<<7;	//USB OTG FS 复位
			delay_ms(5);
			RCC->AHB2RSTR&=~(1<<7);	//复位结束  
			return 1;
		} 
	}else errcnt=0;
	return 0;
} 

//USB轮询函数,必须周期性的被调用.
void usbapp_pulling(void)
{

	//if(HCD_IsDeviceConnected(&USB_OTG_Core)==0||usbx.hdevclass!=1)//U盘连接失败了,需要重新处理USB事务
	{
		USBH_Process(&USB_OTG_Core, &USB_Host);//处理USB事务
		if(USBH_Check_EnumeDead(&USB_Host))	//检测USB HOST 是否死机了?死机了,则重新初始化 
		{ 	  
			printf("usbapp_mode_set\r\n");
			usbapp_mode_set();//重连
		}	
	}			
	
}
//USB HOST 用户回调函数.
USBH_Usr_cb_TypeDef USR_Callbacks=
{
	USBH_USR_Init,
	USBH_USR_DeInit,
	USBH_USR_DeviceAttached,
	USBH_USR_ResetDevice,
	USBH_USR_DeviceDisconnected,
	USBH_USR_OverCurrentDetected,
	USBH_USR_DeviceSpeedDetected,
	USBH_USR_Device_DescAvailable,
	USBH_USR_DeviceAddressAssigned,
	USBH_USR_Configuration_DescAvailable,
	USBH_USR_Manufacturer_String,
	USBH_USR_Product_String,
	USBH_USR_SerialNum_String,
	USBH_USR_EnumerationDone,
	USBH_USR_UserInput,
	USBH_USR_MSC_Application,
	USBH_USR_DeviceNotSupported,
	USBH_USR_UnrecoveredError
};
/////////////////////////////////////////////////////////////////////////////////
//以下为各回调函数实现.

//USB HOST 初始化 
void USBH_USR_Init(void)
{
	USB_OTG_GINTMSK_TypeDef intmsk;
	intmsk.d32=USB_OTG_READ_REG32(&USB_OTG_Core.regs.GREGS->GINTMSK);
	intmsk.b.usbsuspend=0;	//关闭挂起中断 
	USB_OTG_WRITE_REG32(&USB_OTG_Core.regs.GREGS->GINTMSK,intmsk.d32); 
	printf("USB OTG HS MSC Host\r\n");
	printf("> USB Host library started.\r\n");
	printf("  USB Host Library v2.1.0\r\n\r\n");
	
}
//检测到U盘插入
void USBH_USR_DeviceAttached(void)//U盘插入
{
	usbx.bDeviceState|=1<<6;//检测到USB插入
	printf("检测到USB设备插入!\r\n");
}
//检测到U盘拔出
void USBH_USR_DeviceDisconnected (void)//U盘移除
{
	printf("USB设备拔出!\r\n");
	usbx.bDeviceState=0;//连接失效
	usbx.hdevclass=0;	//设备标记清零 
	
	usb_state_set(0);  // 标记设备已经正常断开
	// delay_ms(100);
	// USB_CONNECT_OFF();  // 关闭电源
}  
//复位从机
void USBH_USR_ResetDevice(void)
{
	printf("复位设备...\r\n");
}
//检测到从机速度
//DeviceSpeed:从机速度(0,1,2 / 其他)
void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed)
{
	if(DeviceSpeed==HPRT0_PRTSPD_HIGH_SPEED)
	{
		printf("高速(HS)USB设备!\r\n");
 	}  
	else if(DeviceSpeed==HPRT0_PRTSPD_FULL_SPEED)
	{
		printf("全速(FS)USB设备!\r\n"); 
	}
	else if(DeviceSpeed==HPRT0_PRTSPD_LOW_SPEED)
	{
		printf("低速(LS)USB设备!\r\n");  
	}
	else
	{
		printf("设备错误!\r\n");  
	}
}
//检测到从机的描述符
//DeviceDesc:设备描述符指针
void USBH_USR_Device_DescAvailable(void *DeviceDesc)
{ 
	USBH_DevDesc_TypeDef *hs;
	hs=DeviceDesc;   
	printf("VID: %04Xh\r\n" , (uint32_t)(*hs).idVendor); 
	printf("PID: %04Xh\r\n" , (uint32_t)(*hs).idProduct); 
}
//从机地址分配成功
void USBH_USR_DeviceAddressAssigned(void)
{
	printf("从机地址分配成功!\r\n");   
}
//配置描述符获有效
void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc,
                                          USBH_InterfaceDesc_TypeDef *itfDesc,
                                          USBH_EpDesc_TypeDef *epDesc)
{
	USBH_InterfaceDesc_TypeDef *id; 
	id = itfDesc;   
	if((*id).bInterfaceClass==0x08)
	{
		printf("可移动存储器设备!\r\n"); 
	}else if((*id).bInterfaceClass==0x03)
	{
		printf("HID 设备!\r\n"); 
	}    
}
//获取到设备Manufacturer String
void USBH_USR_Manufacturer_String(void *ManufacturerString)
{
	printf("Manufacturer: %s\r\n",(char *)ManufacturerString);
}
//获取到设备Product String 
void USBH_USR_Product_String(void *ProductString)
{
	printf("Product: %s\r\n",(char *)ProductString);  
}
//获取到设备SerialNum String 
void USBH_USR_SerialNum_String(void *SerialNumString)
{
	printf("Serial Number: %s\r\n",(char *)SerialNumString);    
} 
//设备USB枚举完成
void USBH_USR_EnumerationDone(void)
{ 
	printf("设备枚举完成!\r\n\r\n");    
} 
//无法识别的USB设备
void USBH_USR_DeviceNotSupported(void)
{
	printf("无法识别的USB设备!\r\n\r\n");    
}  
//等待用户输入按键,执行下一步操作
USBH_USR_Status USBH_USR_UserInput(void)
{ 
	printf("跳过用户确认步骤!\r\n");
	usbx.bDeviceState|=0X80;		//标记已连接
	return USBH_USR_RESP_OK;
} 
//USB接口电流过载
void USBH_USR_OverCurrentDetected (void)
{
	printf("端口电流过大!!!\r\n");
} 
//用户要求重新初始化设备
void USBH_USR_DeInit(void)
{
  	AppState=USH_USR_FS_INIT;
	printf("重新初始化!!!\r\n");
}
//无法恢复的错误!!  
void USBH_USR_UnrecoveredError (void)
{
	printf("无法恢复的错误!!!\r\n\r\n");	
}

//USB HOST MSC类用户应用程序
int USBH_USR_MSC_Application(void)
{
	u8 res=0;
  	switch(AppState)
  	{
    	case USH_USR_FS_INIT://初始化文件系统 
			printf("开始执行用户程序!!!\r\n");
			AppState=USH_USR_FS_TEST;
      		break;
    	case USH_USR_FS_TEST:	//执行USB OTG 测试主程序
			//res=USH_User_App(); //用户主程序
     		//res=0;
			//if(res)AppState=USH_USR_FS_INIT;
			if(HCD_IsDeviceConnected(&USB_OTG_Core))//设备连接成功
			{
				usbx.hdevclass=1;	//检测到U盘插入
				//do nothing
			}else 
			{ 
				AppState=USH_USR_FS_INIT;
      		}
      		break;
    	default:break;
  	} 
	return res;
}

////////////////////////////////////////////////////////////////////////////////////////
//用户定义函数,实现fatfs diskio的接口函数 
extern USBH_HOST              USB_Host;

//获取U盘状态
//返回值:0,U盘未就绪
//      1,就绪
u8 USBH_UDISK_Status(void)
{
	return HCD_IsDeviceConnected(&USB_OTG_Core);//返回U盘状态
}

//读U盘
//buf:读数据缓存区
//sector:扇区地址
//cnt:扇区个数	
//返回值:错误状态;0,正常;其他,错误代码;		 
u8 USBH_UDISK_Read(u8* buf,u32 sector,u32 cnt)
{
	u8 res=1;
	if(HCD_IsDeviceConnected(&USB_OTG_Core)&&AppState==USH_USR_FS_TEST)//连接还存在,且是APP测试状态
	{  		    
		do
		{
			res=USBH_MSC_Read10(&USB_OTG_Core,buf,sector,512*cnt);
			USBH_MSC_HandleBOTXfer(&USB_OTG_Core ,&USB_Host);		      
			if(!HCD_IsDeviceConnected(&USB_OTG_Core))
			{
				res=USBH_MSC_FAIL;//读写错误
				break;
			};   
		}while(res==USBH_MSC_BUSY);
	}else res=USBH_MSC_FAIL;		  
	if(res==USBH_MSC_OK)res=USBH_MSC_OK;	
	return res;
}

//写U盘
//buf:写数据缓存区
//sector:扇区地址
//cnt:扇区个数	
//返回值:错误状态;0,正常;其他,错误代码;		 
u8 USBH_UDISK_Write(u8* buf,u32 sector,u32 cnt)
{
	u8 res=1;
	if(HCD_IsDeviceConnected(&USB_OTG_Core)&&AppState==USH_USR_FS_TEST)//连接还存在,且是APP测试状态
	{  		    
		do
		{
			res=USBH_MSC_Write10(&USB_OTG_Core,buf,sector,512*cnt); 
			USBH_MSC_HandleBOTXfer(&USB_OTG_Core ,&USB_Host);		      
			if(!HCD_IsDeviceConnected(&USB_OTG_Core))
			{
				res=1;//读写错误
				break;
			};   
		}while(res==USBH_MSC_BUSY);
	}else res=1;		  
	if(res==USBH_MSC_OK)res=0;	
	return res;
}












































