#ifndef _M200_SETTINGS_H
#define _M200_SETTINGS__H
#include <libmftypes.h>

typedef enum
{
    UNDEFINED   =   0,
    ONEHOUR     =   360,
    TWOHOURS    =   720,
    FOURHOURS   =   1440,
    SIXHOURS    =   2160,
    TWELVEHOURS =   4320,
    ONEDAY      =   8640,
    TWODAYS     =   17280,
    FOURDAYS    =   34560,
    ONEWEEK     =   60480,
    TWOWEEKS    =   120960,
    THREEWEEKS  =   181440,
    FOURWEEKS   =   241920,
    ONEMONTH    =   267840,
    TWOMONTHS   =   535680,
    THREEMONTHS =   803520,
    PERIOD_OFF  =   0xffffffff
}send_period_t;

typedef enum
{
    ATONCE = 0,
    ATENDOFHOUR = 1,
    ATENDOFDAY = 2,
    ATENDOFWEEK = 3,
    ATENDOFMONTH = 4,
    ATCERTAINDAY = 0x80,
    MODE_OFF = 0xFF
}read_mode_t;


typedef enum
{
    T1 = 0,
    T2 = 1,
    T3 = 2,
    T4 = 3,
    END = 0xff
} tarif_type;

typedef struct
{
    tarif_type tarif;
    uint8_t hour;
    uint8_t minute;
} TARIF_ZONE;

typedef struct
{
    uint32_t    rs_add;
    send_period_t    regular_period;
    read_mode_t read_mode;
    send_period_t    regular_period_aux;
    read_mode_t read_mode_aux;
    uint8_t  t_mode;
    TARIF_ZONE twotarifs[5];
    TARIF_ZONE threetarifs[10];
    uint8_t     reserved[16];
}m200_settings_t;


extern m200_settings_t __xdata m200_settings;

void M200_ReadSettings();
void M200_WriteSettings();


#endif
