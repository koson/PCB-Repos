#ifndef __EC25_H__
#define __EC25_H__	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	   
//********************************************************************************
//修改说明
//V1.1 20140810
//1,新增__ec25dev结构体
//2,修改ec25_send_cmd函数,新增非预期结果返回
////////////////////////////////////////////////////////////////////////////////// 	
 
#define EC25_MAX_NEWMSG	10		//最大10条新消息

// 4G CTRL
#define DTR_4G   	PDout(13)	// 低电平唤醒模组
#define DISABLE_4G  PDout(5)	// 是否使用飞行模式
#define PRESET_4G   PDout(6)	// 重启模块，低电平时间>460ms

typedef enum 
{
	EC25_ERR_NONE=0,  //没有错误
	
	EC25_ERR_AT,  	//AT1
	
	EC25_ERR_CSQ,  //CSQ2
	
	EC25_ERR_CPIN,  //询问SIM卡3
	
	EC25_ERR_CREG,  //CS Service4
	
	EC25_ERR_CGREG,  //PS Service5
	
	EC25_ERR_CEREG,  //PS Service6
	
	EC25_ERR_QICSGP,  //配置PDP环境7
	
	EC25_ERR_QIOPEN,  //打开连接8
	EC25_ERR_DEACT,  //打开连接
}MY_EC25_ERR;

typedef struct 
{							  
 	u8 status;		//EC25状态
					//bit7:0,没有找到模块;1,找到模块了,csq
					//bit6:0,SIM卡不正常;1,SIM卡正常,cpin
					//bit5:0,未获得运营商名字;1,已获得运营商名字
					//bit4:0,中国移动;1,中国联通
					//bit3:0:保留
					//bit2:0:保留
					//bit1:0:保留
					//bit0:0:保留
	
	u8 mode;		//当前工作模式
					//0,号码输入模式/短信模式/数据模式
					//1,拨号中
					//2,通话中
					//3,来电响应中
	
	vu8 cmdon;		//标记是否有指令在发送等待状态
					//0,没有指令在等待回应
					//1,有指令在等待回应
	
	u8 csq;			//信号质量
	
	vu8 newmsg;		//新消息条数,0,没有新消息;其他,新消息条数
	u8 newmsgindex[EC25_MAX_NEWMSG];//新短信在SIM卡内的索引,最长记录EC25_MAX_NEWMSG条新短信
	u8 incallnum[20];//来电号码缓存区,最长20位
}__ec25dev; 

extern __ec25dev ec25dev;	//ec25控制器
  
#define swap16(x) (x&0XFF)<<8|(x&0XFF00)>>8		//高低字节交换宏定义

void ec25_Init(void);
void ec25_wakeup(int sw);
void ec25_reset(int ms);

u8* ec25_check_cmd(u8 *str);
u8 ec25_send_cmd(u8 *cmd,u8 *ack,u16 waittime);
u8 ec25_send_cmdEX(u8 *cmd,u8 *ack,u16 waittime);
void ec25_cmd_over(void);

u8 ec25_chr2hex(u8 chr);
u8 ec25_hex2chr(u8 hex);
u8 ec25_soft_start(void);
u8 ec25_dec(void);
void ec25_status_check(void);


u8 ec25_TCPConnect(u8 *ip,u16 port);
void ec25_TCPDisConnect(void);

u8 ec25_TCPSendData(u8 *data,u16 length);
u8 ec25_TCPSendData1024(u8 *data,u16 length);
	
void ec25_TCPReadData(u8 *data,u16 length);


#endif





