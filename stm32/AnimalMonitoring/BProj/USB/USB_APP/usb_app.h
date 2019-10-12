#ifndef __USB_APP_H
#define __USB_APP_H	 
#include "sys.h"
#include "usbh_msc_core.h"
//USB APP控制结构体
typedef struct 
{
	u8 bDeviceState; 	//USB状态标记
						//bit0:表示电脑正在向SD卡写入数据
						//bit1:表示电脑正从SD卡读出数据
						//bit2:SD卡写数据错误标志位
						//bit3:SD卡读数据错误标志位
						//bit4:1,表示电脑有轮询操作(表明连接还保持着)
						//bit5:保留.
						//bit6:1,表示USB有设备插入;0,表示没有设备插入
						//bit7:1,表示USB已经连接;0,表示USB没有连接.
	u8 hdevclass; 		//USB HOST设备类型
						//1,U盘
}_usb_app;

extern _usb_app usbx;
extern USBH_HOST  USB_Host; 
extern USB_OTG_CORE_HANDLE  USB_OTG_Core;
 
void usbapp_mode_stop(void);
void usbapp_mode_set(void);

void usbapp_init(void);			//初始化USB
void usbapp_pulling(void);
void usbapp_mode_stop(void);
void usbapp_mode_set(void);
u8 usbapp_user_app(void);
// USB识别状态
u8 usbConnectSwitchGet(void);
void usbConnectSwitchSet(u8 state);
u8 usbConnectStateGet(void);
void usbConnectStateSet(u8 state);
u8 waitUsbConnectFlag(u16 timeout);
u8 waitUsbDisonnectFlag(u16 timeout);


#endif

















