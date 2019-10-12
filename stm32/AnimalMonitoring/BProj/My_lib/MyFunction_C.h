#ifndef _MYFUNCTION_C_H_
#define _MYFUNCTION_C_H_
#include "sys.h"
/*
功能：计算字符串中有效字符长度
参数：字符串首地址
返回值：返回字符实际字符长度
*/
uint16_t stringLength(const uint8_t *string);

int16_t MystringSeek(uint8_t *data,uint16_t datalen,uint8_t *str,uint16_t strlen);

/*
功能：字节数组复制
参数：*obj：目标数据地址
			*src：源数据地址
			length：源数据长度
返回值：无
*/
void bytesCopy(uint8_t *obj,uint8_t *src,uint16_t length);


void bytesCleare(uint8_t *obj,uint16_t length);


uint8_t BytesSum(uint8_t *data,uint16_t length);
uint8_t BytesSumNot(uint8_t *data,uint16_t length);


uint8_t BytesDeleteOnenumber(uint8_t *Bytes,uint8_t BytesLength,uint8_t number);
uint8_t BytesAddOnenumber(uint8_t *Bytes,uint8_t BytesLength,uint8_t MaxLength,uint8_t number,uint8_t value);


uint32_t stringtoNum(char* str);
uint8_t locate_character(char* str, char ch);

u32 Frame_UID_get(void);
unsigned short ModBusCRC(unsigned char *buf, unsigned int lenth);

#endif
