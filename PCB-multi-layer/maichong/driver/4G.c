/******************** (C) COPYRIGHT 2016 CETC27-CETIIC-ZYGD ********************
* File Name          : 4G.c
* Author             : WZJ
* Version            : V1.0
* Date               : 11/07/2016
* Description        : 4G模块移远EC20驱动
*******************************************************************************/
#include "4G.h"
#include <stdio.h>
// #include <string.h>
#include <stdlib.h>
#include "spi_flash.h"
#include "save.h"

//#define debug_4g

// struct WirelessConnect EC20;
vu8 Connect_Channel_On[4], GPRS_State[4], GPRS_Act_Counter, GPRS_Open_Counter[4];
//u8 APN_Lenth, APN_Username_Lenth, APN_Password_Lenth;
u8 IP_Port[4][6];
u8 flag_OK, flag_update;
vu16 GPRS_Reset_Count;

char APN[40], APN_Username[40], APN_Password[40]; //APN必须以0结尾
char AT_STRING[200];
char GPRS_Receive_Data[GPRS_Buf_Size], GPRS_Receive_Command[GPRS_Command_Size];
u8 Server_Command_Chn;  //GPRS_Command_Pointer,
u8 Data_Buf2[8];
u8 filldata_buf[256];
u16 GPRS_Update_Pointer;


u8 Deact_flag = 2, Close_flag[4] = {4, 4, 4, 4};
u8 OpenSuccess_flag[4];
u8 CloseFucUsing_flag;  //20170906初始化5改为0

extern vu16 timer_timeout, timer_heart, timer_react, timer_reconnect[], timer_update;
extern u16 TxCounter2, TxPointer2, RxCounter2, RxPointer2;
extern u8 RxBuffer2[], TxBuffer2[];
extern u16 TR_FLAG2, Fra_Len2, Com_Type2;

extern u32 BaseAdress;
extern void Read_Flash2Buffer(void);
extern void Write_Buffer2Flash(void);

extern void Delay1s(u8 Xtime);
extern void Delay(u32 nCount);
extern vu8 GPRS_rssi, GPRS_brr;
extern void Trans_Data_Uart2(void);
extern void senddatafill(void);

/***********************************
*函数名称：arraystr
*输入        ：供搜索的数组首地址指针，要搜索字符串的首地址指针，供搜索数组字节数
*输出        ：供搜索数组中存在搜索字符串，返回第一个字符在数组中的序号（1...N），否则返回0
*描述        ：在供搜索数组中搜索是否存在搜索字符串，如果存在返回数组中需要，否则返回0
*作者        ：王中金
*版本        ：V1.0
*日期        ：2016.11.16
***********************************/
u8 arraystr(char *Array_target, char *string_search, u8 length_target)
{
    u8 i, j, count_search, length_search;
    for (i = 0; * (string_search + i) != 0; i++);
    length_search = i;
//  length_search = strlen(string_search);
    if (length_search <= length_target)
    {
        count_search = length_target - length_search;
        for (i = 0; i < count_search; i++)
        {
            if (*(Array_target + i) == *string_search)
            {
                for (j = 0; j < length_search; j++)
                {
                    if (*(Array_target + i + j) != *(string_search + j))break;
                }
                if (j >= length_search)
                {
                    *(Array_target + i) = 0; //20161216 避免重复判断
                    return i + 1;
                }
            }
        }
    }
    return 0;
}

void EC20_Reset(void)
{
    GPIO_WriteBit(GPIO_CTRL, GPIO_PIN_GPRS_RESET, Bit_SET);
    Delay(1);
    GPIO_WriteBit(GPIO_CTRL, GPIO_PIN_GPRS_RESET, Bit_RESET);
    Delay(20);
    GPIO_WriteBit(GPIO_CTRL, GPIO_PIN_GPRS_RESET, Bit_SET);
}
void EC20_Power(void)
{
    GPIO_WriteBit(GPIOA, GPIO_PIN_GPRS_Pow_Ctrl, Bit_SET);
    Delay(100);
    GPIO_WriteBit(GPIOA, GPIO_PIN_GPRS_Pow_Ctrl, Bit_RESET);
}
void senddatafill(void)
{
  u8 i,j;

  j = Fra_Len2 + 8;
  for( i = 0; i < j ; i++ )
  {
    TxBuffer2[i] = filldata_buf[i];
  }

}
void GPRS_AT(char *ptr, u8 len)
{
    u8 i, k;
    u16 timer_temp;

    k = 0;
    for (i = 0; i < len; i++)
    {
        TxBuffer2[k++] = *ptr;
        ptr++;
    }
    TxBuffer2[k++] = 0x0D;
    TxBuffer2[k++] = 0x0A;

    TxPointer2 = 0;
    TxCounter2 = len + 2;
    for (i = 0; i < GPRS_Command_Size; i++)GPRS_Receive_Command[i] = 0;
    RxPointer2 = 0;
    USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
    timer_temp = timer_timeout;
    while (TxPointer2 < TxCounter2 && timer_timeout - timer_temp < 0x10)
    {
        IWDG_ReloadCounter();
        __nop();
    }
}
u8 GPRS_Exist(void)
{
    u8 i = 0;
    static u16 timer_temp;

    if (timer_temp == 0)
    {
        AT_STRING[i++] = 'A';
        AT_STRING[i++] = 'T';
        GPRS_AT(AT_STRING, i);
        timer_temp = timer_timeout;
    }
    if (timer_timeout - timer_temp < 3)
    {
        if (arraystr(GPRS_Receive_Command, "OK", GPRS_Command_Size) != 0)
        {
            timer_temp = 0;
            return 1;
        }
        if (arraystr(GPRS_Receive_Command, "ERROR", GPRS_Command_Size) != 0)
        {
            timer_temp = 0;
            return 0;
        }
    }
    else
    {
        timer_temp = 0;
        return 0;
    }
    return 2;
}
u8 GPRS_Config_Context(void)
{
    u8 i;
    u16 timer_temp;
// printf(AT_STRING,"AT+IPR=9600;+CSTT=\"%s\",\"\",\"\";+CIPSTART=\"TCP\",\"%03d.%03d.%03d.%03d\",\"%d\"",APN,IP_Port[0],IP_Port[1],IP_Port[2],IP_Port[3],IP_Port[5]*256+IP_Port[4]);
    sprintf(AT_STRING, "AT+QICSGP=1,1,\"%s\",\"%s\",\"%s\",1", APN, APN_Username, APN_Password);
    for (i = 0; AT_STRING[i] != '\0'; i++) {}
    GPRS_AT(AT_STRING, i);
    timer_temp = timer_timeout;
    while (timer_timeout - timer_temp < 2)
    {
        if (arraystr(GPRS_Receive_Command, "OK", GPRS_Command_Size) != 0)return 1;
        if (arraystr(GPRS_Receive_Command, "ERROR", GPRS_Command_Size) != 0)return 0;
        IWDG_ReloadCounter();
    }
    return 0;
}
u8 GPRS_Act_Context(void)
{
    u8 i = 0;
    static u16 timer_temp;

    if (timer_temp == 0)
    {
        AT_STRING[i++] = 'A';
        AT_STRING[i++] = 'T';
        AT_STRING[i++] = '+';
        AT_STRING[i++] = 'Q';
        AT_STRING[i++] = 'I';
        AT_STRING[i++] = 'A';
        AT_STRING[i++] = 'C';
        AT_STRING[i++] = 'T';
        AT_STRING[i++] = '=';
        AT_STRING[i++] = '1';
        GPRS_AT(AT_STRING, i);
        timer_temp = timer_timeout;
    }

//  if(timer_timeout-timer_temp<6)  //20170830修改
    if (timer_timeout - timer_temp < 150)
    {
        if (arraystr(GPRS_Receive_Command, "OK", GPRS_Command_Size) != 0)
        {
            timer_temp = 0;
            return 1;
        }
        if (arraystr(GPRS_Receive_Command, "ERROR", GPRS_Command_Size) != 0)
        {
            timer_temp = 0;
            return 0;
        }
    }
    else
    {
        timer_temp = 0;
        return 0;
    }
    return 2;
}
u8 GPRS_Deact_Context(void)
{
    u8 i = 0;
    static u16 timer_temp;

    if (timer_temp == 0)
    {
        AT_STRING[i++] = 'A';
        AT_STRING[i++] = 'T';
        AT_STRING[i++] = '+';
        AT_STRING[i++] = 'Q';
        AT_STRING[i++] = 'I';
        AT_STRING[i++] = 'D';
        AT_STRING[i++] = 'E';
        AT_STRING[i++] = 'A';
        AT_STRING[i++] = 'C';
        AT_STRING[i++] = 'T';
        AT_STRING[i++] = '=';
        AT_STRING[i++] = '1';
        GPRS_AT(AT_STRING, i);
        timer_temp = timer_timeout;
    }

    if (timer_timeout - timer_temp < 6)
    {
        if (arraystr(GPRS_Receive_Command, "OK", GPRS_Command_Size) != 0)
        {
            timer_temp = 0;
            return 1;
        }
        if (arraystr(GPRS_Receive_Command, "ERROR", GPRS_Command_Size) != 0)
        {
            timer_temp = 0;
            return 0;
        }
    }
    else
    {
        timer_temp = 0;
        return 0;
    }
    return 2;
}
u8 GPRS_Open_Connect(u8 chn)
{
//  char open[13]={'+','Q','I','O','P','E','N',':',' ','0',',','0',0};
    u8 i; //,PT_u8
//  u16 timer_temp;
    sprintf(AT_STRING, "AT+QIOPEN=1,%d,\"TCP\",\"%03d.%03d.%03d.%03d\",%d,0,1", chn, IP_Port[chn][0], IP_Port[chn][1], IP_Port[chn][2], IP_Port[chn][3], IP_Port[chn][5] * 256 + IP_Port[chn][4]);
    for (i = 0; AT_STRING[i] != '\0'; i++) {}
//  open[9] = chn+0x30;
    GPRS_AT(AT_STRING, i);

//   timer_temp = timer_timeout;
//   while(timer_timeout-timer_temp<10) //20161108，5改成10，避免结果出现在下一个通道的回复中造成虚假失败
//   {
//      PT_u8 = arraystr(GPRS_Receive_Command,open,GPRS_Command_Size);
//      if(PT_u8!=0)break;
//     IWDG_ReloadCounter();
//   }
//  if(timer_timeout-timer_temp<10)
//  {
//      if(*(GPRS_Receive_Command+PT_u8+8)==chn+0x30 && *(GPRS_Receive_Command+PT_u8+10)==0x30)
//          return 1;
//  }
    return 0;
}
u8 GPRS_Query_OpenSuccess(u8 chn)
{
    u8 i = 0, PT_u8, PT_u8_temp;
    static u16 timer_temp;

    if (timer_temp == 0)
    {
        AT_STRING[i++] = 'A';
        AT_STRING[i++] = 'T';
        AT_STRING[i++] = '+';
        AT_STRING[i++] = 'Q';
        AT_STRING[i++] = 'I';
        AT_STRING[i++] = 'S';
        AT_STRING[i++] = 'T';
        AT_STRING[i++] = 'A';
        AT_STRING[i++] = 'T';
        AT_STRING[i++] = 'E';
        AT_STRING[i++] = '=';
        AT_STRING[i++] = '1';
        AT_STRING[i++] = ',';
        AT_STRING[i++] = chn + 0x30;
        GPRS_AT(AT_STRING, i);
        timer_temp = timer_timeout;
    }
//  AT+QISTATE=0,1

// +QISTATE: 0,"TCP","123.15.53.254",2050,5036,4,1,0,1,"uart1"
// +QISTATE: 1,"TCP","123.15.53.254",2020,5018,2,1,1,1,"uart1"

// OK
    if (timer_timeout - timer_temp < 6)
    {
//      sprintf(AT_STRING,"AT+QISTATE: %d,\"TCP\",\"%03d.%03d.%03d.%03d\",%d,0,1",chn,IP_Port[chn][0],IP_Port[chn][1],IP_Port[chn][2],IP_Port[chn][3],IP_Port[chn][5]*256+IP_Port[chn][4]);
        if (chn == 0)PT_u8 = arraystr(GPRS_Receive_Command, "+QISTATE: 0", GPRS_Command_Size);
        else if (chn == 1)PT_u8 = arraystr(GPRS_Receive_Command, "+QISTATE: 1", GPRS_Command_Size);
        else if (chn == 2)PT_u8 = arraystr(GPRS_Receive_Command, "+QISTATE: 2", GPRS_Command_Size);
        else if (chn == 3)PT_u8 = arraystr(GPRS_Receive_Command, "+QISTATE: 3", GPRS_Command_Size);

        if (PT_u8 != 0)
        {
            Delay(10);
            PT_u8 += 20;
            PT_u8_temp = arraystr(GPRS_Receive_Command + PT_u8, ",", GPRS_Command_Size - PT_u8); //ip后逗号
            if (PT_u8_temp != 0)
            {
                PT_u8 = PT_u8 + PT_u8_temp;
                PT_u8_temp = arraystr(GPRS_Receive_Command + PT_u8, ",", GPRS_Command_Size - PT_u8); //远程端口后逗号
                if (PT_u8_temp != 0)
                {
                    PT_u8 = PT_u8 + PT_u8_temp;
                    PT_u8_temp = arraystr(GPRS_Receive_Command + PT_u8, ",", GPRS_Command_Size - PT_u8); //本地端口后逗号
                    PT_u8 = PT_u8 + PT_u8_temp;
                    if (*(GPRS_Receive_Command + PT_u8) == '2')
                    {
                        timer_heart = 0;    //GPRS发送成功，心跳清零
                        timer_temp = 0;
                        OpenSuccess_flag[i] = 2;
                        return 1;
                    }
                    else if (*(GPRS_Receive_Command + PT_u8) == '4')
                    {
                        timer_temp = 0;
                        OpenSuccess_flag[i] = 4;
                        return 0;
                    }
                }
            }
        }
        /*      if(PT_u8!=0)
                {
                    Delay(10);
                    PT_u8 += 20;
                    PT_u8_temp = arraystr(GPRS_Receive_Command+PT_u8,",",GPRS_Command_Size-PT_u8);  //ip后逗号
                    if(PT_u8_temp!=0)
                    {
                        PT_u8 = PT_u8+PT_u8_temp;
                        PT_u8_temp = arraystr(GPRS_Receive_Command+PT_u8,",",GPRS_Command_Size-PT_u8);  //远程端口后逗号
                        if(PT_u8_temp!=0)
                        {
                            PT_u8 = PT_u8+PT_u8_temp;
                            PT_u8_temp = arraystr(GPRS_Receive_Command+PT_u8,",",GPRS_Command_Size-PT_u8);  //本地端口后逗号
                            PT_u8 = PT_u8+PT_u8_temp;
                            if(*(GPRS_Receive_Command+PT_u8)=='2')
                            {
                                timer_heart = 0;    //GPRS发送成功，心跳清零
                                timer_temp = 0;
                                return 1;
                            }
                            else if(*(GPRS_Receive_Command+PT_u8)=='4')
                            {
                                timer_temp = 0;
                                return 0;
                            }
                        }
                    }
                }*/
    }
    else
    {
        timer_temp = 0;
        return 2;
    }
    return 2;
}
u8 GPRS_Query_OpenSuccess_1(u8 chn) //20170906
{
    static u16 timer_temp[4];

    if (timer_temp[chn] == 0)
    {
        timer_temp[chn] = timer_timeout;
    }
    if (timer_timeout - timer_temp[chn] < 150)
    {
        if (OpenSuccess_flag[chn] == 1)
        {
            OpenSuccess_flag[chn] = 0;
            timer_temp[chn] = 0;
            return 1;
        }
        else if (OpenSuccess_flag[chn] == 255)
        {
            OpenSuccess_flag[chn] = 0;
            timer_temp[chn] = 0;
            return 0;
        }
    }
    else
    {
        timer_temp[chn] = 0;
        return 2;
    }
    return 2;
}
u8 GPRS_Close_Connect(u8 chn)
{
    u8 i = 0;
    static u16 timer_temp;

    if (CloseFucUsing_flag == 5 && timer_temp == 0)
    {
        AT_STRING[i++] = 'A';
        AT_STRING[i++] = 'T';
        AT_STRING[i++] = '+';
        AT_STRING[i++] = 'Q';
        AT_STRING[i++] = 'I';
        AT_STRING[i++] = 'C';
        AT_STRING[i++] = 'L';
        AT_STRING[i++] = 'O';
        AT_STRING[i++] = 'S';
        AT_STRING[i++] = 'E';
        AT_STRING[i++] = '=';
        AT_STRING[i++] = chn + 0x30;
        AT_STRING[i++] = ',';
        AT_STRING[i++] = '6';   //6秒超时
        GPRS_AT(AT_STRING, i);

        timer_temp = timer_timeout;
        CloseFucUsing_flag = chn;
    }
    if (timer_timeout - timer_temp < 7)
    {
        if (arraystr(GPRS_Receive_Command, "OK", GPRS_Command_Size) != 0)
        {
            timer_temp = 0;
            return 1;
        }
        if (arraystr(GPRS_Receive_Command, "ERROR", GPRS_Command_Size) != 0)
        {
            timer_temp = 0;
            return 0;
        }
    }
    else
    {
        timer_temp = 0;
        return 1;
    }
    return 2;
}

void GPRS_Query_Signal(void)
{
//  AT+CSQ
//  +CSQ: 31,99                     Maximum Response Time 300ms
    u8 i = 0, PT_u8, PT_u8_temp;
    u16 timer_temp;
    AT_STRING[i++] = 'A';
    AT_STRING[i++] = 'T';
    AT_STRING[i++] = '+';
    AT_STRING[i++] = 'C';
    AT_STRING[i++] = 'S';
    AT_STRING[i++] = 'Q';
    GPRS_AT(AT_STRING, i);

    timer_temp = timer_timeout;
    while (timer_timeout - timer_temp < 2)
    {
        PT_u8 = arraystr(GPRS_Receive_Command, "+CSQ: ", GPRS_Command_Size);
        if (PT_u8 != 0)break;
        IWDG_ReloadCounter();
    }
    if (PT_u8 != 0)
    {
        Delay(5);
        GPRS_rssi = atoi(GPRS_Receive_Command + PT_u8 + 5);
        PT_u8_temp = arraystr(GPRS_Receive_Command + PT_u8, ",", GPRS_Command_Size - PT_u8);
        PT_u8 = PT_u8 + PT_u8_temp;
        GPRS_brr = atoi(GPRS_Receive_Command + PT_u8);
    }
}
//4G模块初始化
void GPRS_Init(void)
{
    u8 PT_u8;
    static u16 timer_temp;
    //关闭连接、关闭场景、重新启动 AT+IPR=9600
    char AT_Echo[] = {'A', 'T', 'E', '1'};
    char AT_Echo_Send[] = "AT+QISDE=0";

    if (Connect_Channel_On[0] == 1) // || Connect_Channel_On[1]==1 || Connect_Channel_On[2]==1 || Connect_Channel_On[3]==1)
    {
        if (timer_temp == 0)
        {
            //20170830同时断电源、重启，有些老板子上没有电源引脚，只有复位引脚
            /*          if(GPRS_Act_Counter>3 && GPRS_Act_Counter<9)    //初始化3次以上9次以内，复位模块，否则模块断电重启
                        {
                            EC20_Reset();
                        }else
                        {
                            EC20_Power();
                        }
            */
            EC20_Reset();
            EC20_Power();

            timer_temp = timer_timeout;
        }
        if (timer_timeout - timer_temp < 30) //20170622把时间从16s改到30s
        {
            PT_u8 = arraystr(GPRS_Receive_Command, "RDY", GPRS_Command_Size);
            if (PT_u8 != 0)
            {
                GPRS_AT(AT_Echo, 4);
                Delay(5);
                GPRS_AT(AT_Echo_Send, 10);
                Delay(5);
                GPRS_Config_Context();
                Delay(5);
                GPRS_State[0] = 1;
                GPRS_State[1] = 1;
                GPRS_State[2] = 1;
                GPRS_State[3] = 1;
                timer_temp = 0;
            }
        }
        else
        {
            if (GPRS_Exist() == 1)
            {
                GPRS_AT(AT_Echo, 4);
                Delay(5);
                GPRS_AT(AT_Echo_Send, 10);
                Delay(5);
                GPRS_Config_Context();
                Delay(5);
                GPRS_State[0] = 1;
                GPRS_State[1] = 1;
                GPRS_State[2] = 1;
                GPRS_State[3] = 1;
                timer_temp = 0;
            }
            else
            {
                if (timer_timeout - timer_temp > 43)timer_temp = 0; //模块未启动，等16s+查存在3s，29-19=10s后重新启动  2017090628改成43
            }
        }
    }
}
void GPRS_SetTime(void)
{
}
//关闭连接，重新连接服务器
u8 GPRS_Connect(u8 chn)
{
    if (GPRS_Close_Connect(chn) == 1)
    {
        Delay(1);
        if (GPRS_Open_Connect(chn) == 1)return 1;
    }
//  Delay1s(3);
    return 0;
}
void GPRS_Send(u8 chn)
{
    u8 i, k;
    u16 Len, timer_temp;
    char Temp[40];

    if (chn > 4)while (1); //如果通道数量大于4个，出错，重启
    Len = Fra_Len2 + 8;
    k = 0;
    sprintf(Temp, "AT+QISEND=%d,%d", chn, Len)   ;
    for (i = 0; Temp[i] != '\0'; i++)
    {
        TxBuffer2[k++] = Temp[i];
    }
    TxBuffer2[k++] = 0x0D;
    TxBuffer2[k++] = 0x0A;

    TxPointer2 = 0x00;
    TxCounter2 = k;
    USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
    timer_temp = timer_timeout;
    while (TxPointer2 < TxCounter2 && timer_timeout - timer_temp < 0x10)
    {
        IWDG_ReloadCounter();
        __nop();
    }
    Delay(5);
//   TR_FLAG2 = 0x80;
}
u8 GPRS_Query_SendSuccess(u8 chn)
{
    u8 i, PT_u8, PT_u8_temp;
    u16 timer_temp1, timer_temp2;
//  Delay1s(1);
    RxPointer2 = 0;
    timer_temp1 = timer_timeout;
    while (timer_timeout - timer_temp1 < 3)
    {
        if (arraystr(GPRS_Receive_Command, "SEND OK", GPRS_Command_Size) != 0)break;
        if (arraystr(GPRS_Receive_Command, "SEND FAIL", GPRS_Command_Size) != 0)return 0;
        if (arraystr(GPRS_Receive_Command, "ERROR", GPRS_Command_Size) != 0)return 0;
        IWDG_ReloadCounter();
    }
    timer_temp1 = timer_timeout;
    while (timer_timeout - timer_temp1 < 10)
    {
        Delay(20);
#ifdef debug_4g
        printf("timer_timeout=%d\r\n", timer_timeout);
#endif
        i = 0;      //20161208增加，第二次及以后没清零，出问题
        AT_STRING[i++] = 'A';
        AT_STRING[i++] = 'T';
        AT_STRING[i++] = '+';
        AT_STRING[i++] = 'Q';
        AT_STRING[i++] = 'I';
        AT_STRING[i++] = 'S';
        AT_STRING[i++] = 'E';
        AT_STRING[i++] = 'N';
        AT_STRING[i++] = 'D';
        AT_STRING[i++] = '=';
        AT_STRING[i++] = chn + 0x30;
        AT_STRING[i++] = ',';
        AT_STRING[i++] = '0';
        GPRS_AT(AT_STRING, i);

        //+QISEND: 4,4,0
        timer_temp2 = timer_timeout;
        while (timer_timeout - timer_temp2 < 3) //20161208从3改成5，出现连续查询的状况消失
        {
            PT_u8 = arraystr(GPRS_Receive_Command, "+QISEND: ", GPRS_Command_Size);
            if (PT_u8 != 0)
                break;
            IWDG_ReloadCounter();
        }
        if (PT_u8 != 0)
        {
            Delay(5);
            PT_u8--;
            PT_u8_temp = arraystr(GPRS_Receive_Command + PT_u8, ",", GPRS_Command_Size - PT_u8);
            if (PT_u8_temp != 0)
            {
                PT_u8 = PT_u8 + PT_u8_temp;
                PT_u8_temp = arraystr(GPRS_Receive_Command + PT_u8, ",", GPRS_Command_Size - PT_u8);
                if (PT_u8_temp != 0)
                {
                    PT_u8 = PT_u8 + PT_u8_temp;
                    if (*(GPRS_Receive_Command + PT_u8) == 0x30)
                    {
                        timer_heart = 0;    //GPRS发送成功，心跳清零
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}
u8 GPRS_Send_Heartbeat(u8 chn)
{
    Fra_Len2 = 0x06;
    GPRS_Send(chn);
    Com_Type2 = 0x3A;

    TxBuffer2[0x08] = GPRS_rssi;
    TxBuffer2[0x09] = GPRS_brr;
    if (sizeof(Version) == 1)
    {
        TxBuffer2[0x0A] = '0';
    }
    else  if (sizeof(Version) == 2)
    {
        TxBuffer2[0x0A] = *Version - '0';
    }
    else if (sizeof(Version) == 3)
    {
        TxBuffer2[0x0A] = (*Version - '0') * 10 + *(Version + 1) - '0';
    }
//  TxBuffer2[0x0A] = 3;    //版本号：V1.0 从1到99
    Trans_Data_Uart2();
//  Delay(1);
//   if(GPRS_Query_SendSuccess(chn)==1)return 1;else return 0;
    return 0;
}
void GPRS_Heartbeat(void)
{
    u8 i;
    static u16 timer_temp[4];
    GPRS_Query_Signal();
    for (i = 0; i < 4; i++)
    {
        if (Connect_Channel_On[i] == 1 && GPRS_State[i] == 4)
        {
            if (timer_temp[i] == 0)
            {
                timer_temp[i] = timer_timeout;
                GPRS_Send_Heartbeat(i);
            }
            if (timer_timeout - timer_temp[i] < 10)
            {
                if (GPRS_Query_SendSuccess(i) == 1)
                {
                    timer_temp[i] = 0;
                    GPRS_State[i] = 4;
                }
            }
            else
            {
                timer_temp[i] = 0;
                GPRS_State[i] = 2;
            }
        }
    }
}
void GPRS_Send_AllChn(void)
{
    u8 i;
    for (i = 0; i < 4; i++)
    {
        if (GPRS_State[i] == 4)
        {
            GPRS_Send(i);
            Trans_Data_Uart2();
            if (GPRS_Query_SendSuccess(i) == 0)GPRS_State[i] = 2;
#ifdef debug_4g
            printf("all chn send,GPRS_State=%d\r\n", GPRS_State[i]);
#endif
        }
    }
}
void GPRS_Send_SingleChn(u8 chn)
{
	if(GPRS_State[chn]==0x04)
	{
		GPRS_Send(chn);
		senddatafill();
		Trans_Data_Uart2();
		if(GPRS_Query_SendSuccess(chn)==0)GPRS_State[chn]=2;
		#ifdef debug_4g
		printf("single chn send,GPRS_State=%d\r\n",GPRS_State[chn]);
		#endif
	}
}
//接收数据解包
void GPRS_Receive_Unpack(void)
{
    u8 fail_flag;
    u16 i, j, Length_total, Length_data, Length_prefix, chk0, chk1;
    u16 index, temp1_u16, temp2_u16, timer_temp;
    u32 PcakData_Index, App_Length, App_CRC, App_CRC0;
    /*  //+QIURC: "pdpdeact",0
        //+QIURC: "closed",0
      //+QIURC: "recv",0,4  direct push mode
        //test
        //+QIURC: "recv",0      buffer access mode
        //AT+QIRD=0,1500
        //+QIRD: 5
        //test1
        //
        //OK
        //AT+QIRD=0,1500
        //+QIRD: 0
        //
        //OK
        //AT+QIRD=0,0
        //+QIRD: 0,0,0
        //
        //OK        */
    if (TR_FLAG2 == 0x90)
    {
        timer_heart = 0;
        timer_temp = timer_timeout;
        while (GPRS_Update_Pointer < 12 && timer_timeout - timer_temp < 0x10) //从第一个“开始，等候收到12字节（两种情况都是10字节+\r\n），超时放弃
        {
            IWDG_ReloadCounter();
        }
        if (timer_timeout - timer_temp < 0x10)
        {
            if (arraystr(GPRS_Receive_Data, "\"recv\"", 10) != 0) //收到中心站下发数据
            {
//              GPRS_Reset_Count = 0;   //重启计数器清零        20170523屏蔽
//              GPRS_Receive_Data[GPRS_Receive_Pointer++] = ch;
//              if(GPRS_Receive_Data[0]=='0')Server_Command_Chn = Server_Command_Chn & 0x01;
//              else if(GPRS_Receive_Data[0]=='1')Server_Command_Chn = Server_Command_Chn & 0x02;
//              else if(GPRS_Receive_Data[0]=='2')Server_Command_Chn = Server_Command_Chn & 0x04;
//              else if(GPRS_Receive_Data[0]=='4')Server_Command_Chn = Server_Command_Chn & 0x08;
//              for(i=0;i<10;i++)AT_STRING[i] = GPRS_Receive_Data[i];
                if (GPRS_Receive_Data[7] > '0' && GPRS_Receive_Data[7] < '4')Server_Command_Chn = GPRS_Receive_Data[7] - '0';
                else Server_Command_Chn = 0;
                Delay(1);   //收全到回车换行
                Length_data = atoi(GPRS_Receive_Data + 9);
                Length_prefix = arraystr(GPRS_Receive_Data, "\r\n", 18) + 1;
                Length_total = Length_prefix + Length_data;
                timer_temp = timer_timeout;
                while (GPRS_Update_Pointer < Length_total && timer_timeout - timer_temp < 0x10)
                {
                    IWDG_ReloadCounter();
                }
                if (Length_total <= GPRS_Update_Pointer)
                {
                    for (i = 0; i < 6; i++) //在收到的内容前6个字节中搜索开头字符AA
                    {
                        if (GPRS_Receive_Data[Length_prefix + i] == 0xAA)break;
                    }
                    if (i < 6) //如果没有起始字节则放弃此次内容
                    {
                        i = i + Length_prefix + 2;
                        if (GPRS_Receive_Data[i] != 0xBD) //原来命令
                        {
                            Fra_Len2 = GPRS_Receive_Data[i - 1] + 3;
                            if (Fra_Len2 < 128)
                            {
                                for (j = 0; j < Fra_Len2; j++)RxBuffer2[j] = GPRS_Receive_Data[i + j];
                                Fra_Len2 -= 3;
                                TR_FLAG2 = 0x87;
                            }
                            else
                            {
                                TR_FLAG2 = 0x80;
                            }
                        }
                        else    //远程升级指令
                        {
                            timer_update = 0;
                            if (GPRS_Receive_Data[i - 1] == 0xFF)
                            {
                                Fra_Len2 = GPRS_Receive_Data[i + 3] + (GPRS_Receive_Data[i + 4] << 8) + 5; //整帧从命令到数据域结束长度
                                chk0 = 0;
                                for (j = 0; j < Fra_Len2; j++)
                                {
                                    chk0 = chk0 + GPRS_Receive_Data[i + j];
                                }
                                chk1 = GPRS_Receive_Data[i + j] + (GPRS_Receive_Data[i + j + 1] << 8);
                                if (chk0 == chk1)
                                {
                                    IWDG_ReloadCounter();
                                    App_Length = GPRS_Receive_Data[i + 5] + (GPRS_Receive_Data[i + 6] << 8) + (GPRS_Receive_Data[i + 7] << 16) + (GPRS_Receive_Data[i + 8] << 24);
                                    PcakData_Index = GPRS_Receive_Data[i + 13] + (GPRS_Receive_Data[i + 14] << 8) + (GPRS_Receive_Data[i + 15] << 16) + (GPRS_Receive_Data[i + 16] << 24);
                                    if (PcakData_Index == 0) //第一包
                                    {
                                        Read_Flash2Buffer();
                                        SPI_FLASH_SectorErase(BaseAdress);
                                        for (j = 0; j < 8; j++)Data_Buf2[j] = GPRS_Receive_Data[i + 5 + j];
                                        BaseAdress = 0x1FFF00;
                                        SPI_FLASH_PageWrite(Data_Buf2, BaseAdress, 8);
                                        Write_Buffer2Flash();
                                        flag_update = 1;
                                    }
                                    BaseAdress = 0x1F1000 + PcakData_Index;
                                    SPI_FLASH_BufferWrite(&GPRS_Receive_Data[i + 17], BaseAdress, Fra_Len2 - 17);
                                    if ((PcakData_Index + 1024) < App_Length) //非最后一帧
                                    {
                                        Com_Type2 = 0x7D;
                                    }
                                    else    //最后一帧，重新读出文件长度及校验码，读出全部文件校验
                                    {
                                        BaseAdress = 0x1FFF00;
                                        SPI_FLASH_BufferRead(Data_Buf2, BaseAdress, 8);
                                        App_Length = Data_Buf2[0] + (Data_Buf2[1] << 8) + (Data_Buf2[2] << 16) + (Data_Buf2[3] << 24);
                                        App_CRC = Data_Buf2[4] + (Data_Buf2[5] << 8) + (Data_Buf2[6] << 16) + (Data_Buf2[7] << 24);
                                        App_CRC0 = 0;
                                        temp1_u16 = App_Length / GPRS_Buf_Size;
                                        temp2_u16 = App_Length % GPRS_Buf_Size;
                                        if (temp2_u16 == 0)index = temp1_u16;
                                        else index = temp1_u16 + 1;
                                        BaseAdress = 0x1F1000;
                                        for (j = 0; j < index; j++)
                                        {
                                            if (j == index - 1) //最后一次
                                            {
                                                if (temp2_u16 == 0)
                                                {
                                                    SPI_FLASH_BufferRead(GPRS_Receive_Data, BaseAdress, GPRS_Buf_Size);
                                                    for (i = 0; i < GPRS_Buf_Size; i++)App_CRC0 += GPRS_Receive_Data[i];
                                                }
                                                else
                                                {
                                                    SPI_FLASH_BufferRead(GPRS_Receive_Data, BaseAdress, temp2_u16);
                                                    for (i = 0; i < temp2_u16; i++)App_CRC0 += GPRS_Receive_Data[i];
                                                }
                                            }
                                            else
                                            {
                                                SPI_FLASH_BufferRead(GPRS_Receive_Data, BaseAdress, GPRS_Buf_Size);
                                                BaseAdress += GPRS_Buf_Size;
                                                for (i = 0; i < GPRS_Buf_Size; i++)App_CRC0 += GPRS_Receive_Data[i];
                                            }
                                        }
                                        if (App_CRC0 == App_CRC) //校验正确
                                        {
                                            BKP_WriteBackupRegister(BKP_DR42, 0x00AB);
                                            Com_Type2 = 0x7F;
                                        }
                                        else    //校验错误重发第一包
                                        {
                                            Com_Type2 = 0x7E;
                                            fail_flag = 1;
                                        }
                                    }
                                }
                                else    //整帧校验失败，申请重发
                                {
                                    Com_Type2 = 0x7E;
                                }
                                if (Com_Type2 == 0x7D) //校验正确，确认本帧数据可用
                                {
                                    Fra_Len2 = 0x11;
                                    //20190215
//                                    for (j = 0; j < 14; j++)TxBuffer2[j + 8] = GPRS_Receive_Data[i + 3 + j];
                                    for (j = 0; j < 14; j++)filldata_buf[j + 8] = GPRS_Receive_Data[i + 3 + j];
                                }
                                else if (Com_Type2 == 0x7E) //整帧校验失败，申请重发
                                {
                                    Fra_Len2 = 0x11;
                                    //20190215
//                                    for (j = 0; j < 14; j++)TxBuffer2[j + 8] = GPRS_Receive_Data[i + 3 + j];
//                                    if (fail_flag)  //全部文件校验错误，重发第一帧
//                                    {
//                                        TxBuffer2[j + 18] = 0;
//                                        TxBuffer2[j + 19] = 0;
//                                        TxBuffer2[j + 20] = 0;
//                                        TxBuffer2[j + 21] = 0;
//                                    }
                                    for (j = 0; j < 14; j++)filldata_buf[j + 8] = GPRS_Receive_Data[i + 3 + j];
                                    if (fail_flag)  //全部文件校验错误，重发第一帧
                                    {
                                        filldata_buf[j + 18] = 0;
                                        filldata_buf[j + 19] = 0;
                                        filldata_buf[j + 20] = 0;
                                        filldata_buf[j + 21] = 0;
                                    }
                                }
                                else if (Com_Type2 == 0x7F) //最后一帧正确并文件整体校验正确，发升级文件下载成功信息
                                {
                                    Fra_Len2 = 0x05;
                                }
                                if (GPRS_State[Server_Command_Chn] == 0x04)
                                {
                                    GPRS_Send(Server_Command_Chn);
                                    senddatafill();
                                    Trans_Data_Uart2();
                                }
                                if (Com_Type2 == 0x7F)
                                {
                                    Delay1s(2);
                                    if (GPRS_Close_Connect(Server_Command_Chn) == 1)while (1);
                                    else Delay1s(2);
                                    GPRS_Close_Connect(Server_Command_Chn);
                                    while (1);
                                }
                            }
                            TR_FLAG2 = 0x80;
                        }
                    }
                    else
                    {
                        TR_FLAG2 = 0x80;
                    }
                }
                else
                {
                    TR_FLAG2 = 0x80;
                }
            }
            else if (arraystr(GPRS_Receive_Data, "\"closed\"", 10) != 0) //收到中心站主动断开链接
            {
                switch (GPRS_Receive_Data[9])
                {
                case '0':
                    GPRS_State[0] = 2;
                    break;
                case '1':
                    GPRS_State[1] = 2;
                    break;
                case '2':
                    GPRS_State[2] = 2;
                    break;
                case '3':
                    GPRS_State[3] = 2;
                    break;
                default:
                    break;
                }
                TR_FLAG2 = 0x80;
            }
            else if (arraystr(GPRS_Receive_Data, "\"pdpdeact\"", 12) != 0)
            {
                if (GPRS_Receive_Data[11] == '1')GPRS_State[0] = 1;
            }
            else    //其他情况，暂不考虑，放弃掉
            {
                TR_FLAG2 = 0x80;
            }
        }
        else
        {
            TR_FLAG2 = 0x80;
        }
    }
}

void GPRS_Supervise(void)
{
    u8 Act_flag, Open_flag, i;
    if (Connect_Channel_On[0] == 1) // || Connect_Channel_On[1]==1 || Connect_Channel_On[2]==1 || Connect_Channel_On[3]==1)
    {
        //TCP链接管理
        if (GPRS_State[0] != 0) //4G模块启动成功
        {
            if (GPRS_State[0] >= 2) //场景有效，可以建立链接
            {
                /*          for(i=0;i<4;i++)
                            {
                                if(Connect_Channel_On[i]==1 && GPRS_State[i]==2)
                                {
                                    if(Close_flag[i]==2)
                                    {
                                        GPRS_Close_Connect(i);
                                        Delay(50);
                                        Close_flag[i] = 3;
                                        GPRS_Open_Connect(i);
                                        timer_reconnect[i] = 0;
                                    }
                                }
                            }
                            if(timer_reconnect[i]<GPRS_Reconnect_Time)
                            {
                                GPRS_Query_OpenSuccess(0);
                                for(i=0;i<4;i++)
                                {
                                    if(Connect_Channel_On[i]==0)GPRS_State[i] = 2;
                                    if(Close_flag[i]==3)
                                    {
                                        if(OpenSuccess_flag[i]==2)  //建立TCP连接成功
                                        {
                                            Close_flag[i] = 2;
                                            OpenSuccess_flag[i] = 0;
                                            GPRS_Send_Heartbeat(i);
                                            GPRS_State[i]=4;
                                            GPRS_Open_Counter[i] = 0;
                                        }
                                        else if(OpenSuccess_flag[i]==4)
                                        {
                                            Close_flag[i] = 2;
                                            OpenSuccess_flag[i] = 0;
                                            GPRS_Open_Counter[i]++;
                                        }
                                    }
                                }
                            }else
                            {
                                Close_flag[i] = 2;
                                OpenSuccess_flag[i] = 0;
                                GPRS_Open_Counter[i]++;
                            }*/
                for (i = 0; i < 4; i++)
                {
                    if (Connect_Channel_On[i] == 0)GPRS_State[i] = 2;
                    if (Connect_Channel_On[i] == 1 && GPRS_State[i] == 2) //通道1开启
                    {
                        if (Close_flag[i] == 4)Close_flag[i] = 2;
                        if (Close_flag[i] == 2)
                        {
                            if (CloseFucUsing_flag == i || CloseFucUsing_flag == 5)Close_flag[i] = GPRS_Close_Connect(i);
                        }
                        if ((Close_flag[i] == 0 || Close_flag[i] == 1))
                        {
                            if (timer_reconnect[i] >= 30)
                            {
                                Delay(10);
                                timer_reconnect[i] = 0;
                                GPRS_Open_Connect(i);
                                Close_flag[i] = 3;
                            }
                            CloseFucUsing_flag = 5;
                        }
                        if (Close_flag[i] == 3)
                        {
                            if (timer_reconnect[i] < GPRS_Reconnect_Time)
                            {
                                Open_flag = GPRS_Query_OpenSuccess_1(i);//20170906
                                if (Open_flag == 1) //建立TCP连接成功
                                {
                                    GPRS_Query_Signal();//20170906
                                    Close_flag[i] = 2;
                                    GPRS_Send_Heartbeat(i);
                                    GPRS_State[i] = 4;
                                    GPRS_Open_Counter[i] = 0;
                                }
                                else if (Open_flag == 0)
                                {
                                    Close_flag[i] = 2;
                                    GPRS_Open_Counter[i]++;
                                }
                            }
                            else
                            {
                                Close_flag[i] = 2;
                                GPRS_Open_Counter[i]++;
                            }
                        }
//                      Delay1s(1);
                    }
                }

                //打开的通道中，所有通道都在连接6次不成功，重新激活场景
                if ((!(Connect_Channel_On[0] == 1 && GPRS_Open_Counter[0] < 6)) && (!(Connect_Channel_On[1] == 1 && GPRS_Open_Counter[1] < 6))
                        && (!(Connect_Channel_On[2] == 1 && GPRS_Open_Counter[2] < 6)) && (!(Connect_Channel_On[3] == 1 && GPRS_Open_Counter[3] < 6)))GPRS_State[0] = 0; //20170830修改1->0
            }
            else if (GPRS_State[0] == 1) //场景未激活，关闭场景，激活场景
            {
                if (timer_react > GPRS_React_Time)
                {
                    timer_react = 0;
                    if (Deact_flag == 2)
                    {
                        Deact_flag = GPRS_Deact_Context();
                        if (Deact_flag == 0)Deact_flag = 2;
                    }
                    if (Deact_flag == 1)
                    {
                        Delay(1);
                        Act_flag = GPRS_Act_Context();
                        if (Act_flag == 1)
                        {
                            GPRS_Open_Counter[0] = 0;   //一旦场景重新激活，必须重新连接次数达到6次以上才能再次激活场景
                            GPRS_Open_Counter[1] = 0;
                            GPRS_Open_Counter[2] = 0;
                            GPRS_Open_Counter[3] = 0;
                            GPRS_State[0] = 2;
                            GPRS_State[1] = 2;
                            GPRS_State[2] = 2;
                            GPRS_State[3] = 2;

                            Deact_flag = 2;
                        }
                        else if (Act_flag == 0)
                        {
                            GPRS_Act_Counter++;
                            if (GPRS_Act_Counter > 6)
                            {
                                GPRS_Act_Counter = 0;
                                GPRS_State[0] = 0;
                            }
                            Deact_flag = 2;

                        }
                    }
                }
            }
        }
        else    //重启模块
        {
//          #ifdef debug_4g
//          printf("GPRS_State=%d,GPRS_Act_Counter=%d\r\n",GPRS_State[i],GPRS_Act_Counter);
//          #endif
            GPRS_Init();
        }
        if (GPRS_State[0] == 4)GPIO_WriteBit(GPIO_CTRL, GPIO_PIN_GPRS_LED, Bit_RESET);
        else GPIO_WriteBit(GPIO_CTRL, GPIO_PIN_GPRS_LED, Bit_SET);
        //3 minute心跳
        if (timer_heart > 180)
        {
            timer_heart = 0;
            GPRS_Reset_Count++;
            if (GPRS_Reset_Count > 500)while (1); //1天+1小时没有接到命令重启一次
            GPRS_Heartbeat();
        }
        //接收数据解析，并把内容转交给main函数
        GPRS_Receive_Unpack();
        if (flag_update == 1 && timer_update > 30)flag_update = 0; //升级状态在30秒钟内没有收到下一帧，解除升级状态
    }
    else        //20170609修改，如果第一通道关闭，则复位通道状态，关闭模块
    {
        GPRS_State[0] = 0;
        GPRS_State[1] = 0;
        GPRS_State[2] = 0;
        GPRS_State[3] = 0;
        GPIO_WriteBit(GPIOA, GPIO_PIN_GPRS_Pow_Ctrl, Bit_SET);
    }
}
/******************** (C) COPYRIGHT 2016 CETC27-CETIIC-ZYGD ********************/
