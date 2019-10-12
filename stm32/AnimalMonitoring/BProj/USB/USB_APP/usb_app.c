#include "usb_app.h"
#include "usbh_usr.h"
#include "delay.h"
#include "power.h"
#include "string.h"
//USB HOST
USBH_HOST USB_Host;
USB_OTG_CORE_HANDLE USB_OTG_Core;
_usb_app usbx; //USB APP控制器

static vu8 usbConnectFlag = 0; // 标记USB连接状态
u8 usbConnectStateGet(void)
{
	return usbConnectFlag;
}
void usbConnectStateSet(u8 state)
{
	usbConnectFlag = state;
}

static vu8 usbConnectSwitch = 0; // 打开USB开关
u8 usbConnectSwitchGet(void)
{
	return usbConnectSwitch;
}
void usbConnectSwitchSet(u8 state)
{
	usbConnectSwitch = state;
}
/**
 * @description: 等待USB连接成功
 * @param timeout 等待时间
 * @return: 失败超时返回0，成功返回1
 */
u8 waitUsbConnectFlag(u16 timeout)
{
	u16 temp = 0;
	while (!usbConnectStateGet())
	{
		// usbapp_pulling();
		delay_ms(5);
		temp += 5;
		if (temp >= timeout)
		{
			return 0;
		}
	}
	return 1;
}
/**
 * @description: 等待USB断开
 * @param timeout 等待时间
 * @return: 失败超时返回0，成功返回1
 */
u8 waitUsbDisonnectFlag(u16 timeout)
{
	u16 temp = 0;
	while (usbConnectStateGet() != 0)
	{
		delay_ms(5);
		temp += 1;
		if (temp >= timeout)
		{
			return 0;
		}
	}
	return 1;
}
/**
 * @description: USB OTG 中断服务函数,这里仅使用从机模式
 * @param none
 * @return: none
 */
void OTG_FS_IRQHandler(void)
{
	OSIntEnter();
	USBH_OTG_ISR_Handler(&USB_OTG_Core);
	OSIntExit();
}
/**
 * @description: usb状态标志初始化
 * @param {type} 
 * @return: 
 */
//初始化USB
void usbapp_init(void)
{
	usbx.bDeviceState = 0;
	usbx.hdevclass = 0;
	usbConnectStateSet(0);  // 连接成功标志
	usbConnectSwitchSet(0);
}
/**
 * @description: USB轮询函数,必须周期性的被调用
 * @param void
 * @return: void
 */
void usbapp_pulling(void)
{
	if (HCD_IsDeviceConnected(&USB_OTG_Core) == 0 || usbx.hdevclass != 1) // U盘连接失败了,需要重新处理USB事务
	{
		USBH_Process(&USB_OTG_Core, &USB_Host); // 处理USB事务
		if (USBH_Check_EnumeDead(&USB_Host))	// 检测USB HOST 是否死机了?死机了,则重新初始化
		{
			printf("usbapp_mode_set\r\n");
			usbapp_mode_set(); // 重连
		}
	}
}
/**
 * @description: USB结束当前工作模式
 * @param none
 * @return: none
 */
void usbapp_mode_stop(void)
{		
	RCC->AHB2RSTR|=1<<7; // USB OTG FS 复位
	delay_ms(5);
	RCC->AHB2RSTR&=~(1<<7); // 复位结束
	memset(&USB_OTG_Core,0,sizeof(USB_OTG_CORE_HANDLE));
	memset(&USB_Host,0,sizeof(USB_Host));
	usbx.bDeviceState=0;
	usbx.hdevclass=0; 
	usbConnectStateSet(0);  // 连接状态清零
}
/**
 * @description: 设置USB工作模式为USB HOST MSC模式(默认模式,接U盘)
 * @param void
 * @return: void
 */
void usbapp_mode_set(void)
{
	usbapp_mode_stop();  // 复位USB	
	USBH_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USB_Host,&USBH_MSC_cb,&USR_Callbacks);  

}
/**
 * @description: 用户测试程序
 * @param void
 * @return: USB读取状态
 */
u8 usbapp_user_app(void)
{
	u32 total, free;
	u8 res = 0;

	res = exf_getfree("1:", &total, &free);
	if (res == 0)
	{
		printf("U Disk Total Size:%d MB\r\n", total);
		printf("U Disk  Free Size:%d MB\r\n", free);
	}
	else
	{
		printf("01:Failed to get capacity\r\n");
	}
	return res;
}
