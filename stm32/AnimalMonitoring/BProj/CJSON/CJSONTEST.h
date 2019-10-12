#ifndef _CJSONTEST_H_
#define _CJSONTEST_H_

void cjson_task_req(char *msg, int id);  // 请求任务 
void cjson_task_sdata(char *msg, int id);  // 发送传感器数据 

void cjson_parse_sdataresp(char *msg);  // 解析服务器确认数据 
void cjson_parse_tresp(char *msg);  // 解析任务列表 

#endif
