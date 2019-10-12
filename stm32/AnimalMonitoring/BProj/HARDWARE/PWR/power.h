#ifndef __POWER_H
#define __POWER_H
#include "sys.h"

// Power control Interface
#define POWER_MP 		PEout(9)   // MP1584, active=1,default=0
#define POWER_MIC       PEout(10)  // MIC30032,active=1,default=0
//------------------------------------
#define POWER_4G		POWER_MIC   

#define POWER_OUT 		PEout(11)  // 5V out, active=1,default=1
#define POWER_CAM 		PEout(12)  // 5V DC in, active=1,default=1

#define POWER_USB 		PEout(13)  // 5V USB, active=1,default=0

#define POWER_OUT3V3 	PEout(14)  // 3.3V out, active=1,default=0
#define POWER_OUT5 		PEout(15)  // 5V out, active=1,default=0

// Camera control Interface
#define USB_CNT PEout(5) // 8TX-????3???USB
#define USB_PHO PEout(6) // 7GND????5?????

// Power 
void Power_Ctrl_Init(void); //≥ı ºªØ

// Camera
void Cam_Crtl_Init(void);

void USB_Connecting(u8 ctrl);
void USB_Photograph(void);

void USB_CONNECT_ON(void);
void USB_CONNECT_OFF(void);
void openUSB(void);
void closeUSB(void);

#endif
