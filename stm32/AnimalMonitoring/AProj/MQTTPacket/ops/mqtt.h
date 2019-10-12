#ifndef __MQTT_H
#define __MQTT_H
/* Includes ------------------------------------------------------------------*/

#include "sys.h"


/* Exported Functions --------------------------------------------------------*/

#define CLIENTID            	"005a003f3436510b32323534"
#define KEEP_ALIVE_INTERVAL 	60
#define CLEANSESSION        	1
#define C_USERNAME        		"admin"

////*
#define C_PASSWORD      			"qiaojiaoshou123"
#define MQTT_SEERVER_IP				"210.77.68.221"
#define MQTT_SEERVER_PORT			10020
////*/


#define MY_TOPIC_PICDOWN					"A129FB112111/picdown"
#define MY_TOPIC_PICUP						"A129FB112111/picup"
#define MY_TOPIC_MSGDOWN					"A129FB112111/msgdown"
#define MY_TOPIC_MSGUP						"A129FB112111/msgup"


/*
参数1：读取数据偏移位置
参数2：读取数据长度
参数3：buff指针，将读取到的数据写入到该buff中
返回：返回读取到的实际数据长度
*/
#define MY_GET_PICDATA_CALLBACK			u32 (*getdata)(u32,u32,char *)





typedef enum 
{
	MY_MQTT_ERR_NONE=0,
	//内存溢出
	MY_MQTT_ERR_MEMOVER,
	//数据错误
	MY_MQTT_ERR_DATA_ERR,
	//获取数据错误
	MY_MQTT_ERR_GETDATA_FAIL,
	//MQTT发送错误
	MY_MQTT_ERR_SEND_FAIL,
	//响应超时
	MY_MQTT_ERR_ACK_TIMEOUT
}MY_MQTT_ERR;


// 接收缓存区
#define MQTT_RECEIVEBUFF_MAXLENGTH		2048
extern u8 MQTT_Receivebuf[MQTT_RECEIVEBUFF_MAXLENGTH];
// 发送缓存区
#define MQTT_SEND_BUFF_MAXLENGTH		12000
extern u8 MQTT_SEND_buf[MQTT_SEND_BUFF_MAXLENGTH];
// MQTT缓存区
#define MQTT_DATA_BUFF_MAXLENGTH		12000
extern u8 MQTT_DATA_buf[MQTT_DATA_BUFF_MAXLENGTH];


unsigned char mqtt_state_get(void);
void mqtt_state_set(unsigned char state);


void picdown_Flag_set(u8 value);
u8 picdown_Flag_get(void);





//订阅
int mqtt_subscrib(char *pTopic,char *pMessage);

//建立连接
int mqtt_connect(u8 *buf,u16 buflen);

//断开连接
int mqtt_disconnect(void);
//发布,前提是要先建立好了链接

int mqtt_publish_s(char *pTopic,char *pMessage,u16 msglen,u8 dup, u8 qos,u8 retained,u8 *buf,u16 buflen);

int mqtt_publish_Receive(u8 *data,u16 datalen);

//订阅
int mqtt_subscribe(char *pTopic);
//检查缓存区
//int mqtt_subscribe_check(char *pTopic,char *pMessage);
//检查订阅内容
void checkMQTTRECEIVE(void);


#define FILE_PATH1 "0:/IMAG0012.jpg"
#define FILE_PATH10 "0:/IMAG0010.jpg"
#define FILE_PATH11 "0:/IMAG0011.jpg"
#define FILE_PATH12 "0:/IMAG0012.jpg"
#define FILE_PATH2 "0:/PIC.JPG"

//unsigned char send_picture(char* file_path, u16  packagelen, int send_picture_cnt, u16 timeOut);
MY_MQTT_ERR send_picture(u8 *mqttbuff,u32 mqttbufflength,	// MQTT消息包需要的buff空间以及长度
						 u8 *pic_buff,u32 pic_bufflen,	// 单个图片分包需要的buff空间以及buff实际长度
						 char* file_path,						// 图片路径
						 char* file_path_d,						// 图片路径(发送完成后)
						 int picture_id,						// 图片id
						 u32 packagelen,						// 图片分片长度
						 u16 timeOut);						// 等待picdonw响应，如果不需要可以注释

						 MY_MQTT_ERR mysend_picture(char* file_path,						// 图片路径
						 char* file_path_d,						// 图片路径(发送完成后)
						 int picture_id);						// 图片id	
													
MY_MQTT_ERR send_data(u8 *mqttbuff,u32 mqttbufflength, char* msg)	;
#endif

