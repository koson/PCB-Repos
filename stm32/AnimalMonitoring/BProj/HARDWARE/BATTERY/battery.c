#include "battery.h"  // self
#include "usart2.h"
#include "delay.h"
int String2NumHex(u8 *str, int length)
{ 
	int  count = 1;
	int  result = 0;
	int  tmp = 0;
	int  i ;
	
	for (i = length - 1; i >= 0; i--)
	{
		if ((str[i] >= '0') && (str[i] <= '9'))
			tmp = str[i] - 48;
		else if ((str[i] >= 'a') && (str[i] <= 'f'))
			tmp = str[i] - 'a' + 10;
		else if ((str[i] >= 'A') && (str[i] <= 'F'))			
			tmp = str[i] - 'A' + 10;
		else
			tmp = 0;
		result = result + tmp * count;
		count = count * 16;
	}
	return result;
}

Battery_stats battery;

void battery_data_anay(void)
{
	u8 t;
	u8 len;	
	
	F407USART2_SendString("<15300420859E708F180>");
	printf("Try get battery info\r\n");
	USART2_RX_STA=0;
	delay_ms(1000);
	if(USART2_RX_STA&0x8000)
	{
		len=USART2_RX_STA&0x3fff;//得到此次接收到的数据长度

		for(t=0;t<len;t++)
		{
			
			printf("%c",USART2_RX_BUF[t]);
			
		}
		printf("\r\n\r\n");//插入换行
				
		battery.SOH=String2NumHex(USART2_RX_BUF+22,2);
		printf("SOH=%d\r\n",battery.SOH);
		battery.total_voltage=String2NumHex(USART2_RX_BUF+24,4);
		printf("total_voltage=%d\r\n",battery.total_voltage);
		
		battery.charge_current=String2NumHex(USART2_RX_BUF+46,4);
		printf("charge_current=%d\r\n",battery.charge_current);
		battery.discharge_current=String2NumHex(USART2_RX_BUF+50,4);
		printf("discharge_current=%d\r\n",battery.discharge_current);

		battery.cell_temperature=String2NumHex(USART2_RX_BUF+56,2);
		printf("cell_temperature=%d\r\n",battery.cell_temperature);
		
		battery.level=String2NumHex(USART2_RX_BUF+116,2);
		printf("level=%d\r\n",battery.level);
		battery.remain_capacity=String2NumHex(USART2_RX_BUF+118,4);
		printf("remain_capacity=%d\r\n",battery.remain_capacity);
		battery.max_capacity=String2NumHex(USART2_RX_BUF+122,4);
		printf("max_capacity=%d\r\n",battery.max_capacity);
		

		
		USART2_RX_STA=0;
	}
	else
	{
		printf("Battery no response\r\n");
	}
}
