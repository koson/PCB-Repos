#include "mqttTransport.h"
#include "mqttBase.h"
#include "UART_TCPbuff.h"
#include "ec25.h"
/**
 * @description: 发送指定长度的字节
 * @param buf 数据地址
 * @param buflen 数据长度
 * @return: 发送数据长度 <=0 表示失败
 */
int transport_sendPacketBuffer(char* buf, u32 buflen)
{
	return ec25_TCPSendData(buf, buflen);
}
/**
 * @description: 从制定地址获取指定长度的字符
 * @param buf 数据地址
 * @param buflen 数据长度
 * @return: 获取数据数量
 */
int transport_getdata(char* buf, u32 count)
{
	return UART_TCP_buffReads(buf,count);
}
/**
  * @description: 建立一个TCP的连接
  * @param  none
  * @retval <=0表示打开失败
  */
int transport_open(void)
{
	return !ec25_TCPConnect(MQTT_SEERVER_IP,MQTT_SEERVER_PORT);
}

/**
  * @description: 关闭socket
  * @param  无
  * @retval <=0表示关闭失败
  */
int transport_close(void)
{
	return ec25_TCPDisConnect();
}
