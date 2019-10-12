#include "fattester.h"	 
#include "sdio_sdcard.h"
#include "usmart.h"
#include "usart.h"
#include "exfuns.h"
#include "malloc.h"		  
#include "ff.h"
#include "string.h"

#include "malloc.h" 
#include "stmflash.h"
#include "draft.h"

#include "mqtt.h"

#include "power.h"
#include "delay.h"
#include "usbh_usr.h" 
#include "usbh_core.h"
#include "timer.h"

///**
//* 见数据由U盘读取到SD卡中，原始地址和目的地址，名字不改变
// * @param  folder_s 文件夹源地址
// * @param  folder_d 文件夹目的地址
// * @return 文件夹移动的文件夹数目
// */
//u8 dump_file_USB(TCHAR* folder_s, TCHAR* folder_d)
//{	
//	u8 *buffer=0;					
//	u8 sramx=0;  //默认为内部sram
//	
//	FIL fl_in,fl_out;  //文件读取测试
//	DIR dir;  //定义返回文件目录
//	FRESULT res;  // 返回读取结果
//	
//	char *fn;  // 文件名
//	char path[50];  // 当前文件的地址
//	char dn_path[50];  // SD卡中目的地址
//	char labelnum[20];  // USB中改变前的地址
//	char chg_path[50];  // USB中改变后的地址
//	
//	unsigned int r,w;
//	int file_length;  //文件大小
//	int read_cnt,read_residue;  //读取次数，剩余大小
//	
//	u32 pic_now;  // 当前的图片的命名ID，具有唯一性，仅当接收到新图片后改变
//	u16 send_num = 0;  // 成功发送的数量

//	// 读取图片ID
//	pic_now = STMFLASH_Read_Num(FLASH_SAVE_ADDR);
//	printf("Read Picture ID:pic_now = %d\r\n", pic_now);  //打印路径	
//	// 申请内存
//	buffer=mymalloc(sramx,SDCARD_BUFFER);  // 申请移动文件的缓存
//	if(buffer == NULL)
//	{
//		printf("Fail to mymalloc\r\n");
//		return 0;
//	}
//	// 创建SD卡目标目录
//	if(f_opendir(&dir,folder_d) != FR_OK)	
//	{
//		printf("f_mkdir:%s,err1:%d\r\n", folder_d, f_mkdir(folder_d));  
//	}
//	
//	// 创建USB移动目录
//	res = f_opendir(&dir,USB_FILE_DIR_CR);
//	if( res != FR_OK) 
//	{
//		printf("f_mkdir:%s,err2:%d,err:%d\r\n", USB_FILE_DIR_CR, res, f_mkdir(USB_FILE_DIR_CR));  // 创建目录
//	}
//	// 打开目录
//	res = f_opendir(&dir,folder_s);  
//	// 成功打开目录
//	if (res == FR_OK)  
//	{
//		while(1)
//		{
//			// 读取文件夹信息
//	        res = f_readdir(&dir, &fileinfo);                   //读取目录下的一个文件
//	        if (res != FR_OK || fileinfo.fname[0] == 0) break;  //错误了/到末尾了,退出
//   
//        	fn = fileinfo.fname;
//        	// 获得当前文件地址
// 			strcpy(path,folder_s);  
// 			strcat(path,"/");
//			strcat(path,fn);
//			printf("\r\n%s\r\n", path);  //打印路径	
//			
//			// 获得当前图片的标号
//			sprintf(labelnum, "IMAG%05d.JPG", pic_now);
//			
//			// 构造SD卡中目的地址
//			strcpy(dn_path, folder_d); 
//			strcat(dn_path, "/"); 
//			strcat(dn_path, labelnum);
//			printf("%s\r\n", dn_path);  //打印路径		

//			// U盘中当图片移动成功后，需要改变图片的位置
//			strcpy(chg_path, USB_FILE_DIR_C);  
//			strcat(chg_path, "/");
//			strcat(chg_path, labelnum);
//			printf("%s\r\n", chg_path);  //打印路径	

//			// 打开U盘中文件
//			res = f_open(&fl_in,path,FA_OPEN_EXISTING|FA_READ);
//			if(res != FR_OK)
//			{
//				printf("Fail to open file:%s\r\n",path);
//				continue;
//			}
//					
//			//创建文件
//			res = f_open(&fl_out,dn_path,FA_CREATE_ALWAYS|FA_WRITE);
//			if(res != FR_OK)
//			{
//				printf("Fail to creat file:%s,err:%d\r\n",dn_path,res);
//				continue;
//			}
//			
//			//获得文件大小属性
//			file_length = f_size(&fl_in);
//			//printf("Length %s:%d\r\n", path, file_length);
//			read_cnt = file_length / SDCARD_BUFFER;  // 读取的次数
//			read_residue = file_length % SDCARD_BUFFER;  // 读取的余数，作为最后一次发送
//			//  读取并转存文件
//			while(read_cnt--){
//				f_read(&fl_in,buffer,SDCARD_BUFFER,&r);
//				if(r!=SDCARD_BUFFER){printf("Read ERROR %d\r\n",read_cnt);break;}
//				f_write(&fl_out,buffer,SDCARD_BUFFER,&w);
//				if(w!=SDCARD_BUFFER){printf("WRITE ERROR %d\r\n",read_cnt);break;}
//			}
//			if(read_residue)
//			{
//				f_read(&fl_in,buffer,read_residue,&r);
//				if(r!=read_residue){printf("Read ERROR\r\n");}
//				f_write(&fl_out,buffer,read_residue,&w);
//				if(w!=read_residue){printf("WRITE ERROR\r\n");}
//			}

//			// 关闭文件
//			f_close(&fl_in);
//			f_close(&fl_out);
//			
//			// 移动文件
//			res = f_rename(path,chg_path);
//			
//			if(res != FR_OK)
//			{
//				printf("Fail to rename file:%s\r\n%s\r\nERROR:%d\r\n",path, chg_path, res);
//				continue;
//			}else
//			{
//				printf("rename file:%s\r\n%s\r\nERROR:%d\r\n",path, chg_path, res);
//			}

//			send_num++;  // 已发送图片数量
//			pic_now++; //当前的ID
//	
//			// 图片编号加一，并写入Flash中
//			STMFLASH_Write_Num(FLASH_SAVE_ADDR, pic_now);
//		} 
//    }
//    else{
//    	printf("Can't open dir:%s\r\n",USB_FILE_DIR);
//    }	  
//	
//    // 释放内存
//	myfree(sramx,buffer);
//	printf("Send complete!!!\r\n");
//	return send_num;
//}

//// 能够自动提取字符串中的数字
//u32 srt2integer_pro(char* str)
//{
//	u8 len=0;
//	u8 i=0;
//	u8 buf[20]={0};
//	u32 wi=1;
//	u32 sum=0;

//	while(str[i])
//	{
//		if(str[i]>='0' && str[i]<='9')
//		{
//			buf[len] = str[i];
//			//printf("%c",buf[len]);
//			len++;
//		}
//		i++;
//	}

//	for(i=0; i<len; i++)
//	{
//		sum =sum + (buf[len-1-i]-'0') *wi;
//		wi*=10;
//	}
//	
//	return sum;
//}



///** 
// * @brief read file of assigned path(folder)
// * @param path		 assigned path
// * @param nameArry   filename
// *
// * @return num of files
// * @note  max 20
// */
//u8 sd_scan_pictures(TCHAR* path, TCHAR* nameArry[])
//{
//	char cnt=0;
//	FRESULT res;	  
//    char *fn;   /* This function is assuming non-Unicode cfg. */
//	// _USE_LFN
// 	fileinfo.lfsize = _MAX_LFN * 2 + 1;
//	fileinfo.lfname = mymalloc(SRAMIN,fileinfo.lfsize);

//    res = f_opendir(&dir,(const TCHAR*)path); // open a folder
//    if (res == FR_OK) 
//	{	
//		printf("\r\n"); 
//		while(1)
//		{
//	        res = f_readdir(&dir, &fileinfo);                   // read next file of the folder.
//	        if (res != FR_OK || fileinfo.fname[0] == 0 || cnt>10) break;  // end of the foleder or wrong occurs
//        	fn = *fileinfo.lfname ? fileinfo.lfname : fileinfo.fname;
// 			strcpy(nameArry[cnt],path);  
// 			strcat(nameArry[cnt],"/");
//			strcat(nameArry[cnt],fn);			
////			printf("%s/", path);  // printf file path 
////			printf("%s\r\n",  fn);// print file name
//			cnt++;
//		} 
//    }	  
//	myfree(SRAMIN,fileinfo.lfname);
//    return cnt;	  
//}

//u8 mqtt_send_allpic(char* folder_s)
//{
//	DIR dir;  //定义返回文件目录
//	FRESULT res;  // 返回读取结果
//	MY_MQTT_ERR err;
//	char *fn;  // 文件名
//	char path[50];  // 当前文件的地址
//	char chg_path[50];  // USB中改变后的地址
//	
//	u16  picture_id;  // 获取图片数据
//	u16 send_num = 0;  // 成功发送的数量

//	fileinfo.lfsize = _MAX_LFN * 2 + 1;
//	fileinfo.lfname = mymalloc(SRAMIN,fileinfo.lfsize);
//	
//	// 创建目录
//	//if(f_opendir(&dir,SD_FILE_DIR_CR) != FR_OK) printf("res2:%d\r\n",f_mkdir(SD_FILE_DIR_CR));  
//				// 创建SD卡目标目录
//	res = f_opendir(&dir,SD_FILE_DIR_CR);
//	if( res != FR_OK) 
//	{
//		printf("f_mkdir:%s,err1:%d,err2:%d\r\n", SD_FILE_DIR_CR, res, f_mkdir(SD_FILE_DIR_CR));  // 创建目录
//	}	
//	// 打开目录
//	res = f_opendir(&dir,folder_s);  
//	if (res == FR_OK)  // 成功打开目录
//	{
//		while(1)
//		{
//			// 读取文件夹信息
//			res = f_readdir(&dir, &fileinfo);                   //读取目录下的一个文件
//	        if (res != FR_OK || fileinfo.fname[0] == 0) break;  //错误了/到末尾了,退出
//   
//        	fn = *fileinfo.lfname ? fileinfo.lfname : fileinfo.fname;
//			
//			picture_id = srt2integer_pro(fn);
//        	// 获得当前文件地址
// 			strcpy(path,folder_s);  
// 			strcat(path,"/");
//			strcat(path,fn);
//			printf("\r\n%s\r\n", path);  // 打印路径	
//			
//			// U盘中当图片移动成功后，需要改变图片的位置
//			strcpy(chg_path, SD_FILE_DIR_C);  
//			strcat(chg_path, "/");
//			strcat(chg_path, fn);
//			printf("%s\r\n", chg_path);  //打印路径	

//			//err=MY_MQTT_ERR_NONE;
//			printf("\r\nmysend_picture:{\r\n*%s,\r\n*%s,\r\n*picture_id:%d}\r\n",path, chg_path, picture_id);
//			err = mysend_picture(path, chg_path, picture_id);
//			if(err != MY_MQTT_ERR_NONE) {printf("mqtt mysend_picture err:%d\r\n", err);continue;}
//			
//			// 移动文件
//			res = f_rename(path,chg_path);
//			if(res != FR_OK)
//			{
//				printf("Fail to rename file:%s\r\n%s\r\nERROR:%d\r\n",path, chg_path, res);
//				continue;
//			}else
//			{
//				printf("rename file:%s\r\n%s\r\nERROR:%d\r\n",path, chg_path, res);
//			}				
//			
//			send_num++;
//		}
//	}else{
//    	printf("Can't open dir:%s\r\n",USB_FILE_DIR);
//    }	  
//	
//	printf("PIC Send complete!!!\r\n");
//	
//	myfree(SRAMIN,fileinfo.lfname);
//	return send_num;
//}

//// 实现家相机中的数据转移SD卡中
//u8 mydump_file(void)
//{
//	u8 cnt;
//	OS_ERR err;
//	
//	printf("mydump_file:{\r\n  %s\r\n  %s}\r\n",USB_FILE_DIR,SD_FILE_DIR);
//	OSSchedLock(&err);						//UCOSIII的方式,禁止调度，
//	cnt = dump_file_USB(USB_FILE_DIR, SD_FILE_DIR);
//	OSSchedUnlock(&err);					//UCOSIII的方式,恢复调度
//	printf("success mydump_file %d pictures\r\n",cnt);
//	return cnt;
//}

//void openUSB(void)
//{
//	USB_CONNECT_ON();
//	delay_ms(100);
//	USB_Connecting(1);
//	delay_ms(10);
//}

//void closeUSB(void)
//{
//	delay_ms(10);
//	USB_Connecting(0);
//	//delay_ms(100);
//	//USB_CONNECT_OFF();
//}

////等待一个设置过的命令出现，失败超时返回0，成功返回1
//u8 WaitDisconnectflag(unsigned short timeout)
//{
//	OS_ERR err;
//	static uint16 time=0;
//	while(usb_state_get())
//	{
//		OSTimeDly(1,OS_OPT_TIME_DLY,&err);
//		time+=1;
//		if(time>=timeout)
//		{
//			time=0;
//			return 0;
//		}
//	}
//	time=0;
//	return 1;
//}

////等待一个设置过的命令出现，失败超时返回0，成功返回1
//u8 WaitDistinguishflag(unsigned short timeout)
//{
//	OS_ERR err;
//	static uint16 time1=0;
//	while(!usb_cflag_get())
//	{
//		OSTimeDly(1,OS_OPT_TIME_DLY,&err);
//		time1+=1;
//		if(time1>=timeout)
//		{
//			time1=0;
//			return 0;
//		}
//	}
//	time1=0;
//	return 1;
//}
//// 实现将SD卡中的数据全部发送到云端
//u8 mysend_allpic(void)
//{
//	u8 cnt;
//	OS_ERR err;
//	printf("mysend_allpic:%s\r\n",SD_FILE_DIR);
//	OSSchedLock(&err);						//UCOSIII的方式,禁止调度，
//	cnt = mqtt_send_allpic(SD_FILE_DIR);
//	OSSchedUnlock(&err);					//UCOSIII的方式,恢复调度
//	printf("success mysend_allpic %d pictures\r\n",cnt);
//	return cnt;
//}
