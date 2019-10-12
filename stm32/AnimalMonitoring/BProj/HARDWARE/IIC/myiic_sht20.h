#ifndef __MYIIC_SHT20_H
#define __MYIIC_SHT20_H
#include "sys.h"

   	   		   
//IO方向设置
#define SHT20_SDA_IN()  {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=0<<7*2;}	//PB7输入模式
#define SHT20_SDA_OUT() {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=1<<7*2;} //PB7输出模式
//IO操作函数	 
#define SHT20_IIC_SCL    PBout(6) //SCL
#define SHT20_IIC_SDA    PBout(7) //SDA	 
#define SHT20_READ_SDA   PBin(7)  //输入SDA 

//IIC所有操作函数
void IIC_SHT20_Init(void);                //初始化IIC的IO口				 
void IIC_SHT20_Start(void);				//发送IIC开始信号
void IIC_SHT20_Stop(void);	  			//发送IIC停止信号
void IIC_SHT20_Send_Byte(u8 txd);			//IIC发送一个字节
u8 IIC_SHT20_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 IIC_SHT20_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_SHT20_Ack(void);					//IIC发送ACK信号
void IIC_SHT20_NAck(void);				//IIC不发送ACK信号

void IIC_SHT20_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_SHT20_Read_One_Byte(u8 daddr,u8 addr);	  
#endif
















