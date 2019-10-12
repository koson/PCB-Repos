#include "max44009.h"
#include "myiic_max44009.h"

void I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{   

  u8 t;                                   
  
	IIC_MAX44009_Start(); //启动
  IIC_MAX44009_Send_Byte(dev_addr);    //从机地址（0：写）
  IIC_MAX44009_Wait_Ack();
 
  for(t=0;t<cnt;t++)
  {
    IIC_MAX44009_Send_Byte(reg_addr+t);    //发送control byte  
    IIC_MAX44009_Wait_Ack();                           
    IIC_MAX44009_Send_Byte(reg_data[t]); //发送字节                 
    IIC_MAX44009_Wait_Ack();  
  }   
  IIC_MAX44009_Stop();//产生一个停止条件 
}

void I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{       
  u8 t;  

	for(t=0;t<cnt;t++)
	{
		IIC_MAX44009_Start();                //启动读取
		IIC_MAX44009_Send_Byte(dev_addr);    //从机地址（0：写）
		IIC_MAX44009_Wait_Ack();
		IIC_MAX44009_Send_Byte(reg_addr+t);    //发送control byte  
		IIC_MAX44009_Wait_Ack(); //等待从机回复数据
		IIC_MAX44009_Start();                //启动读取
		IIC_MAX44009_Send_Byte(dev_addr+1);  //从机地址（1：读），进入接收模式
		IIC_MAX44009_Wait_Ack();
		reg_data[t]=IIC_MAX44009_Read_Byte(0);		//读取一个字节,不继续再读,发送NAK 
		IIC_MAX44009_Stop();                 //产生一个停止条件   
	}
	  
}

void MAX44009_WR_Reg(u8 addr,u8 val) 
{
	IIC_MAX44009_Start();  				 
	IIC_MAX44009_Send_Byte(MAX_ADDRESS);    //发送写器件指令	 
	IIC_MAX44009_Wait_Ack();	   
  IIC_MAX44009_Send_Byte(addr);   			//发送寄存器地址
	IIC_MAX44009_Wait_Ack(); 	 										  		   
	IIC_MAX44009_Send_Byte(val);     		//发送值					   
	IIC_MAX44009_Wait_Ack();  		    	   
  IIC_MAX44009_Stop();						//产生一个停止条件 	   
}

u8 MAX44009_RD_Reg(u8 addr) 		
{
	u8 temp=0;		 
	IIC_MAX44009_Start();  				 
	IIC_MAX44009_Send_Byte(MAX_ADDRESS);	//发送写器件指令	 
	temp=IIC_MAX44009_Wait_Ack();	   
  IIC_MAX44009_Send_Byte(addr);   		//发送寄存器地址
	temp=IIC_MAX44009_Wait_Ack(); 	 										  		   
	IIC_MAX44009_Start();  	 	   		//重新启动
	IIC_MAX44009_Send_Byte(MAX_ADDRESS+1);//发送读器件指令	 
	temp=IIC_MAX44009_Wait_Ack();	   
  temp=IIC_MAX44009_Read_Byte(0);		//读取一个字节,不继续再读,发送NAK 	    	   
  IIC_MAX44009_Stop();					//产生一个停止条件 	    
	return temp;				//返回读到的值
} 

void i2c_setRegister(uint8_t address, uint8_t registeraddress, uint8_t mask, uint8_t writevalue)
{
    uint8_t _setting;
    I2C_bus_read(address, registeraddress, &_setting, 1 );
    _setting     &= ~mask;
    _setting     |= (writevalue&mask);
    I2C_bus_write(address, registeraddress, &_setting,1);
};



void setEnabled(uint8_t enable)
{
    if (enable)
    {
        i2c_setRegister(MAX_ADDRESS, INT_ENABLE_REG, 1, 0);
        i2c_setRegister(MAX_ADDRESS, CONFIGURATION_REG, CONFIG_CONT_MASK, CONFIG_CONT_ON);
        i2c_setRegister(MAX_ADDRESS, CONFIGURATION_REG, CONFIG_MANUAL_MASK, CONFIG_MANUAL_OFF);
    }
    else
    {
        i2c_setRegister(MAX_ADDRESS, CONFIGURATION_REG, CONFIG_CONT_MASK, CONFIG_CONT_OFF);
    }
};

uint8_t max44009_initialize(void)
{
	IIC_MAX44009_Init();		//初始化IIC
    setEnabled(0);	//不使能中断检测
    return 1;
};

void getFluxMeasurement(uint32_t *mLux_value)
{
    uint8_t lux[2], lux_exponent;

    I2C_bus_read(MAX_ADDRESS, LUX_HIGH_REG, lux, 2);

    lux_exponent    = ((lux[0] >> 4) & 0x0F);
    lux[0]          = ((lux[0] << 4) & 0xF0);
    lux[1]         &= 0x0F;

    //lux_value    = 0.045 * ( lux_high + lux_low ) * (1<< lux_exponent);
    *mLux_value    = 45L * ( lux[0] | lux[1] ) * (1<< lux_exponent);
};
