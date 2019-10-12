#include "includes.h"

#include "MQTTPacket.h"
#include "transport.h"
#include "UART_TCPbuff.h"
#include "mqtt.h"

#include "fattester.h"	 
#include "sdio_sdcard.h"
#include "exfuns.h"
#include "malloc.h"		  
#include "ff.h"

// 接收缓存区
u8 MQTT_Receivebuf[MQTT_RECEIVEBUFF_MAXLENGTH];
// 发送缓存区
u8 MQTT_SEND_buf[MQTT_SEND_BUFF_MAXLENGTH];
// MQTT缓存区
u8 MQTT_DATA_buf[MQTT_DATA_BUFF_MAXLENGTH];


unsigned char  mqtt_state=0;

unsigned char  mqtt_state_get()
{
	return mqtt_state;
}
void mqtt_state_set(unsigned char state)
{
	mqtt_state=state;
}

u8 picdown_Flag=0;
void picdown_Flag_set(u8 value)
{
	picdown_Flag=value;
}

u8 picdown_Flag_get()
{
	return picdown_Flag;
}


/**
  * @brief  向服务器发送一个连接请求
  * @param  pTopic 消息主题，传入
  * @param  pMessage 消息内容，传出
  * @retval 小于0表示订阅消息失败
  */
int mqtt_connect(u8 *buf,u16 buflen)
{
	s32 len,rc;
	//u16 time;
	//u16 receivelength=0;

	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	//
	data.clientID.cstring = CLIENTID;
	//连接保持时间,秒
	data.keepAliveInterval = 255;
	//清理会话标志，1：每次上线后均重新订阅；0：每次上线后重新接受
	/*清理会话标志表示不和以前的连接记录保持关系，如果以前有订阅消息则需要重新订阅*/
	data.cleansession = 1;
	//登录用户名
	data.username.cstring = C_USERNAME;
	//登录密码
	data.password.cstring = C_PASSWORD;
	transport_open();
	delay_ms(200);

	len = MQTTSerialize_connect(buf, buflen, &data); /* 1 */
	rc = transport_sendPacketBuffer(buf,len);

	return rc;
}

int mqtt_disconnect(void)
{
	s32 len,rc;
	unsigned char buf[500];
	int buflen = 500;
	len = MQTTSerialize_disconnect(buf, buflen); /* 3 */
	rc = transport_sendPacketBuffer(buf,len);

	transport_close();//关闭端口

	return rc;
}


//发送一个MATT消息
//失败返回0 ，成功返回1
int mqtt_publish_s(char *pTopic,char *pMessage,u16 msglen,u8 dup, u8 qos,u8 retained,u8 *buf,u16 buflen)
{
	s32 len;
	u8 rc;
	MQTTString topicString = MQTTString_initializer;
	//主题
	topicString.cstring = pTopic;
	len = MQTTSerialize_publish(buf, buflen,dup,qos,retained, 0, topicString, (unsigned char*)pMessage, msglen);
	rc=transport_sendPacketBuffer(buf,len);
	return rc;
	}


/**
  * @brief  向服务器订阅一个消息，该函数会因为TCP接收数据函数而阻塞
  * @param  pTopic 消息主题，传入
  * @retval 小于0表示订阅消息失败
  */
int mqtt_subscribe(char *pTopic)
{
	int rc = 0;
	unsigned char buf[200];
	int buflen = 200;
	int msgid = 1;//包确认符号ID
	MQTTString topicString = MQTTString_initializer;
	int req_qos = 0;
	int len = 0;
	//int type=0;

	topicString.cstring = pTopic;
	//将需要发送的订阅数据类型序列化到buf当中准备发送
	len = MQTTSerialize_subscribe(buf, buflen, 0, msgid, 1, &topicString, &req_qos);
	//将序列化的数据发送出去
	rc = transport_sendPacketBuffer(buf, len);

	return rc;
}

int mqtt_Ping()
{
	int rc = 0;
	int len;
	unsigned char buf[200];
	len = MQTTSerialize_pingreq(buf, 200);//????
	rc = transport_sendPacketBuffer(buf, len);//??
	return rc;
}


void checkMQTTRECEIVE(void)
{
	//===========================
//	s32 type;
//	// unsigned short submsgid;
//	// int subcount;
//	// int granted_qos;
//	u8 dup;
//	s32 qos;
//	//保留标志
//	u8 retained;
//	//包id
//	u16 packetid;
//	MQTTString topicName;
//	//char cstring[20];
//	//数据	
//	u8 *payload;
//	//数据长度
//	s32 payloadlen;
//	//==============================
//	if(UART_TCP_buffLength()!=0)
//	{
//		//F407USART1_SendString("UART_TCP received data\r\n");
//		//处理接收到的MQTT消息，并根据不同的消息类型做不同的处理
//		type=MQTTPacket_read(MQTT_Receivebuf, MQTT_RECEIVEBUFF_MAXLENGTH, transport_getdata);
//		switch(type)
//		{
//			case CONNECT:break;
//			case CONNACK://连接请求响应
//				mqtt_state_set(1);
//				break;
//			case PUBLISH://订阅的消息,由别人发布
//					if(MQTTDeserialize_publish(&dup,&qos,&retained,&packetid,&topicName,&payload,&payloadlen,MQTT_Receivebuf,MQTT_RECEIVEBUFF_MAXLENGTH)==1)
//					{							
//						if(MystringSeek(topicName.lenstring.data,topicName.lenstring.len,MY_TOPIC_PICDOWN,strlen(MY_TOPIC_PICDOWN))!=-1)
//						{
//							printf("\r\nTOPIC1=%s,payloadlen=%d\r\n",MY_TOPIC_MSGDOWN,payloadlen);
//							// for(i=0;i<payloadlen;i++)
//							// {
//							// 	F407USART1_SendByte('*');
//							// 	F407USART1_SendByte(payload[i]);
//							// 	F407USART1_SendByte('*');
//							// }
//							picdown_Flag_set(1);
//						}
//						else if(MystringSeek(topicName.lenstring.data,topicName.lenstring.len,MY_TOPIC_MSGDOWN,strlen(MY_TOPIC_MSGDOWN))!=-1)
//						{
//							printf("TOPIC2=%s,payloadlen=%d\r\n",MY_TOPIC_MSGDOWN,payloadlen);
//						}	
//					}
//				break;
//			case PUBACK://发布消息响应，表示发布成功
//				break;
//			case PUBREC:break;
//			case PUBREL:break;
//			case PUBCOMP:break;
//			case SUBSCRIBE:break;
//			case SUBACK:break;//订阅消息ack
//			case UNSUBSCRIBE:break;
//			case UNSUBACK:break;
//			case PINGREQ:break;
//			case PINGRESP://Pin返回
//				break;
//			case DISCONNECT:break;//由客户端发送给服务器，表示正常断开连接
//			default:break;
//		}
//	}
}


#define SDCARD_BUFFER 8192
struct pick_package{
	u16  start_byte;            //起始字节，固定为0xa596
	u16  picture_id;            //图片序列，重新传输需要自动加1
	u16  packet_total_num;      //一共分成多少包
	u16  data_length;           //本包数据长度 
	u16  packet_id;             //本包图片的ID
	u32  data_total_length;     //图片数据总长度   
	//unsigned char 	data[data_len];		   //图片数据
};
u16 mypicPack_serialize(char *buf,  struct pick_package pkg)
{
	// 起始字节，固定为0xa596
	buf[0]=0xa5;
	buf[1]=0x96;
	// 图片序列，重新传输需要自动加1
	buf[2]=pkg.picture_id>>8;
	buf[3]=pkg.picture_id;
	
	//一共分成多少包
	buf[4]=pkg.packet_total_num>>8;
	buf[5]=pkg.packet_total_num;
	// 本包图片的ID
	buf[6]=pkg.packet_id>>8;
	buf[7]=pkg.packet_id;
	// 本包数据长度 
	buf[8]=pkg.data_length>>8;
	buf[9]=pkg.data_length;
	// 图片数据长度
	buf[10]=pkg.data_total_length>>24;
	buf[11]=pkg.data_total_length>>16;	
	buf[12]=pkg.data_total_length>>8;
	buf[13]=pkg.data_total_length;
	return 14;
}
	
MY_MQTT_ERR send_picture(u8 *mqttbuff,u32 mqttbufflength,	// MQTT消息包需要的buff空间以及长度
						 u8 *pic_buff,u32 pic_bufflen,	// 单个图片分包需要的buff空间以及buff实际长度
						 char* file_path,						// 图片路径
						 char* file_path_d,						// 图片路径(发送完成后)
						 int picture_id,						// 图片id
						 u32 packagelen,						// 图片分片长度
						 u16 timeOut)						// 等待picdonw响应，如果不需要可以注释
{
	// 包结构体
	struct pick_package pic_pack;

	// 文件读取测试
	FIL fl_in; 
	FRESULT res;
	u32 getLen;  // 每包实际发送数据长度，和packagelen对标
		
	u16 picPack_offset=0;  // 发送缓存区偏移量
	u8 reSendCount=0;  // 重发次数统计
	
	
	u32 otherLen;  // 余下没有发送的长度
	u16 time=0;  // 超时时间统计
	

	// 打开数据
	res = f_open(&fl_in,file_path,FA_OPEN_EXISTING|FA_READ);	//#to-do 查阅宏定义 ok
	if(res != FR_OK)
	{
		printf("MY_MQTT_ERR_GETDATA_FAIL <PATH:%s>\r\n",file_path);
		return MY_MQTT_ERR_GETDATA_FAIL;  // 后续这个需要定义
	}
	
	// 固定起始字节 
	pic_pack.start_byte=0x96a5;	

	// 每张图片唯一,后期根据读取的数据改变
	pic_pack.picture_id = picture_id;	
	printf("#test picture_id=%d\r\n",picture_id);
	// 读取文件大小
	pic_pack.data_total_length = f_size(&fl_in);  
	// 计算传输次数
	pic_pack.packet_total_num = (pic_pack.data_total_length/packagelen) + 1; 
	printf("\r\nf_open successfully <file_path:%s|file_size:%d|sub_num:%d|packagelen:%d>\r\n",\
	file_path,pic_pack.data_total_length,pic_pack.packet_total_num,packagelen);
	
	MQTT_RESEND:
	pic_pack.data_length = packagelen;
	// 处理
	f_lseek(&fl_in,0);  //将目录移动到问价起始位置
	otherLen = pic_pack.data_total_length;
	pic_pack.packet_id = 0;
	time = 0;  
	
	while(1)
	{
		// 获取数据
		if(otherLen > packagelen)  // 剩余数据大于图片分片长度
		{
			pic_pack.data_length = packagelen;
			// 序列化包头
			picPack_offset=mypicPack_serialize(pic_buff, pic_pack);
			f_read(&fl_in,pic_buff+picPack_offset,packagelen,&getLen);
			if(getLen!=packagelen)
			{
				f_close(&fl_in); 
				printf("MY_MQTT_ERR_GETDATA_FAIL <rc|pack:%d|%d>\r\n",getLen,packagelen);
				return MY_MQTT_ERR_GETDATA_FAIL;  // 后续这个需要定义
			}
		}
		else if(otherLen>0)  // 小于分片长度
		{
			pic_pack.data_length = otherLen;
			// 序列化包头
			picPack_offset=mypicPack_serialize(pic_buff, pic_pack);
			f_read(&fl_in,pic_buff+picPack_offset,otherLen,&getLen);
			if(getLen!=otherLen)
			{
				f_close(&fl_in); 
				printf("MY_MQTT_ERR_GETDATA_FAIL <rc|pack:%d|%d>\r\n",getLen,packagelen);
				return MY_MQTT_ERR_GETDATA_FAIL;  // 后续这个需要定义
			}
		}
		else //发送数据完毕，等待确认数据
		{	
			printf("等待响应...\r\n");
			while(!picdown_Flag_get())
			{
				time++;
				delay_ms(1);
				if(time>=timeOut)
				{
					reSendCount++;
					if(reSendCount>3)
					{
						printf("MY_MQTT_ERR_SEND_FAIL\r\n");
						return MY_MQTT_ERR_SEND_FAIL;
					}
					else
					{
						printf("MY_MQTT_ERR_ACK_TIMEOUT,retry:%d times\r\n",reSendCount);
						goto MQTT_RESEND;
					}
					//return MY_MQTT_ERR_ACK_TIMEOUT;
				}
			}

			// 成功发送一张图片
			f_close(&fl_in); 

			// 移动文件
			res = f_rename(file_path,file_path_d);
			if(res != FR_OK)
			{
				printf("Fail to rename file:%s\r\n%s\r\nERROR:%d\r\n", file_path, file_path_d, res);
				return MY_MQTT_ERR_DATA_ERR;
			}else
			{
				printf("rename file:%s\r\n%s\r\nERROR:%d\r\n", file_path, file_path_d, res);
			}

			printf("MY_MQTT_ERR_NONE，发送成功\r\n");
			return MY_MQTT_ERR_NONE;
		}	

		// 发送数据
		if(mqtt_publish_s(MY_TOPIC_PICUP,pic_buff,getLen+picPack_offset,0,1,0,mqttbuff,mqttbufflength)==0)
		{
			f_close(&fl_in); 
			printf("MY_MQTT_ERR_SEND_FAIL\r\n");
			return MY_MQTT_ERR_SEND_FAIL;
		}

		otherLen-=getLen;
		printf("otherLen:%d\r\n",otherLen);
		pic_pack.packet_id++;
	}
}

MY_MQTT_ERR mysend_picture(char* file_path,						// 图片路径
						 char* file_path_d,						// 图片路径(发送完成后)
						 int picture_id)						// 图片id						 					
{
	MY_MQTT_ERR res;
	u32 packagelen=10000;					// 图片分片长度
	u16 timeOut=50000;

	res = send_picture( MQTT_SEND_buf,MQTT_SEND_BUFF_MAXLENGTH,
						MQTT_DATA_buf,MQTT_DATA_BUFF_MAXLENGTH,
						file_path, file_path_d,
						picture_id, packagelen, timeOut);
	return res;		
}

MY_MQTT_ERR send_data(u8 *mqttbuff,u32 mqttbufflength, char* msg)						
{
	
	if(mqtt_publish_s(MY_TOPIC_MSGUP,msg,strlen(msg),0,1,0,mqttbuff,mqttbufflength)==0)
	{
		printf("MY_MQTT_ERR_SEND_FAIL\r\n");
		return MY_MQTT_ERR_SEND_FAIL;
	}
	return MY_MQTT_ERR_NONE;
}



