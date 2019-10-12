   #include "sensor.h"  // self
#include <stdio.h>   // printf
#include <string.h>  // string func
#include "cJSON.h"   // json praser

#include "spi.h"     // spi
#include "myiic_sht20.h"  	// iic
#include "myiic_max44009.h" // iic
#include "ms5611.h"	 // ms5611
#include "SHT2x.h"   // SHT2x 
#include "max44009.h"// max44009
#include "battery.h"
#include "rtc.h"     // spi
#include "malloc.h"
//extern int32_t BaroPressure;
//extern float getEstimatedAltitude(int32_t baroPressure);
//extern float BaroAlt;
//extern float temp,Humi;	
//extern uint32_t flux;


//{"mt":"sdata","sid":"11","slist":[
//{"time":"2017-01-01 12:09:12","SHT21":"temp","value":"12.0"},
//{"time":"2017-01-01 12:09:12","SHT21":"hum","value":"50.5"},
//{"time":"2017-01-01 12:09:12","MS5611":" pres","value":"10000"},
//{"time":"2017-01-01 12:09:12","MAX44009":"lus","value":"100"},]}

static char now_time[30] = "2019-09-23";//一个宏定义/函数将数值变为时间 
static float temp = 23.0;
static float hum = 50.2;
static uint32_t lus = 100/3;
int32_t BaroPressure=0;
float BaroAlt = 0.0f;

void Data_Packing_sens(char *msg)
{
	char * output;
	cJSON * root =  cJSON_CreateObject();
	cJSON * item =  cJSON_CreateObject();
	
    cJSON * item_temp =  cJSON_CreateObject();
    cJSON * item_hum =  cJSON_CreateObject();
    cJSON * item_pres =  cJSON_CreateObject();
    cJSON * item_lus =  cJSON_CreateObject();
	cJSON * item_volt =  cJSON_CreateObject();
	cJSON * item_cur =  cJSON_CreateObject();
	cJSON * item_cap =  cJSON_CreateObject();


	// get data
	calendar_get_time(&calendar);
	calendar_get_date(&calendar);
	sprintf(now_time,"%d-%d-%d %d:%d:%d",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);
	
	temp=24;
	hum=66;
	//getFluxMeasurement(&	lus);
	lus=30045;
	BaroPressure=100600;
	BaroAlt=300;
	
//	temp=SHT2x_GetTempPoll();
//	hum=SHT2x_GetHumiPoll();
//	getFluxMeasurement(&lus);

//	BaroPressure=MS5611_Compensation_Calcu();
//	BaroAlt=getEstimatedAltitude(BaroPressure);
//	printf("BaroPressure: %d,BaroAlt=%f\r\n",BaroPressure,BaroAlt);	
	
	if(BaroPressure<0 || BaroPressure>158823)
		BaroPressure=98823;
	cJSON_AddItemToObject(root, "mt", cJSON_CreateString("sdata"));//根节点下添加
	cJSON_AddItemToObject(root, "sid", cJSON_CreateString("11"));

	battery.charge_current = 9999;
	battery.total_voltage = 9999;
	battery.remain_capacity = 9999;
	battery_data_anay();  // 获取电池信息

	
	item = cJSON_CreateArray();    
    cJSON_AddItemToObject(root, "slist", item);//添加子树 
	 
    //SHT21-temp
    cJSON_AddItemToObject(item_temp, "time", cJSON_CreateString(now_time));//添加time节点
    cJSON_AddItemToObject(item_temp, "sensor_type", cJSON_CreateString("temp"));//添加SHT21节点
	cJSON_AddItemToObject(item_temp, "sensor_model", cJSON_CreateString("SHT21"));//添加SHT21节点
    cJSON_AddItemToObject(item_temp, "value", cJSON_CreateNumber(temp));//添加value节点
    //SHT21-humi
    cJSON_AddItemToObject(item_hum, "time", cJSON_CreateString(now_time));//添加time节点
	cJSON_AddItemToObject(item_hum, "sensor_type", cJSON_CreateString("hum"));//添加SHT21节点
	cJSON_AddItemToObject(item_hum, "sensor_model", cJSON_CreateString("SHT21"));//添加SHT21节点
    cJSON_AddItemToObject(item_hum, "value", cJSON_CreateNumber(hum));//添加value节点
	//MS5611-pres 
	cJSON_AddItemToObject(item_pres, "time", cJSON_CreateString(now_time));//添加time节点
	cJSON_AddItemToObject(item_pres, "sensor_type", cJSON_CreateString("pres"));//添加SHT21节点
	cJSON_AddItemToObject(item_pres, "sensor_model", cJSON_CreateString("MS5611"));//添加SHT21节点
    cJSON_AddItemToObject(item_pres, "value", cJSON_CreateNumber((int)BaroPressure));//添加value节点
    //MAX44009-lus
	cJSON_AddItemToObject(item_lus, "time", cJSON_CreateString(now_time));//添加time节点
	cJSON_AddItemToObject(item_lus, "sensor_type", cJSON_CreateString("lus"));//添加SHT21节点
	cJSON_AddItemToObject(item_lus, "sensor_model", cJSON_CreateString("MAX44009"));//添加SHT21节点
    cJSON_AddItemToObject(item_lus, "value", cJSON_CreateNumber(lus));//添加value节点
    //Battery-total_voltage
	cJSON_AddItemToObject(item_volt, "time", cJSON_CreateString(now_time));//添加time节点
	cJSON_AddItemToObject(item_volt, "sensor_type", cJSON_CreateString("volt"));//添加SHT21节点
	cJSON_AddItemToObject(item_volt, "sensor_model", cJSON_CreateString("Battery"));//添加SHT21节点
    cJSON_AddItemToObject(item_volt, "value", cJSON_CreateNumber(battery.total_voltage));//添加value节点
	
	cJSON_AddItemToObject(item_cur, "time", cJSON_CreateString(now_time));//添加time节点
	cJSON_AddItemToObject(item_cur, "sensor_type", cJSON_CreateString("cur"));//添加SHT21节点
	cJSON_AddItemToObject(item_cur, "sensor_model", cJSON_CreateString("Battery"));//添加SHT21节点
    cJSON_AddItemToObject(item_cur, "value", cJSON_CreateNumber(battery.charge_current));//添加value节点
	
	cJSON_AddItemToObject(item_cap, "time", cJSON_CreateString(now_time));//添加time节点
	cJSON_AddItemToObject(item_cap, "sensor_type", cJSON_CreateString("cap"));//添加SHT21节点
	cJSON_AddItemToObject(item_cap, "sensor_model", cJSON_CreateString("Battery"));//添加SHT21节点
    cJSON_AddItemToObject(item_cap, "value", cJSON_CreateNumber(battery.remain_capacity));//添加value节点
	
	//合并 
	cJSON_AddItemToArray(item, item_temp);//root节点下添加slist节点
    cJSON_AddItemToArray(item, item_hum);
    cJSON_AddItemToArray(item, item_pres);
    cJSON_AddItemToArray(item, item_lus);
    cJSON_AddItemToArray(item, item_volt);
	cJSON_AddItemToArray(item, item_cur);
	cJSON_AddItemToArray(item, item_cap);
	output=cJSON_Print(root);
	
	printf("%s\n length:%d\n", output,strlen(output));
	sprintf(msg,"%s", output);
	
    cJSON_Delete(root);  
	cJsonFree(output);
}

//{"mt":"sdata","sid":"12","slist":[
//{"time":"2017-01-01 12:09:12","valid":0,"long":2236.9453,"D_long":"N","lati":11408.4790,"D_lati":"E","alti":11408.4790}]}
void Data_Packing_gps(char *msg)
{
	char * output;
	
	cJSON * root =  cJSON_CreateObject();
	cJSON * item =  cJSON_CreateObject();
		
	char now_time[30] = "2019-09-23";//一个宏定义/函数将数值变为时间 
	char  valid = 0;
	float longitude = 42.89057;
	char*  D_long = "N";
	float latitude = 130.97399;
	char*  D_lati = "E";
	float altitude = 230;

	calendar_get_time(&calendar);
	calendar_get_date(&calendar);
	sprintf(now_time,"%d-%d-%d %d:%d:%d",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);
	
	
	cJSON_AddItemToObject(root, "mt", cJSON_CreateString("sdata"));//根节点下添加
	cJSON_AddItemToObject(root, "sid", cJSON_CreateString("12"));
    cJSON_AddItemToObject(root, "slist", item);//root节点下添加semantic节点
    
	cJSON_AddItemToObject(item, "time", cJSON_CreateString(now_time));//添加time节点
    cJSON_AddItemToObject(item, "valid", cJSON_CreateBool(valid));//添加name节点
    cJSON_AddItemToObject(item, "Long", cJSON_CreateNumber(longitude));//添加long节点
    cJSON_AddItemToObject(item, "D_long", cJSON_CreateString(D_long));//添加D_long节点
    cJSON_AddItemToObject(item, "Lati", cJSON_CreateNumber(latitude));//添加long节点
    cJSON_AddItemToObject(item, "D_lati", cJSON_CreateString(D_lati));//添加D_long节点
    cJSON_AddItemToObject(item, "Alti", cJSON_CreateNumber(altitude));//添加long节点
	
	output=cJSON_Print(root);
	printf("%s\n length:%d\r\n", output,strlen(output));
	sprintf(msg,"%s", output);
    cJSON_Delete(root);  
	cJsonFree(output); 	
}






