#include "MyFunction_C.h"


/*
功能：计算字符串中有效字符长度
参数：字符串首地址
返回值：返回字符实际字符长度
*/
uint16 stringLength(const uint8 *string)
{
	uint16 i=0;
	while(*(string+i))i++;
	return i;
}


/*
功能：在指定的数组中搜索指定的字符
参数：
返回值：返回-1表示搜索失败 否则返回搜索到的字符串起始位置
*/
int16 MystringSeek(uint8 *data,uint16 datalen,uint8 *str,uint16 strlen)
{
	uint8 flag=0;
	int16 i=0,j=0;
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
void bytesCopy(uint8 *obj,uint8 *src,uint16 length)
{
	uint16 i;
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
void bytesCleare(uint8 *obj,uint16 length)
{
	uint16 i;
	for(i=0;i<length;i++)
	{
		obj[i]=0;
	}
}



uint8 BytesSum(uint8 *data,uint16 length)
{
	uint16 i,sum=0;
	for(i=0;i<length;i++)
	{
		sum+=data[i];
	}
	return sum;
}
uint8 BytesSumNot(uint8 *data,uint16 length)
{
	uint16 i,sum=0;
	for(i=0;i<length;i++)
	{
		sum+=data[i];
	}
	sum=~sum;
	return sum;
}
//在一个指定长度的数组中，删除一个数，删除后后面的数据向前移动
uint8 BytesDeleteOnenumber(uint8 *Bytes,uint8 BytesLength,uint8 number)
{
	uint8 i=number;
	if(number>=BytesLength||BytesLength==0)return 0;
	for(;i<BytesLength-1;i++)
	{
		Bytes[i]=Bytes[i+1];
	}
	return 1;
}
//在一个数组中,已知有BytesLength个数据,在数组numer处添加一个数据,其它数据向后移动
uint8 BytesAddOnenumber(uint8 *Bytes,uint8 BytesLength,uint8 MaxLength,uint8 number,uint8 value)
{
	uint8 i=BytesLength;
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
