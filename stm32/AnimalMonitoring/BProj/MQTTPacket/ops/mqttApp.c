#include "mqttApp.h"
#include "mqttBase.h"
// fundamental
#include "usart.h"
#include "led.h"
#include "delay.h"
// communication
#include "usart3.h"
#include "ec25.h"
#include "UART_TCPbuff.h"
#include "MyFunction_C.h"
#include "queue.h"
// file system
#include "fattester.h"	
// mqtt 连接状态
static unsigned char mqtt_state = 0;
unsigned char mqtt_state_get()
{
	return mqtt_state;
}
void mqtt_state_set(unsigned char state)
{
	mqtt_state = state;
	printf("mqtt_state_set %d\r\n", state);
}
// ping状态
static unsigned char mqtt_ping_state = 0;
unsigned char mqtt_ping_state_get()
{
	return mqtt_ping_state;
}
void mqtt_ping_state_set(unsigned char state)
{
	mqtt_ping_state = state;
}
// 初始化队列
SqQueue Q_stage;   // 建立缓存区队列
SqQueue Q_resent;  // 建立重发区队列
QElemType frame_p; // 队列元素
/**
 * @description: serialize package 
 * @param buf 发送缓存区地址
 * @param pkg 数据包
 * @return: 序列化包长度
 */
uint16_t mypicPack_serialize(char *buf, struct pick_package pkg)
{
	uint32_t uid = 0;
	// 起始字节，固定为0xa596
	buf[0] = 0xa5;
	buf[1] = 0x96;

	uid = Frame_UID_get();
	buf[2] = uid >> 24;
	buf[3] = uid >> 16;
	buf[4] = uid >> 8;
	buf[5] = uid;

	// 图片序列，重新传输需要自动加1
	buf[6] = pkg.picture_id >> 8;
	buf[7] = pkg.picture_id;

	//一共分成多少包
	buf[8] = pkg.packet_total_num >> 8;
	buf[9] = pkg.packet_total_num;
	// 本包图片的ID
	buf[10] = pkg.packet_id >> 8;
	buf[11] = pkg.packet_id;
	// 本包数据长度
	buf[12] = pkg.data_length >> 8;
	buf[13] = pkg.data_length;
	// 图片数据长度
	buf[14] = pkg.data_total_length >> 24;
	buf[15] = pkg.data_total_length >> 16;
	buf[16] = pkg.data_total_length >> 8;
	buf[17] = pkg.data_total_length;
	//crc
	buf[18] = 0;
	buf[19] = 0;
	// 更新帧信息
	frame_p.uid = uid;
	frame_p.pic_id = pkg.picture_id;
	frame_p.pack_id = pkg.packet_id;
	return 20;
}
/**
 * @description: mqtt数据发送函数，包含重发机制
 * @param pTopic 主题
 * @param pMessage 待发消息
 * @param msglen 消息长度
 * @param buf mqtt数据缓存区
 * @param buflen mqtt缓存区长度
 * @return: 
 */
MY_MQTT_ERR mqtt_publish_data(char *pTopic, char *pMessage, uint16_t msglen)// 有效消息
{
	uint8_t i, j;
	// 重发变量
	uint8_t try_cnt = 0;	// 重连次数
	uint8_t perst_flag = 0; // reset flag
	uint16_t pack_crc = 0;  // CRC result

	try_cnt = 0;
	LED_GREEN_ON();
	// while ((mqtt_publish_s(pTopic, pMessage, msglen, 0, 1, 0, buf, buflen) == 0))
	while ((mqtt_publish(pTopic, pMessage, msglen) == 0))
	{
		if (try_cnt >= 3) // 网络异常，关闭网络和缓存
		{
			if (perst_flag == 0)
			{
				perst_flag = 1;
				try_cnt = 0;
				ec25_Restart();
				printf("ec25_reset ec25_reset ec25_reset!!!!");
			}
			else if (perst_flag == 1)
			{
				//f_close(&fl_in);
				// 保存中间变量
				// 保存中间变量
				printf("Network ERROR!!!!");
				F407USART1_SendString("mqtt_publish_s failed:MY_MQTT_ERR_SEND_FAIL\r\n");
				return MY_MQTT_ERR_SEND_FAIL;
			}
		}
		printf("mqtt_publish failed, try to reconnect...try_cnt=%d\r\n", try_cnt);
		// reconnect
		i = 0, j = 0;
		while (mqtt_connect() == 0) // 重新建立MQTT连接,重连过程会关闭网络，然后重新连接！
		{
			j++;
			printf("ccc%dccc", j);
			delay_ms(1000);
			if (j > 3)
			{
				i = 20;
				break;
			}
		}
		printf("\r\n");
		mqtt_state_set(0);
		while (mqtt_state_get() == 0) // 连接成功，等待10s
		{
			printf("...%d...", i);
			delay_ms(500);
			if (i++ >= 20) //等待10s
				break;
		}
		printf("\r\n");
		try_cnt++;
	}
	LED_GREEN_OFF();
}
/** 
 * @brief send pictures of assigned path(folder)
 * @param pic_buff 单个图片分包需要的buff空间
 * @param pic_bufflen 单个图片分包需要的buff空间长度
 * @param file_path 图片路径
 * @param picture_id 图片id
 * @param packagelen 图片分片长度
 * @param timeOut 等待picdonw响应，如果不需要可以注释
 * @return num of files
 * @note  max 20
 */
MY_MQTT_ERR send_picture(uint8_t *pic_buff, uint32_t pic_bufflen,
						 uint8_t *file_path,
						 uint32_t picture_id,
						 uint32_t packagelen,
						 uint16_t timeOut)
{
	OS_ERR err;
	uint8_t try_cnt = 0;	// 重连次数
	uint8_t perst_flag = 0; // reset flag
	uint16_t pack_crc = 0;  // CRC result
	// 包结构体
	struct pick_package pic_pack;
	// Queue status
	Status q_res;
	// MQTT status
	MY_MQTT_ERR mqtt_res;
	// 获取文件状态
	FIL fl_in;
	FRESULT res;
	uint32_t getLen;			 // 获取包长度
	uint32_t otherLen;			 // 剩余总长度
	uint16_t picPack_offset = 0; // package长度/偏移量
	// 打卡文件
	res = f_open(&fl_in, (const char *)file_path, FA_OPEN_EXISTING | FA_READ);
	if (res != FR_OK)
	{
		printf("MY_MQTT_ERR_GETDATA_FAIL <PATH:%s>\r\n", file_path);
		return MY_MQTT_ERR_GETDATA_FAIL;
	}
	// 每张图片唯一,后期根据读取的数据改变
	pic_pack.picture_id = picture_id;
	printf("#now sending picture_id=%d#\r\n", picture_id);
	// 读取文件大小
	pic_pack.data_total_length = f_size(&fl_in);
	// 计算传输次数
	pic_pack.packet_total_num = (pic_pack.data_total_length / packagelen) + 1;
	printf("f_open successfully <file_path:%s|file_size:%d|sub_num:%d|packagelen:%d>\r\n",
		   file_path, pic_pack.data_total_length, pic_pack.packet_total_num, packagelen);
	pic_pack.data_length = packagelen;
	// 处理
	f_lseek(&fl_in, 0); // 将目录移动到问价起始位置
	otherLen = pic_pack.data_total_length;
	pic_pack.packet_id = 0;

	while (1)
	{
		int i; //temp
		// 获取数据
		if (otherLen > packagelen) // 剩余数据大于图片分片长度
		{
			pic_pack.data_length = packagelen;
			// 序列化包头
			picPack_offset = mypicPack_serialize((char *)pic_buff, pic_pack);
			f_read(&fl_in, pic_buff + picPack_offset, packagelen, &getLen);
			if (getLen != packagelen)
			{
				f_close(&fl_in);
				printf("MY_MQTT_ERR_GETDATA_FAIL <rc|pack:%d|%d>\r\n", getLen, packagelen);
				return MY_MQTT_ERR_GETDATA_FAIL; // 后续这个需要定义
			}
			// add crc
			pack_crc = ModBusCRC(pic_buff + picPack_offset, getLen);
			// printf("%0X,picPack_offset=%d,packagelen=%d\r\n", pack_crc, picPack_offset, getLen);
			pic_buff[18] = pack_crc >> 8;
			pic_buff[19] = pack_crc;
		}
		else if (otherLen > 0) // 小于分片长度
		{
			pic_pack.data_length = otherLen;
			// 序列化包头
			picPack_offset = mypicPack_serialize((char *)pic_buff, pic_pack);
			f_read(&fl_in, pic_buff + picPack_offset, otherLen, &getLen);
			if (getLen != otherLen)
			{
				f_close(&fl_in);
				printf("MY_MQTT_ERR_GETDATA_FAIL <rc|pack:%d|%d>\r\n", getLen, packagelen);
				return MY_MQTT_ERR_GETDATA_FAIL; // 后续这个需要定义
			}
			// add crc
			pack_crc = ModBusCRC(pic_buff + picPack_offset, getLen);
			printf("%0X,picPack_offset=%d,packagelen=%d\r\n", pack_crc, picPack_offset, getLen);
			pic_buff[18] = pack_crc >> 8;
			pic_buff[19] = pack_crc;
		}
		else // 发送数据完毕，等待确认数据
		{
			f_close(&fl_in);
			F407USART1_SendString("PICTURE MY_MQTT_ERR_NONE，发送成功\r\n");
			return MY_MQTT_ERR_NONE;
		}
		// 发送前先清空缓存区
		UART_TCPbuff_Run(F407USART3_buffRead);
		// 发送数据
		mqtt_res = mqtt_publish_data(MY_TOPIC_PICUP, (char *)pic_buff, getLen + picPack_offset);
		if (mqtt_res == MY_MQTT_ERR_SEND_FAIL)
		{
			f_close(&fl_in);
			return MY_MQTT_ERR_SEND_FAIL;
		}
		// 写入队列
		if (otherLen - getLen > packagelen)
		{
			q_res = EnQueue(&Q_stage, frame_p);
			if (q_res) // 入队列成功
			{
				printf("+ Q_stage:[%3d,%3d,%3d],Q+UID:%08X,pid:%04X\r\n", Q_stage.front, Q_stage.rear, QueueLength(Q_stage), frame_p.uid, frame_p.pack_id);
				printf("* Q_resnt:[%3d,%3d,%3d],Q*UID:%08X,pid:%04X\r\n", Q_resent.front, Q_resent.rear, QueueLength(Q_resent), Q_resent.data[Q_resent.front].uid, Q_resent.data[Q_resent.front].pack_id);
			}
			else // 队列满，则代表服务器异常，因为每一张图片结束我会check队列！
			{
				printf("Server error\r\n");
				return MY_MQTT_ERR_ACK_TIMEOUT;
			}
		}
		otherLen -= getLen;
		printf("otherLen:%d,crc=%0X\r\n", otherLen, pack_crc);
		pic_pack.packet_id++;
	}
	return 0;
}
/**
 * @description: 获取重发图片的名字
 * @param d_path 文件夹路径
 * @param file_path 文件路径
 * @param pic_id 图片id
 * @return: 
 */
void get_resent_path(uint8_t *d_path, uint8_t *file_path, uint16_t pic_id)
{
	uint8_t fn_t[30] = {0};

	file_path[0] = 0;
	strcat((char *)file_path, d_path); // 复制原始源文件路径
	strcat((char *)file_path, "/");	// 源路径加斜杠
	sprintf((char *)fn_t, "IMAG%04d.JPG", pic_id);
	strcat((char *)file_path, fn_t); // 目标路径加文件名
	printf("get_resent_path file name: %s\r\n", file_path);
}
/**
 * @description: 
 * @param pic_buff 图片缓存区
 * @param pic_bufflen 图片缓存区长度
 * @param packagelen mqtt包长度
 * @return: 0  发送成功，>0 发送事变
 */
MY_MQTT_ERR check_Queue(uint8_t *pic_buff, uint32_t pic_bufflen,	// 单个图片分包需要的buff空间以及buff实际长度
						uint32_t packagelen)					  // 图片分片长度
{
	uint16_t i;
	QElemType elem;
	// 重发变量
	//	uint8_t try_cnt = 0;  // 重连次数
	//	uint8_t perst_flag = 0;  // reset flag
	uint16_t pack_crc = 0; // CRC result

	// 包结构体
	struct pick_package pic_pack;

	// Queue status
	Status q_res;
	// MQTT status
	MY_MQTT_ERR mqtt_res;
	// 获取文件状态
	FIL fl_in;
	FRESULT res;
	uint32_t getLen;			 // 获取包长度
	uint32_t otherLen;			 // 剩余总长度
	uint16_t picPack_offset = 0; // package长度/偏移量
	uint8_t file_path[40] = {0};
	// 获取重发队列第一个元素
	GetHead(Q_resent, &elem);
	i = Q_resent.front;
	while (i != Q_resent.rear)
	{
#if (EN_log_print >= 2)
		printf("\r\n#####check_Queue####\r\n");
#endif // EN_log_print \
	// 如果是图片包
		if (elem.pic_id != 0)
		{
			get_resent_path((uint8_t *)"0:ARCH", file_path, elem.pic_id); //获取文件的地址
			file_path[19] = 0;
			// printf("resent picture path:%s \r\n", file_path);
			res = f_open(&fl_in, (const char *)file_path, FA_OPEN_EXISTING | FA_READ);
			if (res != FR_OK)
			{
				get_resent_path((uint8_t *)"0:INBOX", file_path, elem.pic_id); //获取文件的地址
				file_path[20] = 0;
				// printf("resent picture path:%s \r\n", file_path);
				res = f_open(&fl_in, (const char *)file_path, FA_OPEN_EXISTING | FA_READ);
				if (res != FR_OK)
				{
					printf("MY_MQTT_ERR_GETDATA_FAIL <PATH:%s>\r\n", file_path);
					f_close(&fl_in);
					return MY_MQTT_ERR_GETDATA_FAIL;
				}
			}
#if (EN_log_print >= 2)
			printf("resent picture path:%s \r\n", file_path);
#endif // EN_log_print
		RESENT:
			// 每张图片唯一,后期根据读取的数据改变
			pic_pack.picture_id = elem.pic_id;
			// 读取文件大小
			pic_pack.data_total_length = f_size(&fl_in);
			// 计算传输次数
			// pic_pack.packet_total_num = (pic_pack.data_total_length / packagelen) + 1;
			pic_pack.packet_total_num = 0XFFFF; // 重发，按单次算
			//	printf("f_open successfully <file_path:%s|file_size:%d|sub_num:%d|packagelen:%d>\r\n",
			//		   file_path, pic_pack.data_total_length, pic_pack.packet_total_num, packagelen);
			pic_pack.packet_id = elem.pack_id;
			if ((elem.pack_id + 1) * packagelen < pic_pack.data_total_length)
				pic_pack.data_length = packagelen;
			else
				pic_pack.data_length = pic_pack.data_total_length % packagelen;
			otherLen = pic_pack.data_length;
			// 处理
			picPack_offset = mypicPack_serialize((char *)pic_buff, pic_pack);
			// 序列化包头
			res = f_lseek(&fl_in, packagelen * (elem.pack_id)); // 将目录移动到问价起始位置,pack_id是从零开始的。
			if (res != FR_OK)
			{
				printf("f_lseek ERROR f_lseek ERROR f_lseek ERROR\r\n");
			}
#if (EN_log_print >= 2)
			printf("f_lseek:%d|%d\r\n", packagelen * (elem.pack_id), pic_pack.data_total_length);
#endif // EN_log_print
			f_read(&fl_in, pic_buff + picPack_offset, otherLen, &getLen);
			if (getLen != otherLen)
			{
				f_close(&fl_in);
				//myfree(SRAMIN, file_path);
				printf("MY_MQTT_ERR_GETDATA_FAIL <rc|pack:%d|%d>\r\n", getLen, packagelen);
				return MY_MQTT_ERR_GETDATA_FAIL; // 后续这个需要定义
			}
			// add crc
			pack_crc = ModBusCRC(pic_buff + picPack_offset, getLen);
#if (EN_log_print >= 2)
			printf("%0X,picPack_offset=%d,packagelen=%d\r\n", pack_crc, picPack_offset, getLen);
#endif // EN_log_print
			pic_buff[18] = pack_crc >> 8;
			pic_buff[19] = pack_crc;
			// 发送前先清空缓存区
			UART_TCPbuff_Run(F407USART3_buffRead);
			// 发送数据
			//mqtt_publish_data(MY_TOPIC_PICUP, mqttbuff, mqttbufflength, pic_buff, getLen + picPack_offset);
			mqtt_res = mqtt_publish_data(MY_TOPIC_PICUP, (char *)pic_buff, getLen + picPack_offset);
			if (mqtt_res == MY_MQTT_ERR_SEND_FAIL)
			{
				f_close(&fl_in);
				printf("%0X,picPack_offset=%d,packagelen=%d\r\n", pack_crc, picPack_offset, getLen);
				return MY_MQTT_ERR_SEND_FAIL;
			}
			// 写入队列
			DeQueue(&Q_resent, &elem);
			printf("*-Q_resnt:[%3d,%3d,%3d],Q-UID:%08X,pid:%04X\r\n", Q_resent.front, Q_resent.rear, QueueLength(Q_resent), Q_resent.data[Q_resent.front].uid, Q_resent.data[Q_resent.front].pack_id);
			//printf("---Q_resent:[%d,%d],Qin len=%d\r\n",Q_resent.front,Q_resent.rear,QueueLength(Q_resent));
			q_res = EnQueue(&Q_stage, frame_p);
			if (q_res) // 入队列成功
			{
				printf("*+Q_stage:[%3d,%3d,%3d],Q+UID:%08X,pid:%04X\r\n", Q_stage.front, Q_stage.rear, QueueLength(Q_stage), frame_p.uid, frame_p.pack_id);
			}
			else // 队列满，则代表服务器异常，因为每一张图片结束我会check队列！
			{
				f_close(&fl_in);
				printf("Server error\r\n");
				return MY_MQTT_ERR_ACK_TIMEOUT;
			}
			q_res = GetHead(Q_resent, &elem);
			if (q_res == SUCCESS && (elem.pic_id == pic_pack.picture_id))
			{
				printf("send  data\r\n");
				goto RESENT;
			}
			else
			{
				printf("close picture\r\n");
				f_close(&fl_in);
				break;
			}
		}
		// 重发传感器消息
		else
		{
			DeQueue(&Q_resent, &elem);
			printf("*-Q_resnt:[%3d,%3d,%3d],Q-UID:%08X,pid:%04X\r\n", Q_resent.front, Q_resent.rear, QueueLength(Q_resent), Q_resent.data[Q_resent.front].uid, Q_resent.data[Q_resent.front].pack_id);
			GetHead(Q_resent, &elem);
		}
		i = (i + 1) % MAXSIZE;
	}
	return 0;
}
MY_MQTT_ERR mycheck_Queue(void)
{
	uint32_t packagelen = PACKAGE_LEN; // 图片分片长度
	check_Queue(MQTT_DATA_buf, MQTT_DATA_BUFF_MAXLENGTH,
				packagelen);
}
MY_MQTT_ERR mysend_picture(uint8_t *file_path, uint32_t picture_id)
{
	MY_MQTT_ERR res;
	uint32_t packagelen = PACKAGE_LEN; // 图片分片长度
	uint16_t timeOut = 50000;
	res = send_picture(MQTT_DATA_buf, MQTT_DATA_BUFF_MAXLENGTH,
					   file_path, picture_id, packagelen, timeOut);
	mycheck_Queue();
	return res;
}

/**
 * @description: 序列换传感器数据包头
 * @param buf 缓存区地址
 * @param msg_len 消息长度
 * @param crc crc校验值
 * @return: 
 */
int mymes_serialize(char *buf, uint16_t msg_len, uint16_t crc)
{

	uint32_t uid = 0;
	// 起始字节，固定为0xa590
	buf[0] = 0xa5;
	buf[1] = 0x96;

	uid = Frame_UID_get();
	buf[2] = uid >> 24;
	buf[3] = uid >> 16;
	buf[4] = uid >> 8;
	buf[5] = uid;

	buf[6] = msg_len >> 8;
	buf[7] = msg_len;

	buf[8] = crc >> 8;
	buf[9] = crc;
	buf[10] = 0;

	frame_p.uid = uid;
	frame_p.pic_id = 0;
	frame_p.pack_id = 0;
	return 10;
}
/**
 * @description: 发送数据
 * @param msg 数据地址
 * @return: 0 发送成功，>0 发送失败
 */
MY_MQTT_ERR mysend_data(char *msg)
{
	uint16_t len = 0;
	uint16_t crc = 0;
	uint16_t offset = 0;
	// 扫描缓存区是否存在内容
	UART_TCPbuff_Run(F407USART3_buffRead);
	// 打包
	len = strlen(msg);
	crc = ModBusCRC((uint8_t *)msg, len);
	offset = mymes_serialize(MQTT_DATA_buf, len, crc);
	strcat(MQTT_DATA_buf, msg);
	// printf("%d,%d####\r\n",len+offset,strlen(MQTT_DATA_buf));
	// 发送数据
	mqtt_publish_data(MY_TOPIC_MSGUP, MQTT_DATA_buf, len+offset);
	// 写入队列
	EnQueue(&Q_stage, frame_p);
	printf("+ Q_stage:[%3d,%3d,%3d],QOUID:%08X,pid:%04X\r\n", Q_stage.front, Q_stage.rear, QueueLength(Q_stage), frame_p.uid, frame_p.pack_id);
	printf("* Q_resnt:[%3d,%3d,%3d],QOUID:%08X,pid:%04X\r\n", Q_resent.front, Q_resent.rear, QueueLength(Q_resent), Q_resent.data[Q_resent.front].uid, Q_resent.data[Q_resent.front].pack_id);
	return 0;
}
