/******************** (C) COPYRIGHT 2017 CETC27-CETIIC-ZYGD ********************
* File Name          : filemap.h
* Author             : WZJ
* Version            : V1.0
* Date               : 2017/05/12
* Description        : 每分钟一条数据占一页，共31936条数据，22天零256条。建立时间到存储地址的映射转换。
*******************************************************************************/

#ifndef __FILEMAP_H__
#define __FILEMAP_H__

#include "stm32f10x_lib.h"

extern u8 datasave_buf[], datasend_buf[], rawsend_buf[];
extern u16 datasave_length;
extern u8 cur_time[], historydata_time[],collect_time[];
extern vu8 historydata_flag, historydata_raw_flag, sendagain_raw_flag;
extern vu8 collect_mode, autosend;
extern vu8 collectperiod, sendperiod[];
extern vu16 sendperiod_raw;

extern u32 sendagain_unixtime;
extern u32 historydata_unixtime[],  historydata_raw_unixtime[];

u32 Unixtime_ArraytoU32(u8 *pDateTime);
void Unixtime_U32toArray(u8 *pDateTime,u32 tim);
u32 FileMap(u8 datatype, u32 unixtime);
int DataSave(u8 datatype, u8 *pDateTime);
u32 peroidtosecond(vu8 period);
int DataSend(u8 mode, u8 *pDateTime);
void historydatasend(void);
void historydatasend_raw(void);
void datasend_again_raw(void);

#endif

/******************** (C) COPYRIGHT 2017 CETC27-CETIIC-ZYGD ********************/
