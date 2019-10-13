/******************** (C) COPYRIGHT 2017 CETC27-CETIIC-ZYGD ********************
* File Name          : filemap.c
* Author             : WZJ
* Version            : V1.0
* Date               : 2017/05/12
* Description        :
*******************************************************************************/
#include "save.h"
#include "spi_flash.h"

#define PAGE_NUM    3

u8 Data_Buf[PAGE_NUM][256];
u32 BaseAdress, WR_Adress;


void Read_Flash2Buffer(void)
{
    u8 i;
    BaseAdress = 0x7F0000;
    for (i = 0; i < PAGE_NUM; i++)
    {
        BaseAdress += 0x100;
        SPI_FLASH_BufferRead(Data_Buf[i], BaseAdress, 0x100);
    }
}
void Write_Buffer2Flash(void)
{
    u8 i;
    BaseAdress = 0x7F0000;
    for (i = 0; i < PAGE_NUM; i++)
    {
        BaseAdress += 0x100;
        SPI_FLASH_PageWrite(Data_Buf[i], BaseAdress, 0x100);
    }
}
/******************** (C) COPYRIGHT 2017 CETC27-CETIIC-ZYGD ********************/
