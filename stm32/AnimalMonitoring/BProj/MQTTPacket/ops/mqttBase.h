#ifndef __MQTTBASE_H
#define __MQTTBASE_H
#include "sys.h"

#define CLIENTID "005a003f3436510b32323534"
#define KEEP_ALIVE_INTERVAL 60
#define CLEANSESSION 1

#define C_USERNAME 					"admin"
#define C_PASSWORD      			"qiaojiaoshou123"
//#define MQTT_SEERVER_IP				"47.106.99.188"
#define MQTT_SEERVER_IP             "210.77.68.221"
#define MQTT_SEERVER_PORT			10020

#define MQTT_DATA_BUFF_MAXLENGTH 12000
extern uint8_t MQTT_DATA_buf[MQTT_DATA_BUFF_MAXLENGTH];
//#define C_PASSWORD "qiaojiaoshou123"
//#define MQTT_SEERVER_IP "210.77.68.221"
//#define MQTT_SEERVER_PORT 10020

//订阅
int mqtt_subscrib(char *pTopic, char *pMessage);
//建立连接
int mqtt_connect(void);
//断开连接
int mqtt_disconnect(void);
//发布,前提是要先建立好了链接
int mqtt_publish(char *pTopic, char *pMessage, unsigned short msglen);
//订阅
int mqtt_subscribe(char *pTopic);
//ping
int mqtt_Ping(void);
#endif
