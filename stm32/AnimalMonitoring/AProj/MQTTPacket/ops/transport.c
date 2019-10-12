#include "transport.h"  // from:ec25 to:mqtt
#include "UART_TCPbuff.h"  // from:uart3 to:ec25
#include "ec25.h"  // from:UART_TCPbuff to:transport
#include "mqtt.h"  // from:transport to:
/**
  * @brief  通过TCP方式发送数据到TCP服务器
  * @param  buf 数据首地址
  * @param  buflen 数据长度
  * @retval 小于0表示发送失败
 */
int transport_sendPacketBuffer(unsigned char* buf, int buflen)
{
	u16 len;
	//len=SIM7100_TCPSendData_T(buf,buflen);
	len=ec25_TCPSendData(buf,buflen);
	return len;
}


int transport_getdata(unsigned char* buf, int count)
{

	return UART_TCP_buffReads(buf,count);
}

/**
  * @brief  建立一个TCP的连接
  * @param  无
  * @retval 小于0表示打开失败
  */
int transport_open(void)
{
	return ec25_TCPConnect(MQTT_SEERVER_IP,MQTT_SEERVER_PORT);
}

/**
  * 关闭连接 
  * @brief  关闭socket
  * @param  无
  * @retval 小于等于0表示关闭失败
  */
int transport_close(void)
{
	ec25_TCPDisConnect();
	return 1;
}
