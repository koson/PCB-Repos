#include <stdio.h>
#include <string.h>
#include "cJSON.h"
#include "stdlib.h"
#include "cjsontest.h"
#include "mqtt.h"

//#define MQTT_RECEIVEBUFF_MAXLENGTH		2048
//extern uint8 MQTT_Receivebuf[MQTT_RECEIVEBUFF_MAXLENGTH];

#define MQTT_SEND_BUFF_MAXLENGTH		12000
extern uint8 MQTT_SEND_buf[MQTT_SEND_BUFF_MAXLENGTH];

#define MQTT_DATA_BUFF_MAXLENGTH		12000
extern uint8 MQTT_DATA_buf[MQTT_DATA_BUFF_MAXLENGTH];
// char* c_sdataresp="{\"mt\":\"sdataresp\",\"sid\":21}" ;

// char* c_tresp="{\"mt\":\"tresp\",\"t_id\":21,\"tlist\":[\
// {\"taskperiod\":3600,\"taskname\":\"comm_heartbeat\"},\
// {\"taskperiod\":3600,\"taskname\":\"take_pic\"},\
// {\"taskperiod\":3600,\"taskname\":\"get_sensor\"}]}" ;


// cjson_task_req(Tmessage,11);	
// cjson_task_sdata(Tmessage,12);

// cjson_parse_sdataresp(c_sdataresp);
// cjson_parse_tresp(c_tresp);

//{"mt":"sdata","sid":"11","slist":[
//{"time":"2017-01-01 12:09:12","SHT21":"temp","value":"12.0"},
//{"time":"2017-01-01 12:09:12","SHT21":"hum","value":"50.5"},
//{"time":"2017-01-01 12:09:12","MS5611":" pres","value":"10000"},
//{"time":"2017-01-01 12:09:12","MAX44009":"lus","value":"100"},
//{"time":"2017-01-01 12:09:12","Battery":"volt","value":"12.3"},]}

void cjson_task_req(char *msg, int id)
{
	cJSON * root =  cJSON_CreateObject();
	
	cJSON_AddItemToObject(root, "mt", cJSON_CreateString("treq"));//根节点下添加
	cJSON_AddItemToObject(root, "t_id", cJSON_CreateString("11"));
	//cJSON_AddItemToObject(root, "t_id", cJSON_CreateNumber(id));
		
	sprintf(msg,"%s", cJSON_Print(root));
	
	send_data(MQTT_SEND_buf,MQTT_SEND_BUFF_MAXLENGTH,msg);
	printf("%s",msg);

    cJSON_Delete(root);   
}


void cjson_task_sdata(char *msg, int id)
{
	cJSON * root =  cJSON_CreateObject();
	cJSON * item =  cJSON_CreateObject();
	
    cJSON * item_temp =  cJSON_CreateObject();
    cJSON * item_hum =  cJSON_CreateObject();
    cJSON * item_volt =  cJSON_CreateObject();
	
	char now_time[20] = "2018-9-2 10:09:12";//一个宏定义/函数将数值变为时间 
	float temp = 23.0;
	float hum = 50.2;
	float volt = 12.3;
	
	cJSON_AddItemToObject(root, "mt", cJSON_CreateString("sdata"));//根节点下添加
	cJSON_AddItemToObject(root, "sid", cJSON_CreateNumber(id));
	
	item = cJSON_CreateArray();    
    cJSON_AddItemToObject(root, "slist", item);//添加子树 
	 
    // SHT21-temp
    cJSON_AddItemToObject(item_temp, "time", cJSON_CreateString(now_time));//添加time节点
    cJSON_AddItemToObject(item_temp, "SHT21", cJSON_CreateString("temp"));//添加SHT21节点
    cJSON_AddItemToObject(item_temp, "value", cJSON_CreateNumber(temp));//添加value节点
    // SHT21-humi
    cJSON_AddItemToObject(item_hum, "time", cJSON_CreateString(now_time));//添加time节点
    cJSON_AddItemToObject(item_hum, "SHT21", cJSON_CreateString("hum"));//添加SHT21节点
    cJSON_AddItemToObject(item_hum, "value", cJSON_CreateNumber(hum));//添加value节点
	// Battery-volt
	cJSON_AddItemToObject(item_volt, "time", cJSON_CreateString(now_time));//添加time节点
    cJSON_AddItemToObject(item_volt, "Battery", cJSON_CreateString("volt"));//添加SHT21节点
    cJSON_AddItemToObject(item_volt, "value", cJSON_CreateNumber(volt));//添加value节点
	// 合并 
	cJSON_AddItemToArray(item, item_temp);//root节点下添加slist节点
    cJSON_AddItemToArray(item, item_hum);
    cJSON_AddItemToArray(item, item_volt);
	
	sprintf(msg,"%s", cJSON_Print(root));
	send_data(MQTT_SEND_buf,MQTT_SEND_BUFF_MAXLENGTH,msg);
	printf("%s",msg);

    cJSON_Delete(root);
	cJSON_Delete(item);     
	
	cJSON_Delete(item_temp);   
	cJSON_Delete(item_hum);   
	cJSON_Delete(item_volt);   
}

void cjson_parse_sdataresp(char *msg)
{
    cJSON *root;
    cJSON *mt, *sid;
    root = cJSON_Parse(msg);   
    if(!root)
    {
        printf("cJSON_Parse error!!\n");
        return;
    }

    mt = cJSON_GetObjectItem(root, "mt");
    if(mt)
    {																										
        printf("mt value : %s\n", mt->valuestring);
    }


    sid = cJSON_GetObjectItem(root, "sid");
    if(sid)
    {
        printf("tid value : %d\n", sid->valueint);
    }
    
    // 释放根节点 
    cJSON_Delete(root); 
	cJSON_Delete(mt);   
	cJSON_Delete(sid);    
}

void cjson_parse_tresp(char *msg)
{
    cJSON *root;
    cJSON *mt, *t_id, *tlist;
    cJSON *itemnode, *taskperiod, *taskname;

    root = cJSON_Parse(msg);   
    if(!root)
    {
        printf("cJSON_Parse error!!\n");
        return;
    }

    mt = cJSON_GetObjectItem(root, "mt");
    if(mt)
    {																										
        printf("mt value : %s\n", mt->valuestring);
    }


    t_id = cJSON_GetObjectItem(root, "t_id");
    if(t_id)
    {
        printf("t_id value : %d\n", t_id->valueint);
    }

    tlist = cJSON_GetObjectItem(root, "tlist");
    if(tlist)
    {
    	int i = 0;
        int size = cJSON_GetArraySize(tlist);
        printf("Get tlist size : %d\n", size);
 
        for(i=0; i<size; i++)
        {
            itemnode = cJSON_GetArrayItem(tlist, i);
            //printJson(tlistnode);
          	if(itemnode)
          	{                
                taskperiod = cJSON_GetObjectItem(itemnode,"taskperiod");
                if(taskperiod)
                {            
                    printf("Get taskperiod : %d\n", taskperiod->valueint);
                }

                taskname = cJSON_GetObjectItem(itemnode,"taskname");
                if(taskname)
                {            
                    printf("Get taskname : %s\n", taskname->valuestring);
                }
			}
        }
    }
	// 释放根节点 
    cJSON_Delete(root); 

	cJSON_Delete(mt); 
	cJSON_Delete(t_id); 
	cJSON_Delete(tlist); 

	cJSON_Delete(itemnode); 
	cJSON_Delete(taskperiod); 
	cJSON_Delete(taskname); 
}
