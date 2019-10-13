#include "ad5245.h"

#define iic_scl_low() GPIO_ResetBits(GPIOB,GPIO_Pin_6)
#define iic_scl_high() GPIO_SetBits(GPIOB,GPIO_Pin_6)
#define iic_sdl_low() GPIO_ResetBits(GPIOB,GPIO_Pin_7)
#define iic_sdl_high() GPIO_SetBits(GPIOB,GPIO_Pin_7)

void delay_us(u16 cnt)
{
    u8 i;
    while(cnt--)
    {
        for(i=0;i<36;i++)
        {
            __nop();
        }
    }
}

void ad5245_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    iic_scl_low();
    iic_sdl_low();
//    iic_scl_high();
//    iic_sdl_high();
}
void iic_start(void)
{
    //启动
    iic_scl_high();
    iic_sdl_high();
    delay_us(2);//最小 1.3us
    iic_sdl_low();
    delay_us(1);//最小 0.6us
}
void iic_stop(void)
{
    //停止
    iic_scl_high();
    iic_sdl_low();
    delay_us(2);//最小 1.3us
    iic_sdl_high();
    delay_us(1);//最小 0.6us
}
void iic_ack(void)
{
    //应答
    iic_sdl_low();
    iic_scl_low();
    delay_us(2);//最小 1.3us
    iic_scl_high();
    delay_us(1);//最小 0.6us
    iic_scl_low();
}
void ad5245_writebyte(u8 data)
{
    u8 i;
    //写
    iic_scl_low();
    for(i=0;i<8;i++)
    {
        if(data&0x80)
        {
            iic_sdl_high();
        }else
        {
            iic_sdl_low();
        }
        data = data<<1;
//        iic_scl_low();
        delay_us(2);//最小 1.3us
        iic_scl_high();
        delay_us(1);//最小 0.6us
        iic_scl_low();
    }
}
void ad5245_write(u8 data)
{
    iic_start();
    ad5245_writebyte(0x58);//地址 0101 1000
    iic_ack();
    ad5245_writebyte(0x00);//指令 0000 0000
    iic_ack();
    ad5245_writebyte(data);//数据 
    iic_ack();
    iic_stop();
}
