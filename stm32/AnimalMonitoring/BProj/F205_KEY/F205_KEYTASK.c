#include "os_include.h"

#include "usertypedef.h"

#include "F205_KEY.h"
#include "F205_KEYTASK.h"

#include "F407USART1.h"

extern uint8 key_down=0;
extern uint8 key2_down=0;


void keyup(u8 value,uint16 time)
{
	//OS_ERR err;
	if(value&KEY1)
	{
		
	}
}


void keyDown(u8 value,uint16 time)
{
	//OS_ERR err;
	if(value&KEY1)
	{
		F407USART1_SendString("key1_down\r\n");
		key_down=1;
	}
	if(value&KEY2)
	{
		F407USART1_SendString("key2_down\r\n");
		key2_down=1;
	}	
}

//按键扫描任务函数
void  KeyScan_Task(void *p_arg)
{
	OS_ERR err;
	const uint16 divtime=30;
	//
	F205_KEYInit(keyup,keyDown,0,0);
	//
	printf("KeyScan_Task run\r\n");
	while(1)
	{
		F205_KEYRun(divtime);
		OSTimeDly(divtime,OS_OPT_TIME_DLY,&err);
	}
}
