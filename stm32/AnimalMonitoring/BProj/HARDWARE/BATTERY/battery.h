#ifndef __BATTERY_H
#define __BATTERY_H
#include "sys.h"

typedef struct{
	u16 SOH;  // 电池健康状态
    u16 total_voltage;  // 总电压
    u16 charge_current;  // 充电电流
	u16 discharge_current;  // 放电电流
    u16 cell_temperature;  // 电池温度
	u16 level;  // 电池容量
	u16 max_capacity;  // 电池容量
	u16 remain_capacity;  // 当前电量
} Battery_stats;

void battery_data_anay(void);
extern Battery_stats battery;
#endif
