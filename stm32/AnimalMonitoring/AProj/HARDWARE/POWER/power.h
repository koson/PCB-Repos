#ifndef __POWER_H
#define __POWER_H
#include "sys.h"
/*
- DEVC_CRL  -PC0
- GPS_CRL   -PC1
- USB_CRL   -PC2
 */

//控制端口定义
#define VCC_DVC   PBout(13)	
#define VCC_GPS   PBout(14)	
#define VCC_USB   PBout(15)	

#define VCC_WIFI  PCout(4)	
#define VCC_4G    PCout(5)	

void Power_Ctrl_Init(void);//初始化	

void USB_CONNECT_ON(void);
void USB_CONNECT_OFF(void);


// 相机端口初始化
#define USB_CNT		PDout(0)	// 8TX-，内部为3，连接USB
#define USB_PHO		PDout(1)	// 7GND，内部为5，控制拍照

void Cam_Crtl_Init(void);
void USB_Connecting(u8 ctrl);
void USB_Photograph(void);

u8 openUSB(void);  // 打开USB
u8 closeUSB(void);  // 关闭USB
#endif
