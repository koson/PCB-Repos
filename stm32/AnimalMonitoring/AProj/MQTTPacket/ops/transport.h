/**
  ******************************************************************************
  * @file    transport.h
  * $Author: 飞鸿踏雪 $
  * $Revision: 17 $
  * $Date:: 2012-07-06 11:16:48 +0800 #$
  * @brief   以太网收发相关函数包装.
  ******************************************************************************
  * @attention
  *
  *<h3><center>&copy; Copyright 2009-2012, EmbedNet</center>
  *<center><a href="http:\\www.embed-net.com">http://www.embed-net.com</a></center>
  *<center>All Rights Reserved</center></h3>
  * 
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TRANSPORT_H
#define __TRANSPORT_H
/* Includes ------------------------------------------------------------------*/

/* Exported Functions --------------------------------------------------------*/

#include "sys.h"

#define TRANSPORT_DONE	1
#define TRANSPORT_AGAIN	0
#define TRANSPORT_ERROR	-1

// Set TCP/UDP Connections
#define TEST_CONNECTION_TYPE   1      // 0=WIFI module as UDP, 1=WIFI module as TCP Client, 2=WIFI module as TCP Server
#define TEST_LOCAL_PORT        0   // local port=0 then an updated random port will be generated each time. To avoid the rejection by some TCP server due to repeative same ip:port
#define TEST_REMOTE_IP_ADDR    "192.168.1.102" /*通讯猫MQTT,注意这里不是逗号*/
#define TEST_REMOTE_PORT        61618

int transport_sendPacketBuffer(unsigned char* buf, int buflen);
int transport_getdata(unsigned char* buf, int count);
int transport_open(void);
int transport_close(void);

#endif /* __MAIN_H */

/*********************************END OF FILE**********************************/
