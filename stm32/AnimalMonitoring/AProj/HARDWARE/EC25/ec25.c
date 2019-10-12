#include "ec25.h"  // self
#include "delay.h"	
#include "led.h"     
#include "string.h" 
#include "usart.h" 
#include "usart3.h" 
#include "UART_TCPbuff.h" 
#include "malloc.h"

//////////////////////////////////////////////////////////////////////////////////	   
//********************************************************************************
//修改说明
//1,新增__ec25dev结构体
//2,修改ec25_send_cmd函数,新增非预期结果返回
////////////////////////////////////////////////////////////////////////////////// 	
 
__ec25dev ec25dev;	//ec25控制器

void ec25_Init(void)
{    	 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//使能GPIOF时钟

	//GPIOD13,GPIOD6,GPIOD5初始化设置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化

	GPIO_SetBits(GPIOD,GPIO_Pin_5 );  // GPIOD5(W_DISABLE#) 高，启动网络；
	GPIO_ResetBits(GPIOD,GPIO_Pin_6);  // GPIOD6(PEREST) 高，不重启,由于reset加入了一个三极管电路
	GPIO_ResetBits(GPIOD,GPIO_Pin_13);  // GPIOD13(DTR)设置为低，启动模块
}

//#define DTR_4G   	PDout(13)	// 低电平唤醒模组
//#define DISABLE_4G  PDout(5)	// 是否使用飞行模式,低电平有效
//#define PRESET_4G   PDout(6)	// 重启模块，低电平时间>460ms

void ec25_wakeup(int sw)
{
	printf("wakeup_modle\r\n");
	if(sw)  DTR_4G = 0; // 关闭
	else 	DTR_4G = 1; 
}

void ec25_reset(int ms)
{
	printf("reset_modle\r\n");
	PRESET_4G = 1;
	delay_ms(ms);
	PRESET_4G = 0;
}

// ec25发送命令后,检测接收到的应答
// str:期待的应答结果
// 返回值:0,没有得到期待的应答结果
//    其他,期待应答结果的位置(str的位置)
//u8* ec25_check_cmd(u8 *str)
//{
//	char *strx=0;
//	if(USART3_RX_STA&0X8000)		//接收到一次数据了
//	{ 
//		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//添加结束符
//		//usart debug
//		printf("*dubug-cmd-rev*%s\r\n",USART3_RX_BUF);
//		
//		strx=strstr((const char*)USART3_RX_BUF,(const char*)str);
//	} 
//	return (u8*)strx;
//}

////向ec25发送命令
////cmd:发送的命令字符串(不需要添加回车了),当cmd<0XFF的时候,发送数字(比如发送0X1A),大于的时候发送字符串.
////ack:期待的应答结果,如果为空,则表示不需要等待应答
////waittime:等待时间(单位:10ms)
////返回值:0,发送成功(得到了期待的应答结果)
////       1,收到非预期结果
////       2,没收到任何回复
//u8 ec25_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
//{
//	u8 i=0;
//	u8 res=0;  
//	USART3_RX_STA=0;
//	ec25dev.cmdon=1;//进入指令等待状态
//	if((u32)cmd<=0XFF)
//	{   
//		while((USART3->SR&0X40)==0);//等待上一次数据发送完成  
//		USART3->DR=(u32)cmd;
//	}else 
//	{
//		//u3_printf("%s\r\n",cmd);//发送命令
//		while(cmd[i])
//		{
//			U3_Send_Byte(cmd[i]);
//			i++;
//		}
//		U3_Send_Byte('\r');
//		U3_Send_Byte('\n');
//	}
//	if(ack&&waittime)		//需要等待应答
//	{
//		while(--waittime)	//等待倒计时
//		{
//			delay_ms(10);
//			if(USART3_RX_STA&0X8000)//是否接收到期待的应答结果
//			{
//				if(ec25_check_cmd(ack))res=0;//收到期待的结果了
//				else res=1;//不是期待的结果
//				break; 
//			} 
//		}
//		if(waittime==0)res=2; 
//	}
//	return res;
//}

//u8 ec25_send_cmdEX(u8 *cmd,u8 *ack,u16 waittime)
//{
//	u8 i=0;
//	u8 res=0;  
//	USART3_RX_STA=0;
//	ec25dev.cmdon=1;//进入指令等待状态
//	if((u32)cmd<=0XFF)
//	{   
//		while((USART3->SR&0X40)==0);//等待上一次数据发送完成  
//		USART3->DR=(u32)cmd;
//	}else 
//	{
//		while(cmd[i])
//		{
//			U3_Send_Byte(cmd[i]);
//			i++;
//		}
//		U3_Send_Byte('\r');
//		U3_Send_Byte('\n');
//	}
//	if(ack&&waittime)		//需要等待应答
//	{
//		while(--waittime)	//等待倒计时
//		{
//			delay_ms(10);
//			if(USART3_RX_STA&0X8000)//是否接收到期待的应答结果
//			{
//				if(ec25_check_cmd(ack))res=0;//收到期待的结果了
//				else res=1;//不是期待的结果
//				break; 
//			} 
//		}
//		if(waittime==0)res=2; 
//	}
//	return res;
//}


////命令处理完时调用,与ec25_send_cmd成对使用/多个与ec25_send_cmd后调用.
//void ec25_cmd_over(void)
//{
//	USART3_RX_STA=0;
//	ec25dev.cmdon=0;//退出指令等待状态
//}

////将1个字符转换为16进制数字
////chr:字符,0~9/A~F/a~F
////返回值:chr对应的16进制数值
//u8 ec25_chr2hex(u8 chr)
//{
//	if(chr>='0'&&chr<='9')return chr-'0';
//	if(chr>='A'&&chr<='F')return (chr-'A'+10);
//	if(chr>='a'&&chr<='f')return (chr-'a'+10); 
//	return 0;
//}

////将1个16进制数字转换为字符
////hex:16进制数字,0~15;
////返回值:字符
//u8 ec25_hex2chr(u8 hex)
//{
//	if(hex<=9)return hex+'0';
//	if(hex>=10&&hex<=15)return (hex-10+'A'); 
//	return '0';
//}


// 
/**
  * @brief  wait for OK flag, unit:ms
  * @param  timeout wait time
  *
  * @retval 
  * 	-<em>1</em> success
  * 	-<em>0</em> fail
 */
u8 WaitString_OKflag(u16 timeout)
{
	static u16 time=0;
	while(!waitStringOK_get())
	{
		delay_ms(1);
		time+=1;
		if(time>=timeout)
		{
			time=0;
			return 0;
		}
	}
	time=0;
	return 1;
}

u8 ec25_init(void)
{
	u8 *p1; 
	u8 cnt;
	u8 i;
	ec25_wakeup(1);   // DRT
	ec25_reset(460);  // reset
	
	
	cnt=0;
	//AT
	for(i=0;i<10;i++)
	{
		waitString_set("OK",2);
		F407USART3_SendString("AT\r\n");
		F407USART1_SendString("AT\r\n");
		if(WaitString_OKflag(2000))  // wait 2s, if success, then break.
			break;
	}  // wait 20s
	
	waitString_set("OK",2);
	F407USART3_SendString("AT+CPIN?\r\n");
	F407USART1_SendString("AT+CPIN?\r\n");
	WaitString_OKflag(3000);
	
	// set APN
	waitString_set("OK",2);
	F407USART3_SendString("AT+QICSGP=1,1,\"HVTV\",\"\",\"\",3\r\n");
	F407USART1_SendString("AT+QICSGP=1,1,\"HCTV\",\"\",\"\",3\r\n");
	WaitString_OKflag(3000);
	
	delay_ms(1000);
	
	// reset
	waitString_set("OK",2);
	F407USART3_SendString("AT+CFUN=0\r\n");
	F407USART1_SendString("AT+CFUN=0\r\n");
	WaitString_OKflag(3000);	
	delay_ms(500);
	waitString_set("OK",2);
	F407USART3_SendString("AT+CFUN=1\r\n");
	F407USART1_SendString("AT+CFUN=1\r\n");
	WaitString_OKflag(3000);
	
	delay_ms(1000);
	
	for(i=0;i<5;i++)
	{
		waitString_set("+CREG: 0,1",10);
		F407USART3_SendString("AT+CREG?\r\n");
		F407USART1_SendString("AT+CREG?\r\n");
		if(WaitString_OKflag(3000))  //
			break;
	}
		
	waitString_set("+CSQ",2);
	F407USART3_SendString("AT+CSQ\r\n");
	F407USART1_SendString("AT+CSQ\r\n");
	WaitString_OKflag(3000);
}

u8 ec25_TCPConnect(u8 *ip,u16 port)
{
	char *ptr = mymalloc(SRAMIN,50);  // 申请50个字节的内存
	
	waitString_set("OK",2);
	//关闭网络，否则可能会打开失败
	F407USART3_SendString("AT+QICLOSE=0\r\n");
	F407USART1_SendString("AT+QICLOSE=0\r\n");
	WaitString_OKflag(3000);
	
	delay_ms(500);
	
	waitString_set("OK:",2);
	F407USART3_SendString("AT+QIDEACT=1\r\n");
	F407USART1_SendString("AT+QIDEACT=1\r\n");
	WaitString_OKflag(3000);
	
	delay_ms(1000);
	
	waitString_set("OK:",2);
	//打开网络
	F407USART3_SendString("AT+QIACT=1\r\n");
	F407USART1_SendString("AT+QIACT=1\r\n");
	WaitString_OKflag(50000);

	delay_ms(1000);
	
	waitString_set("+QIOPEN: 0,0",12);//命令模式等待该响应
	//打开tcp连接
	sprintf(ptr,"AT+QIOPEN=1,0,\"TCP\",\"%s\",%d,0,1\r\n",ip,port);
	F407USART3_SendString(ptr);
	F407USART1_SendString(ptr);
	//AT+QIOPEN=1,0,"TCP","47.106.99.188",10020,0,1
	//AT+QIOPEN=1,0,"TCP","112.74.59.250",10002,0,2
	//sprintf(ptr,"%s","AT+QIOPEN=1,0,\"TCP\",\"47.106.99.188\",10020,0,1");
	//sprintf(ptr,"%s",);
	//sprintf(ptr,"AT+QIOPEN=1,0,\"TCP\",\"%s\",%d,0,0",ip,port);
	//F407USART3_SendString("AT+QIOPEN=1,0,\"TCP\",\"112.74.59.250\",10002,0,1\r\n");
	//F407USART1_SendString("AT+QIOPEN=1,0,\"TCP\",\"112.74.59.250\",10002,0,1\r\n");
	WaitString_OKflag(10000);
	
	F407USART1_SendString("ec25_TCPConnect\r\n");


	myfree(SRAMIN,ptr);  // 释放内存
	
	return 1;
}

void ec25_TCPDisConnect(void)
{
	waitString_set("OK",2);
	//关闭网络，否则可能会打开失败
	F407USART3_SendString("AT+QICLOSE=0\r\n");
	F407USART1_SendString("AT+QICLOSE=0\r\n");
	WaitString_OKflag(3000);
	
	delay_ms(500);
	
	waitString_set("OK:",2);
	F407USART3_SendString("AT+QIDEACT=1\r\n");
	F407USART1_SendString("AT+QIDEACT=1\r\n");
	WaitString_OKflag(3000);
}

u8 ec25_TCPSendData(u8 *data,u16 length)
{
	u8 state=0;
	u16 len=length,index=0;
	#define UART_SEND_PACK_LENGTH_MAX 1500
	while(len>UART_SEND_PACK_LENGTH_MAX)
	{
		state=ec25_TCPSendData1024(data+index,UART_SEND_PACK_LENGTH_MAX);
		if(state==0)
		{
			return 0;
		}
		len-=UART_SEND_PACK_LENGTH_MAX;
		index+=UART_SEND_PACK_LENGTH_MAX;
	}
	if(len>0)
	{
		state=ec25_TCPSendData1024(data+index,len);
		if(state==0)
		{
			return 0;
		}		
	}
	return state;
}

u8 ec25_TCPSendData1024(u8 *data,u16 length)
{
	u8 state;
	char *ptr = mymalloc(SRAMIN,50);  // 申请50个字节的内存
	
	waitString_set("\r\n>",3);
	sprintf(ptr,"AT+QISEND=0,%d\r\n",length);
	F407USART3_SendString(ptr);
	//F407USART1_SendString(ptr);
	myfree(SRAMIN,ptr);  // 释放内存
	//printf("AT+CIPSEND=0,%d\r\n",length);
	state=WaitString_OKflag(5000);
	if(state==0)
	{
		F407USART1_SendString("SIM7100_TCPSendData1024 ERROR-1");
		return 0;
	}		
	//
	waitString_set("SEND OK",7);
	F407USART3_SendBytes(data,length);
	state=WaitString_OKflag(10000);	
	if(state==0)
	{
		F407USART1_SendString("SIM7100_TCPSendData1024 ERROR-2");
		return 0;
	}	
	return 1;
}

void ec25_TCPReadData(u8 *data,u16 length)
{
	// =UART_TCPbuff_Run
}

//u8 ec25_soft_start(void)
//{
//	u8 *p1; 
//	u8 cnt;
//	//DRT
//	ec25_wakeup(1);
//	ec25_reset(460);
//	//reset
//	//AT
//	cnt=0;
//	while(ec25_send_cmd("AT","OK",100))  // 0检测是否应答AT指令 
//	{
//		delay_ms(600);
//		printf("(1/9)wait ec25 response,retrt times:%d\r\n",cnt);
//		delay_ms(400);  
//		if(cnt++>15) return EC25_ERR_AT;
//	} 
//	printf("(1/9)AT? +OK\r\n");	
//	cnt=0;
//	while(ec25_send_cmd("AT+CPIN?","+CPIN: READY",100))  // 1检测SIM卡
//	{
//		delay_ms(500);
//		printf("(2/9)AT+CPIN? %d\r\n",cnt);
//		delay_ms(500);  
//		if(cnt++>20) 
//			return EC25_ERR_CPIN; 
//	}
//	printf("(2/9)AT+CPIN? +CPIN: READY\r\n");	
//	cnt=0;
//	while(ec25_send_cmd("AT+CREG?","+CREG: 0,1",100))  // 2 CS Service
//	{
//		delay_ms(500);
//		printf("(3/9)AT+CREG? %d\r\n",cnt);
//		delay_ms(500);  
//		if(cnt++>5) return EC25_ERR_CREG; 
//	}

//	if(ec25_send_cmd("AT+QICSGP=1,1,\"hctv\",\"\",\"\",3","OK",500)) {
//		printf("(7/9)!fail connet to hctv\r\n");
//		return EC25_ERR_QICSGP;	
//	}
//	else
//		printf("(7/9)connet to hctv\r\n");
//	delay_ms(500);
//	
//	printf("(3/9)AT+CREG? +CREG: 0,1\r\n");	
//	cnt=0;
//	while(ec25_send_cmd("AT+CGREG?","+CGREG: 0,1",100))  // 3 PS Service
//	{
//		delay_ms(500);
//		printf("(4/9)AT+CGREG? %d\r\n",cnt);
//		delay_ms(500);  
//		if(cnt++>5) return EC25_ERR_CGREG; 
//	}
//	printf("(4/9)AT+CGREG? +CGREG: 0,1\r\n");		
//	cnt=0;
//	while(ec25_send_cmd("AT+CEREG?","+CEREG: 0,1",100))  // 4 PS Service
//	{
//		delay_ms(500);
//		printf("(5/9)AT+CEREG? %d\r\n",cnt);
//		delay_ms(500);  
//		if(cnt++>5) return EC25_ERR_CEREG; 
//	} 
//	printf("(5/9)AT+CEREG? +CEREG: 0,1\r\n");	
//	
//	delay_ms(5000);
//	if(ec25_send_cmd("AT+CSQ","OK",200)==0)//查询信号质量,顺便检测GSM模块状态
//	{
//		p1=(u8*)strstr((const char*)(USART3_RX_BUF),":"); 
//		p1+=2;
//		ec25dev.csq=(p1[0]-'0')*10+p1[1]-'0';//信号质量
//		printf("(6/9)AT+CSQ: %d\r\n",ec25dev.csq);
//		//printf("p1:%c,p2:%c,ps:5s\r\n",p1[0],p1[1],USART3_RX_BUF);
//		if(ec25dev.csq>30) 
//			ec25dev.csq=30;		
//		ec25dev.status|=1<<7;	//查询GSM模块是否在位?
//	}else 
//	{ 
//		ec25dev.csq=0;	
//		ec25dev.status=0;	//重新查找
//		printf("(6/9)fail AT+CSQ: %d\r\n",ec25dev.csq);
//		return EC25_ERR_CSQ;
//	}


//	// Activate a PDP Context
//	while(ec25_send_cmd("AT+CFUN=0","OK",100))  // 4 PS Service
//	{
//		delay_ms(500);
//		printf("AT+CFUN=0");
//		delay_ms(500);  
//		if(cnt++>5) return EC25_ERR_CEREG; 
//	} 
//	while(ec25_send_cmd("AT+CFUN=1","OK",100))  // 4 PS Service
//	{
//		delay_ms(500);
//		printf("AT+CFUN=1");
//		delay_ms(500);  
//		if(cnt++>5) return EC25_ERR_CEREG; 
//	} 
//	if(ec25_send_cmd("AT+QIACT=1","OK",500)) {
//		printf("(8/9)!fail Activate a PDP Context\r\n");
//		return EC25_ERR_QICSGP;	
//	}
//	else
//		printf("(8/9)Activate a PDP Context\r\n");
//	delay_ms(500);
//	
//	if(ec25_send_cmd("AT+QIOPEN=1,0,\"TCP\",\"47.106.99.188\",10808,0,2","CONNECT",1000)) return EC25_ERR_QIOPEN;
//	//if(ec25_send_cmd("AT+QIOPEN=1,0,\"TCP\",\"210.77.68.221\",10020,0,2","CONNECT",1000)) return EC25_ERR_QIOPEN;
//	
//	// tongxunmao TCP:112.74.59.250:10002
//	//if(ec25_send_cmd("AT+QIOPEN=1,0,\"TCP\",\"112.74.59.250\",10002,0,2","CONNECT",1000)){  //beijing 

//	
////	delay_ms(2000);
////	U3_Send_Date("123456789\r\n",12);
////	delay_ms(2000);
////	U3_Send_Date("123456789\r\n",12);
////	delay_ms(2000);
////	U3_Send_Date("123456789\r\n",12);
////	delay_ms(2000);
////	U3_Send_Date("123456789\r\n",12);
////	delay_ms(2000);
////	U3_Send_Date("123456789\r\n",12);
//	
////	printf("t\r\n");
//	return 0;
//}

//u8 ec25_tcp_transparent(void)
//{
//	if(ec25_send_cmd("AT","OK",100))  // if can't reveive response, it may in transparent mode.
//		ec25_send_cmdEX("+++","OK",200);  
//	
//	if(ec25_send_cmd("AT+QIOPEN=1,0,\"TCP\",\"47.106.99.188\",10808,0,2","CONNECT",1000)) return EC25_ERR_QIOPEN;
//}

//u8 ec25_dec(void)
//{
//	printf("ec25_dec\r\n");
//	delay_ms(10);
//	//if(ec25_send_cmdEX("+++","OK",1000)) return EC25_ERR_DEACT;	
//	if(ec25_send_cmd("AT","OK",100))  // if can't reveive response, it may in transparent mode.
//		ec25_send_cmdEX("+++","OK",200);  
//	if(ec25_send_cmd("AT+QICLOSE=0","OK",500)) return EC25_ERR_DEACT;
//	if(ec25_send_cmd("AT+QIDEACT=1","OK",500)) return EC25_ERR_DEACT;
//	return 0;
//}

//ec25状态监测
//void ec25_status_check(void)
//{
//	u8 *p1; 
//	if(ec25dev.cmdon==0&&ec25dev.mode==0&&USART3_RX_STA==0)//非指令等待状态.拨号/短信模式/且没有收到任何数据,才允许查询
//	{
//		if(ec25_send_cmd("AT+CSQ","OK",25)==0)//查询信号质量,顺便检测GSM模块状态
//		{
//			p1=(u8*)strstr((const char*)(USART3_RX_BUF),":"); 
//			p1+=2;
//			ec25dev.csq=(p1[0]-'0')*10+p1[1]-'0';//信号质量
//			if(ec25dev.csq>30)ec25dev.csq=30;		
//			ec25dev.status|=1<<7;	//查询GSM模块是否在位?
//		}else 
//		{ 
//			ec25dev.csq=0;	
//			ec25dev.status=0;	//重新查找
//		} 
//		if((ec25dev.status&0XC0)==0X80)//CPIN状态,未获取?
//		{ 
//			ec25_send_cmd("ATE0","OK",100);//不回显(必须关闭,否则接收数据可能异常)
//			if(ec25_send_cmd("AT+CPIN?","OK",25)==0)ec25dev.status|=1<<6;//SIM卡在位
//			else ec25dev.status&=~(1<<6);//SIM卡错误 
//		} 
//		if((ec25dev.status&0XE0)==0XC0)//运营商名字,未获取?
//		{ 
//			if(ec25_send_cmd("AT+COPS?","OK",25)==0)//查询运营商名字
//			{ 
//				p1=(u8*)strstr((const char*)(USART3_RX_BUF),"MOBILE");//查找MOBILE,看看是不是中国移动?
//				if(p1)ec25dev.status&=~(1<<4); //中国移动 
//				else 
//				{
//					p1=(u8*)strstr((const char*)(USART3_RX_BUF),"UNICOM");//查找UNICOM,看看是不是中国联通?
//					if(p1)ec25dev.status|=1<<4;	//中国联通 
//				}
//				if(p1)
//				{
//					ec25dev.status|=1<<5;	//得到运营商名字了 
//					//phone部分通用设置
//					ec25_send_cmd("AT+CLIP=1","OK",100);	//设置来电显示 
//					ec25_send_cmd("AT+COLP=1","OK",100);	//设置被叫号码显示
//					//sms部分通用设置
//					ec25_send_cmd("AT+CMGF=1","OK",100);			//设置文本模式 
//					ec25_send_cmd("AT+CSCS=\"UCS2\"","OK",100);	//设置TE字符集为UCS2 
//					ec25_send_cmd("AT+CSMP=17,0,2,25","OK",100);	//设置短消息文本模式参数 
//				}
//			}else ec25dev.status&=~(1<<5);	//未获得运营商名字
//		}
//		ec25_cmd_over();//处理完毕
//	}
//}

