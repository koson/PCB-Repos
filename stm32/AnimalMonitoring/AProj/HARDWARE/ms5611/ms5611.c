#include <math.h>
#include "ms5611.h"
#include "sys.h"
#include "spi.h"
#include "delay.h"

//硬件连接说明
/*
MS5611   stm32F407
VCC   ----->3.3
VDD   ----->0
PS    ----->0
CSB ----->PA15
SCL ----->PB3_SPI_SCK
SDO ----->PB4_SPI_MISO
SDA ----->PB5_SPI_MOSI
*/

#define MS5611_CS PAout(15)// PA15	
#define CMD_RESET         0x1E  // ADC reset command
#define CMD_PROM_RDC1     0xA2  // Prom read command
#define CMD_PROM_RDC2     0xA4  // Prom read command
#define CMD_PROM_RDC3     0xA6  // Prom read command
#define CMD_PROM_RDC4     0xA8  // Prom read command
#define CMD_PROM_RDC5     0xAA  // Prom read command
#define CMD_PROM_RDC6     0xAC  // Prom read command
#define CMD_PROM_RDC6     0xAC  // Prom read command
#define CMD_TEMP_4096     0x58  // Prom read command
#define CMD_PRES_4096     0x48  // Prom read command

u16 c[6];
u8  data[3];
u32 D1,D2;
int dt,TEMP,T2;
int64_t OFF,SENS,OFF2,SENS2;
int64_t P_array[11];
int32_t  P,P_last;
#define PA_OFFSET_INIT_NUM 100	
static float Alt_offset_Pa=0;
double paOffsetNum = 0; 
uint16_t  paInitCnt=0;
uint8_t paOffsetInited=0;
void MS5611_Init(void)
{
	 SPI1_Init();
//	 SPI1_SetSpeed(SPI_BaudRatePrescaler_16);
	 delay_ms(1000);
	 MS5611_CS = 0;
	 delay_us(10);
	 SPI1_ReadWriteByte(0x1e);//ms5611重启
	 MS5611_CS = 1;
	 delay_ms(3);//延时3ms
}
void MS5611_PROM_READ(void)
{
   MS5611_CS = 0;
	 delay_us(10);
	 SPI1_ReadWriteByte(0xa2);
	 data[0] = SPI1_ReadWriteByte(0x55);
	 data[1] = SPI1_ReadWriteByte(0x55);
	 MS5611_CS = 1;
	 c[0] = 0;
	 c[0] = (c[0]|(data[0]<<8))+data[1];

	 MS5611_CS = 0;
	 delay_us(10);
	 SPI1_ReadWriteByte(0xa4);
	 data[0] = SPI1_ReadWriteByte(0x55);
	 data[1] = SPI1_ReadWriteByte(0x55);
	 MS5611_CS = 1;
	 c[1] = 0;
	 c[1] = (c[1]|(data[0]<<8))+data[1];
	 
	 MS5611_CS = 0;
	 delay_us(10);
	 SPI1_ReadWriteByte(0xa6);
	 data[0] = SPI1_ReadWriteByte(0x55);
	 data[1] = SPI1_ReadWriteByte(0x55);
	 MS5611_CS = 1;
	 c[2] = 0;
	 c[2] = (c[3]|(data[0]<<8))+data[1];
	 
	 MS5611_CS = 0;
	 delay_us(10);
	 SPI1_ReadWriteByte(0xa8);
	 data[0] = SPI1_ReadWriteByte(0x55);
	 data[1] = SPI1_ReadWriteByte(0x55);
	 MS5611_CS = 1;
	 c[3] = 0;
	 c[3] = (c[3]|(data[0]<<8))+data[1];
	 
	 MS5611_CS = 0;
	 delay_us(10);
	 SPI1_ReadWriteByte(0xaa);
	 data[0] = SPI1_ReadWriteByte(0x55);
	 data[1] = SPI1_ReadWriteByte(0x55);
	 MS5611_CS = 1;
	 c[4] = 0;
	 c[4] = (c[4]|(data[0]<<8))+data[1];

	 MS5611_CS = 0;
	 delay_us(10);
	 SPI1_ReadWriteByte(0xac);
	 data[0] = SPI1_ReadWriteByte(0x55);
	 data[1] = SPI1_ReadWriteByte(0x55);
	 MS5611_CS = 1;
	 c[5] = 0;
	 c[5] = (c[5]|(data[0]<<8))+data[1];
}
void MS5611_Temperature_Get(void)
{
   	MS5611_CS = 0;
		delay_us(1);
		SPI1_ReadWriteByte(0x58);//ad开始采集温度值，OSR=4096
		//MS5611_CS = 1;
		delay_ms(9);
		MS5611_CS = 1;
	
		delay_ms(1);
		MS5611_CS = 0;
		delay_us(10);
		SPI1_ReadWriteByte(0x00);//采样结束后读取ad值
		data[0] = SPI1_ReadWriteByte(0x55);
		data[1] = SPI1_ReadWriteByte(0x55);
		data[2] = SPI1_ReadWriteByte(0x55);//共24位
		MS5611_CS = 1;
		D2 = 0;
		D2 = ((D2|(data[0]<<16))|(data[1]<<8))+data[2];
		dt = D2 - (c[4]*256);
		TEMP = 2000+((double)dt*c[5]/8388608+0.5);//2000表示20.00摄氏度
}
void MS5611_Pressure_Get(void)
{
	MS5611_CS = 0;
		delay_us(10);
		SPI1_ReadWriteByte(0x48);//ad开始采集大气压值.OSR=4096
		//MS5611_CS = 1;
		delay_ms(9);
		MS5611_CS = 1;
	
	  delay_ms(1);
		MS5611_CS = 0;
		delay_us(10);
		SPI1_ReadWriteByte(0x00);//采样结束后读取ad值
		data[0] = SPI1_ReadWriteByte(0x55);
		data[1] = SPI1_ReadWriteByte(0x55);
		data[2] = SPI1_ReadWriteByte(0x55);
		MS5611_CS = 1;
		
		D1 = 0;
		D1 = ((D1|(data[0]<<16))|(data[1]<<8))+data[2];
		OFF = (int64_t)c[1]*256*256+((int64_t)c[3]*dt)/128;
		SENS = (int64_t)c[0]*256*128+((int64_t)c[2]*dt)/256;
}

int32_t MS5611_Compensation_Calcu(void)
{
	static unsigned char i = 20,j = 0;
	if(TEMP < 2000)
		{
		  OFF2 = 5*(TEMP - 2000)*(TEMP - 2000)/2;
			SENS2 = 5*(TEMP - 2000)*(TEMP - 2000)/4; 
			T2 = (int)((int64_t)dt*dt/2147483648u);
			if(TEMP < -1500)
			{
				OFF2 = OFF2+7*(TEMP+1500)*(TEMP+1500);
				SENS2 = SENS2+11*(TEMP+1500)*(TEMP+1500)/2;
			}
		}
		else
		{
			OFF2 = SENS2 = T2 = 0;
		}
		
		
		OFF = OFF - OFF2;
		SENS = SENS - SENS2;
		TEMP = TEMP - T2;//校正之后的TEMP
		P = ((int64_t)D1*SENS/2097152-OFF)/32768;//校正之后的Pressure
		//采用消抖滤波法对大气压值进行滤波
		if(i==20)
		{
			P_last = P;
			i=0;
		}
		if(P>P_last)
			i++;
		if(P<P_last)
			j++;
		if((i==10)||(j==10))
			P_last = P;
		if(P==P_last)
		{
			i=j=0;
		}
		return P_last;
	}	
float getEstimatedAltitude(int32_t baroPressure)
{
    float Altitude;
    if(Alt_offset_Pa == 0)
			{ 
        if(paInitCnt > PA_OFFSET_INIT_NUM)
					{
            Alt_offset_Pa = paOffsetNum/paInitCnt;
            paOffsetInited=1;
          }
				else paOffsetNum += baroPressure;  
        paInitCnt++; 
        Altitude = 0; 
        return Altitude;
     }
    Altitude = 4433000.0f * (1.0f - powf((((float) baroPressure) / Alt_offset_Pa), 0.190295f));
return Altitude; 
		 
}
