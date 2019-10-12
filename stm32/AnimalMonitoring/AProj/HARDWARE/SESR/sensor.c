#include "sensor.h"
#include <stdio.h>
#include <string.h>
#include "cJSON.h"



//{"mt":"sdata","sid":"11","slist":[
//{"time":"2017-01-01 12:09:12","SHT21":"temp","value":"12.0"},
//{"time":"2017-01-01 12:09:12","SHT21":"hum","value":"50.5"},
//{"time":"2017-01-01 12:09:12","MS5611":" pres","value":"10000"},
//{"time":"2017-01-01 12:09:12","MAX44009":"lus","value":"100"},
//{"time":"2017-01-01 12:09:12","Battery":"volt","value":"12.3"},]}

void Data_Packing_11(char *msg)
{
	cJSON * root =  cJSON_CreateObject();
	cJSON * item =  cJSON_CreateObject();
	
    cJSON * item_temp =  cJSON_CreateObject();
    cJSON * item_hum =  cJSON_CreateObject();
    cJSON * item_pres =  cJSON_CreateObject();
    cJSON * item_lus =  cJSON_CreateObject();
    cJSON * item_volt =  cJSON_CreateObject();
	
	char now_time[20] = "2018-03-28 12:09:12";//一个宏定义/函数将数值变为时间 
	float temp = 23.0;
	float hum = 50.2;
	float pres = 10000;
	float lus = 100/3;
	float volt = 12.3;
	
	cJSON_AddItemToObject(root, "mt", cJSON_CreateString("sdata"));//根节点下添加
	cJSON_AddItemToObject(root, "sid", cJSON_CreateNumber(11));
	
	item = cJSON_CreateArray();    
    cJSON_AddItemToObject(root, "slist", item);//添加子树 
	 
    //SHT21-temp
    cJSON_AddItemToObject(item_temp, "time", cJSON_CreateString(now_time));//添加time节点
    cJSON_AddItemToObject(item_temp, "SHT21", cJSON_CreateString("temp"));//添加SHT21节点
    cJSON_AddItemToObject(item_temp, "value", cJSON_CreateNumber(temp));//添加value节点
    //SHT21-humi
    cJSON_AddItemToObject(item_hum, "time", cJSON_CreateString(now_time));//添加time节点
    cJSON_AddItemToObject(item_hum, "SHT21", cJSON_CreateString("hum"));//添加SHT21节点
    cJSON_AddItemToObject(item_hum, "value", cJSON_CreateNumber(hum));//添加value节点
	//MS5611-pres 
	cJSON_AddItemToObject(item_pres, "time", cJSON_CreateString(now_time));//添加time节点
    cJSON_AddItemToObject(item_pres, "MS5611", cJSON_CreateString("pres"));//添加SHT21节点
    cJSON_AddItemToObject(item_pres, "value", cJSON_CreateNumber(pres));//添加value节点
    //MAX44009-lus
	cJSON_AddItemToObject(item_lus, "time", cJSON_CreateString(now_time));//添加time节点
    cJSON_AddItemToObject(item_lus, "MAX44009", cJSON_CreateString("lus"));//添加SHT21节点
    cJSON_AddItemToObject(item_lus, "value", cJSON_CreateNumber(lus));//添加value节点
    //Battery-volt
	cJSON_AddItemToObject(item_volt, "time", cJSON_CreateString(now_time));//添加time节点
    cJSON_AddItemToObject(item_volt, "Battery", cJSON_CreateString("volt"));//添加SHT21节点
    cJSON_AddItemToObject(item_volt, "value", cJSON_CreateNumber(volt));//添加value节点
	//合并 
	cJSON_AddItemToArray(item, item_temp);//root节点下添加slist节点
    cJSON_AddItemToArray(item, item_hum);
    cJSON_AddItemToArray(item, item_pres);
    cJSON_AddItemToArray(item, item_lus);
    cJSON_AddItemToArray(item, item_volt);
	
	printf("%s\n length:%d\n", cJSON_Print(root),strlen(cJSON_Print(root)));
	sprintf(msg,"%s", cJSON_Print(root));
    cJSON_Delete(root);   
}

//{"mt":"sdata","sid":"12","slist":[
//{"time":"2017-01-01 12:09:12","valid":0,"long":2236.9453,"D_long":"N","lati":11408.4790,"D_lati":"E","alti":11408.4790}]}
void Data_Packing_12(char *msg)
{
	cJSON * root =  cJSON_CreateObject();
	cJSON * item =  cJSON_CreateObject();
		
	char now_time[20] = "2018-03-28 12:09:12";//一个宏定义/函数将数值变为时间 
	char  valid = 0;
	float longitude = 2236.9453;
	char*  D_long = "N";
	float latitude = 11408.4790;
	char*  D_lati = "E";
	float altitude = 10;
	
	
	cJSON_AddItemToObject(root, "mt", cJSON_CreateString("sdata"));//根节点下添加
	cJSON_AddItemToObject(root, "gid", cJSON_CreateNumber(12));
    cJSON_AddItemToObject(root, "slist", item);//root节点下添加semantic节点
    
	cJSON_AddItemToObject(item, "time", cJSON_CreateString(now_time));//添加time节点
    cJSON_AddItemToObject(item, "valid", cJSON_CreateBool(valid));//添加name节点
    cJSON_AddItemToObject(item, "Long", cJSON_CreateNumber(longitude));//添加long节点
    cJSON_AddItemToObject(item, "D_long", cJSON_CreateString(D_long));//添加D_long节点
    cJSON_AddItemToObject(item, "Lati", cJSON_CreateNumber(latitude));//添加long节点
    cJSON_AddItemToObject(item, "D_lati", cJSON_CreateString(D_lati));//添加D_long节点
    cJSON_AddItemToObject(item, "Alti", cJSON_CreateNumber(altitude));//添加long节点
	
	printf("%s\n length:%d\r\n", cJSON_Print(root),strlen(cJSON_Print(root)));
	sprintf(msg,"%s", cJSON_Print(root));
    cJSON_Delete(root);   
}






