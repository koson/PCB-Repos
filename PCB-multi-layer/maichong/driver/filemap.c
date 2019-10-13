/******************** (C) COPYRIGHT 2017 CETC27-CETIIC-ZYGD ********************
* File Name          : filemap.c
* Author             : WZJ
* Version            : V1.0
* Date               : 2017/05/12
* Description        : 每分钟一条数据占一页，共31936条数据，22天零256条。建立时间到存储地址的映射转换。
*******************************************************************************/
#include "filemap.h"
#include "Rtc_time.h"
#include "spi_flash.h"
#include <stdio.h>

#define addr_begin1  0x000000
#define totalpages1 (256*20)
#define pages1 3
#define length1 (256/pages1)
#define saveperiod1 600
#define addr_begin2  0x140000
#define totalpages2 (256*11)
#define pages2 4
#define length2 (256*pages2)
#define saveperiod2 600

u8 datasave_buf[1024], datasend_buf[256], rawsend_buf[1024];
u16 datasave_length;
u8 cur_time[7], historydata_time[7],collect_time[7];
u32 historydata_unixtime[2],  historydata_raw_unixtime[2];
u32 sendagain_unixtime;

vu8 collect_mode, autosend;
vu8 collectperiod, sendperiod[3];
vu16 sendperiod_raw;

//    collect_mode = 0; //实时采集0,定时采集1
//    collectperiod = 1; //采集周期
//    sendperiod[0] = 1; //实时发送周期
//    sendperiod[1] = 60; //定时发送周期

extern struct tm time_now;
extern vu16 timer_timeout;
extern u32 BaseAdress;
extern u8 TxBuffer1[], TxBuffer3[];
extern vu16 Com_Type3, Fra_Len3;
extern void Trans_Data_Uart3(void);
int Send_RawData(void);
void send_data(u8 mode, u8 *pDateTime);
void send_missdata(u8 mode, u8 *pDateTime);
//------------------------------------------//
// - 函数名称：Unixtime_ArraytoU32
// - 输入        ：日期时间，年月日时分
// - 输出        ：Linux时间戳
// - 描述        ：转换时间成秒数
// - 作者        ：王中金
// - 版本        ：V1.0
// - 日期        ：2017.05.12
//------------------------------------------//
u32 Unixtime_ArraytoU32(u8 *pDateTime)
{
    time_now.tm_year = pDateTime[0] * 100 + pDateTime[1];   //2017
    time_now.tm_mon = pDateTime[2] - 1;
    time_now.tm_mday = pDateTime[3];
    time_now.tm_hour = pDateTime[4];
    time_now.tm_min = pDateTime[5];
    time_now.tm_sec = pDateTime[6];
    return Time_ConvCalendarToUnix(time_now);
}
void Unixtime_U32toArray(u8 *pDateTime, u32 tim)
{
    time_now = Time_ConvUnixToCalendar(tim);
    pDateTime[0] = time_now.tm_year / 100;
    pDateTime[1] = time_now.tm_year % 100;
    pDateTime[2] = time_now.tm_mon + 1;
    pDateTime[3] = time_now.tm_mday;
    pDateTime[4] = time_now.tm_hour;
    pDateTime[5] = time_now.tm_min;
    pDateTime[6] = time_now.tm_sec;
}
/*
u32 FileMap(u8 mode, u8 *pDateTime)
{
    u32 Addr,tmp;
    if (mode == 0)//每页pages1条数据
    {
        UnixTime = Unixtime_ArraytoU32(pDateTime);
        tmp = (UnixTime / 60) % (totalpages1*pages1);
        Addr=addr_begin1;
        Addr += (tmp/pages1) << 8;
        Addr += (tmp%pages1)*(256/pages1);
    }else if (mode == 1)//每条数据pages2页
    {
        UnixTime = Unixtime_ArraytoU32(pDateTime);
        tmp = (UnixTime / 60) % (totalpages2 / pages2);
        Addr=addr_begin2;
        Addr += (tmp*pages2) << 8;
    }else
    {
        Addr = 0xFFFFFFFF;
    }
    return Addr;
}
*/
u32 FileMap(u8 datatype, u32 unixtime)
{
    u32 Addr, tmp;
    if (datatype == 0)//每页pages1条数据  用于存储冻土数据
    {
//        tmp = (unixtime / peroidtosecond(sendperiod[0])) % (totalpages1 * pages1);
        tmp = (unixtime / saveperiod1) % (totalpages1 * pages1);
        Addr = addr_begin1;
        Addr += (tmp / pages1) << 8;
        Addr += (tmp % pages1) * (256 / pages1);
    }
    else if (datatype == 1) //每条数据pages2页  用于存储原始数据
    {
//        tmp = (unixtime / peroidtosecond(sendperiod[0])) % (totalpages2 / pages2);
        tmp = (unixtime / sendperiod_raw) % (totalpages2 / pages2);//20190218 saveperiod2改成sendperiod_raw
        Addr = addr_begin2;
        Addr += (tmp * pages2) << 8;
    }
    else
    {
        Addr = 0xFFFFFFFF;
    }
    return Addr;
}
int DataSave(u8 datatype, u8 *pDateTime)
{
    u16 i,len;
    if (datatype > 1)return -1;
    BaseAdress = FileMap(datatype, Unixtime_ArraytoU32(pDateTime));//datatype  0 冻土数据 1 原始数据
    if ((BaseAdress & 0x00FFFF) == 0)
        SPI_FLASH_SectorErase(BaseAdress);
    
    if (datatype == 0)
    {
        len = length1;
    }
    else if (datatype == 1)
    {
        len = length2;
    }
    //20190219
    len = datasave_length;
    //20190218
    SPI_FLASH_BufferRead(TxBuffer3, BaseAdress, len);
    for (i = 0; i < len; i++)
    {
        if (TxBuffer3[i] != 0xFF)
        {
            SPI_FLASH_SectorErase(BaseAdress);
            break;
        }
    }
    
//    SPI_FLASH_PageWrite(datasave_buf, BaseAdress, len);
    SPI_FLASH_BufferWrite(datasave_buf, BaseAdress, len);
    return 0;
}
u32 peroidtosecond(vu8 period)
{
    if (period == 0)
    {
        return 1;
    }
    else if (period < 60)
    {
        return period*60;
    }
    else if (period < 84)//20190218sendperiod[2]
    {
        return 3600 * (period - 59);
    }
    return 3600;
}
int DataSend(u8 mode, u8 *pDateTime)//20190218
{
    u8 i,cur_time_temp;

    if(mode>1)return -1;

    if(mode==0)//实时
    {
        BaseAdress = FileMap(0, Unixtime_ArraytoU32(collect_time));//datatype  0 冻土数据 1 原始数据
        SPI_FLASH_BufferRead(datasend_buf, BaseAdress, length1);
        for (i = 0; i < 6; i++)
        {
            if (datasend_buf[i+2] != collect_time[i])break;//20190222 datasend_buf[i+2] != pDateTime[i]
        }
        if (i < 5)
        {
            send_missdata(mode,pDateTime);//无数据
    //        printf("%02d%02d-%02d-%02d %02d:%02d, Date Error...\r\n", pDateTime[0], pDateTime[1], pDateTime[2], pDateTime[3], pDateTime[4], pDateTime[5]);
    //        printf("%02d%02d-%02d-%02d %02d:%02d\r\n", datasave_buf[0], datasend_buf[1], datasend_buf[2], datasend_buf[3], datasend_buf[4], datasend_buf[5]);
        }
        else
        {
            send_data(mode,pDateTime);
        }
    }else if(mode==1)//定时
    {
        cur_time_temp = collect_time[5];
        collect_time[5] = 0;
        BaseAdress = FileMap(0, Unixtime_ArraytoU32(collect_time));//datatype  0 冻土数据 1 原始数据
//        collect_time[5] = cur_time_temp;//20190222移到下面
        SPI_FLASH_BufferRead(datasend_buf, BaseAdress, length1);
        for (i = 0; i < 5; i++)
        {
            if (datasend_buf[i+2] != collect_time[i])break;
        }
        if (i < 5)
        {
            send_missdata(mode,collect_time);
        }
        else
        {
            send_data(mode,collect_time);
        }
        collect_time[5] = cur_time_temp;//20190222
    }
    return 0;
}
int DataSend_raw(u8 *pDateTime)
{
    u8 i;

//    *(pDateTime+6) = 0;
    BaseAdress = FileMap(1,Unixtime_ArraytoU32(pDateTime));//datatype  0 冻土数据 1 原始数据
    SPI_FLASH_BufferRead(rawsend_buf, BaseAdress, length2);
    for (i = 0; i < 6; i++)
    {
        if (rawsend_buf[i+2] != pDateTime[i])break;
    }
    if (i < 5)
    {
//        printf("%02d%02d-%02d-%02d %02d:%02d, Date Error...\r\n", pDateTime[0], pDateTime[1], pDateTime[2], pDateTime[3], pDateTime[4], pDateTime[5]);
//        printf("%02d%02d-%02d-%02d %02d:%02d\r\n", rawsend_buf[0], rawsend_buf[1], rawsend_buf[2], rawsend_buf[3], rawsend_buf[4], rawsend_buf[5]);
        return 0;
    }
    else
    {
        return Send_RawData();
    }
}
int DataSend1(u8 mode, u8 *pDateTime)
{
    u8 i,cur_time_temp;//[7];

    if(mode>1)return -1;

    //20190110
    cur_time_temp = pDateTime[5];
    pDateTime[5] -= pDateTime[5]%10;
    
    BaseAdress = FileMap(0, Unixtime_ArraytoU32(pDateTime));//datatype  0 冻土数据 1 原始数据
    SPI_FLASH_BufferRead(datasend_buf, BaseAdress, length1);

    if(mode==0)//实时
    {
        for (i = 0; i < 6; i++)
        {
            if (datasend_buf[i+2] != pDateTime[i])break;
        }
    }else if(mode==1)//定时
    {
        for (i = 0; i < 5; i++)
        {
            if (datasend_buf[i+2] != pDateTime[i])break;
        }
    }
    if (i < 5)
    {
        //20180111该增加存储内容为空的输出
        if(mode==0)
        {
            pDateTime[5] = cur_time_temp;
        }
        
        send_missdata(mode,pDateTime);
//        send_data(mode,1,pDateTime);//无数据
//        printf("%02d%02d-%02d-%02d %02d:%02d, Date Error...\r\n", pDateTime[0], pDateTime[1], pDateTime[2], pDateTime[3], pDateTime[4], pDateTime[5]);
//        printf("%02d%02d-%02d-%02d %02d:%02d\r\n", datasave_buf[0], datasend_buf[1], datasend_buf[2], datasend_buf[3], datasend_buf[4], datasend_buf[5]);
    }
    else
    {
        pDateTime[5] = cur_time_temp;//20190110
        send_data(mode,pDateTime);
    }
    return 0;
}
void historydatasend(void)//下载
{
//    static u16 timer;

    if (historydata_unixtime[0] <= historydata_unixtime[1])
    {
//        if(timer_timeout - timer > 0)
//        {
//            timer = timer_timeout;
            Unixtime_U32toArray(historydata_time, historydata_unixtime[0]);
            DataSend1(historydata_flag-1, historydata_time); //historydata_flag 1实时数据 2定时数据
            historydata_unixtime[0] += peroidtosecond(sendperiod[2]);
//        }
    }
    else
    {
        historydata_flag = 0;
    }
}
void historydatasend_raw(void)
{
    static u16 timer;
//    u32 period;

    //20190110
//    if (collectperiod == 0)
//    {
//        period = 1;
//    }
//    else if (collectperiod < 60)
//    {
//        period = collectperiod * 60;
//    }
//    else if (collectperiod < 84)
//    {
//        period = 3600 * (collectperiod - 60);
//    }
    if (historydata_raw_unixtime[0] <= historydata_raw_unixtime[1])//20190214修正条件放到内部，否则标志位清零导致只传一条  && (timer_timeout - timer > 0)
    {
        if(timer_timeout - timer > 0)
        {
            timer = timer_timeout;
            Unixtime_U32toArray(historydata_time, historydata_raw_unixtime[0]);
            if (DataSend_raw(historydata_time) == 0)
            {
                historydata_raw_unixtime[0] += sendperiod_raw;
            }
        }
    }
    else
    {
        historydata_raw_flag = 0;
    }
}
void datasend_again_raw(void)//20190218
{
//    static u16 timer;
    u32 unixtime_temp;

//    if (collectperiod == 0)
//    {
//        period = 1;
//    }
//    else if (collectperiod < 60)
//    {
//        period = collectperiod * 60;
//    }
//    else if (collectperiod < 84)
//    {
//        period = 3600 * (collectperiod - 60);
//    }
    //20190110
//    sendagain_unixtime = BKP_ReadBackupRegister(BKP_DR10) + (BKP_ReadBackupRegister(BKP_DR11) << 8) + period;
//    sendagain_unixtime = BKP_ReadBackupRegister(BKP_DR10) + (BKP_ReadBackupRegister(BKP_DR11) << 8);
    unixtime_temp = sendagain_unixtime+sendperiod_raw;
    if (unixtime_temp <= Unixtime_ArraytoU32(cur_time))
    {
//        if (timer_timeout - timer > 1)//20190110
//        {
//            timer = timer_timeout;
            Unixtime_U32toArray(historydata_time, unixtime_temp);
            if (DataSend_raw(historydata_time) == 0)
            {
                sendagain_unixtime = unixtime_temp;
                BKP_WriteBackupRegister(BKP_DR10, sendagain_unixtime);
                BKP_WriteBackupRegister(BKP_DR11, sendagain_unixtime >> 16);
            }
//            sendagain_unixtime += sendperiod_raw;
//        }
    }
    else
    {
        sendagain_raw_flag = 0;
    }
}
/******************** (C) COPYRIGHT 2017 CETC27-CETIIC-ZYGD ********************/
