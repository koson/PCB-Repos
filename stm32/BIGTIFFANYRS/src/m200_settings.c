#include "m200_settings.h"
#include <libmfflash.h>

m200_settings_t __xdata m200_settings =
{
    0,
    ONEMONTH,
    ATCERTAINDAY + 20,
    ONEMONTH,
    ATCERTAINDAY + 15,
    0,
    {{T1, 7, 0},{T2, 23, 0},{END, 0, 0},{END, 0, 0},{END, 0, 0}},
    {{T1, 7, 0},{T3, 10, 0},{T1, 17, 0},{T3, 21, 0},{T2, 23, 0},{END, 0, 0},{END, 0, 0},{END, 0, 0},{END, 0, 0},{END, 0, 0}}
};


void M200_ReadSettings()
{
    uint16_t __xdata *ptr = (uint16_t __xdata *)&m200_settings;

    if(flash_read(0xfd40) != 0xffff)
    {
        for(int i = 0; i < sizeof(m200_settings_t)/2 - 1; i++)
        {
            ptr[i] = flash_read(0xfd40 + i*2);
        }
    }
}

void M200_WriteSettings()
{

    uint16_t cal[50];
    uint16_t usr[128];
    uint16_t nbfi[32];
    for(int i = 0; i < 100; i += 2)
    {
        cal[i/2] = flash_read(0xfc00 + i);
    }
    for(int i = 0; i < 64; i += 2)
    {
        usr[i/2] = flash_read(0xfd00 + i);
    }
    for(int i = 0; i < 64; i += 2)
    {
        nbfi[i/2] = flash_read(0xfe00 + i);
    }
    flash_unlock();
    flash_pageerase(0xfc00);

    for(int i = 0; i < 100; i += 2)
    {
        flash_write(0xfc00 + i, cal[i/2]);
    }

    for(int i = 0; i < 64; i += 2)
    {
        flash_write(0xfe00 + i, nbfi[i/2]);
    }

    uint16_t __xdata *ptr = (uint16_t __xdata *)&m200_settings;

    for(int i = 0; i < sizeof(m200_settings_t)/2; i++) usr[32+i] = ptr[i];

    for(int i = 0; i < 64 + sizeof(m200_settings_t); i += 2)
    {
        flash_write(0xfd00 + i, usr[i/2]);
    }

    flash_lock();
}
