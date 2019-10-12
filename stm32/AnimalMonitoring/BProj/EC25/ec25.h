#ifndef _EC25_H_
#define _EC25_H_
#include "sys.h"
#define In_Condition_HCTV 1

typedef enum
{
    EC25_ERR_NONE = 0,  // 无错误
    EC25_ERR_NOSIMCARD, // 缺少SD卡
    EC25_ERR_CREG,      // failed to register on CS domain service
    EC25_ERR_CGREG,     // failed to register on PS service
    EC25_ERR_QIACT,     // 打开PDP环境失败
    EC25_ERR_QIOPEN,    // 打开TCP链接失败
    EC25_ERR_QICLOSE,   // 关闭TCP链接失败
    EC25_ERR_CPIN,      // 打开SIM卡失败
	EC25_ERR_SEND,		// 发送数据失败
	EC25_ERR_CACHE_FULL // 缓存区满
} EC25_ERR;

#define UART_SEND_PACK_LENGTH_MAX 1460
void ec25_reset(int ms);
void ec25_DTR_Sleep(void);
void ec25_DTR_Wakeup(void);
void ec25_Turn_off(void);
void ec25_Restart(void);
// 4G CTRL IO
#define EC24_DTR PDout(13)      // Sleep mode control, active low.
#define EC25_W_DISABLE PDout(7) // Disable wireless commuunications, pull-up by default, active low.
#define EC25_PERST PDout(14)    // a low level voltage with the frame of150~460ms, 300ms recommand
// Local use
EC25_ERR ec25_tcp_Conditon_Normal(void);
uint32_t ec25_TCPSendData(char *data, uint16_t length);
uint16_t ec25_TCPSendDataPack(char *data, uint16_t length);
// uint32_t ec25_TCPSendData(uint8_t *data, uint16_t length);
// uint16_t ec25_TCPSendDataPack(uint8_t *data, uint16_t length);
// For mqtt
// void SIM7100_TCPReadData(uint8_t *data, uint16_t length);
// uint8_t SIM7100_TCPConnect(uint8_t *ip, uint16_t port);
// uint8_t ec25_TCPDisConnect(void);

// Global use
EC25_ERR ec25_Init(void);
EC25_ERR ec25_Act_PDP_Condition(void);
EC25_ERR ec25_TCPConnect(char *ip, uint16_t port);
uint8_t ec25_TCPDisConnect(void);
#endif
