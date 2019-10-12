#include "MyFunction_C.h"
#include "rng.h"
/*
功能：计算字符串中有效字符长度
参数：字符串首地址
返回值：返回字符实际字符长度
*/
uint16_t stringLength(const uint8_t *string)
{
	uint16_t i=0;
	while(*(string+i))i++;
	return i;
}


/*
功能：在指定的数组中搜索指定的字符
参数：
返回值：返回-1表示搜索失败 否则返回搜索到的字符串起始位置
*/
int16_t MystringSeek(uint8_t *data,uint16_t datalen,uint8_t *str,uint16_t strlen)
{
	uint8_t flag=0;
	int16_t i=0,j=0;
	for(i=0;i<=datalen-strlen;i++)
	{
		if(data[i]!=str[0])continue;
		//
		for(j=0,flag=0;j<strlen;j++)
		{
			if(data[i+j]!=str[j])
			{
				flag=1;
				break;
			}
		}
		if(flag==0)return i;
	}
	return -1;
}



/*
功能：字节数组复制
参数：*obj：目标数据地址
			*src：源数据地址
			length：源数据长度
返回值：无
*/
void bytesCopy(uint8_t *obj,uint8_t *src,uint16_t length)
{
	uint16_t i;
	for(i=0;i<length;i++)
	{
		obj[i]=src[i];
	}
}
/*
功能：清空字节数组
参数：*obj：目标数据地址
			length：源数据长度
返回值：无
*/
void bytesCleare(uint8_t *obj,uint16_t length)
{
	uint16_t i;
	for(i=0;i<length;i++)
	{
		obj[i]=0;
	}
}

uint32_t stringtoNum(char* str)
{
	uint32_t num = 0;
	uint8_t  cnt = 0;
	while (str[cnt]>='0' && str[cnt]<='9')
	{
		num *= 10;
		num += (str[cnt] - '0');
		cnt++;
	}
	return num;
}

uint8_t locate_character(char* str, char ch)
{
	uint8_t i=0;
	#define MAX_LOCATE_LEN 20
	while(i <MAX_LOCATE_LEN)
	{
		if(str[i] == ch)
			return i;
		i++;
	}
	return 0;
}

uint8_t BytesSum(uint8_t *data,uint16_t length)
{
	uint16_t i,sum=0;
	for(i=0;i<length;i++)
	{
		sum+=data[i];
	}
	return sum;
}
uint8_t BytesSumNot(uint8_t *data,uint16_t length)
{
	uint16_t i,sum=0;
	for(i=0;i<length;i++)
	{
		sum+=data[i];
	}
	sum=~sum;
	return sum;
}
//在一个指定长度的数组中，删除一个数，删除后后面的数据向前移动
uint8_t BytesDeleteOnenumber(uint8_t *Bytes,uint8_t BytesLength,uint8_t number)
{
	uint8_t i=number;
	if(number>=BytesLength||BytesLength==0)return 0;
	for(;i<BytesLength-1;i++)
	{
		Bytes[i]=Bytes[i+1];
	}
	return 1;
}
//在一个数组中,已知有BytesLength个数据,在数组numer处添加一个数据,其它数据向后移动
uint8_t BytesAddOnenumber(uint8_t *Bytes,uint8_t BytesLength,uint8_t MaxLength,uint8_t number,uint8_t value)
{
	uint8_t i=BytesLength;
	if(number>BytesLength||BytesLength>=MaxLength)return 0;
	if(number==BytesLength)
	{
		Bytes[number]=value;
	}
	else 
	{
		for(i=BytesLength;i>number;i--)
		{
			Bytes[i]=Bytes[i-1];
		}	
		Bytes[number]=value;
	}
	return 1;
}

//////////////////////CRC
#define POLY 0xa001
unsigned short ModBusCRC(unsigned char *buf, unsigned int lenth)
{
	int i, j;
	unsigned short crc;
	for (i = 0, crc = 0xffff; i < lenth; i++)
	{
		crc ^= buf[i];
		//printf("%0X ",buf[i]);
		for (j = 0; j < 8; j++)
		{
			if (crc & 0x01)
				crc = (crc >> 1) ^ POLY;
			else
				crc >>= 1;
		}
	}
	return crc;
}
//////////////////////UID
u32 Frame_UID_get(void)
{
	u32 uid = 0;
	uid += (*(volatile uint32_t *)0x1fff7a10);
	uid += (*(volatile uint32_t *)0x1fff7a14);
	uid += (*(volatile uint32_t *)0x1fff7a18);

	uid += RTC->TR;
	uid += RTC_GetSubSecond();

	uid += rngGetRandomNum();
	return uid;
}
//////////////////////
