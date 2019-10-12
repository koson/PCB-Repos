#ifndef NBFI_CONFIG_H
#define NBFI_CONFIG_H
#include "defines.h"


#if BAND_ID == UL868800_DL446000
#define NBFI_UL_FREQ_BASE       (868800000 - 25000)
#define NBFI_DL_FREQ_BASE       446000000
#elif BAND_ID == UL868800_DL864000
#define NBFI_UL_FREQ_BASE       (868800000 - 25000)
#define NBFI_DL_FREQ_BASE       864000000
#elif BAND_ID == UL868800_DL446000_DL864000
#define NBFI_UL_FREQ_BASE       (868800000 - 25000)
#define NBFI_DL_FREQ_BASE       864000000
#elif BAND_ID == UL867950_DL446000
#define NBFI_UL_FREQ_BASE       (867950000 - 25000)
#define NBFI_DL_FREQ_BASE       446000000
#elif BAND_ID == UL868500_DL446000
#define NBFI_UL_FREQ_BASE       (868500000 - 25000)
#define NBFI_DL_FREQ_BASE       446000000
#elif BAND_ID == UL868100_DL446000
#define NBFI_UL_FREQ_BASE       (868100000 - 25000)
#define NBFI_DL_FREQ_BASE       446000000
#elif UL868500_DL864000
#define NBFI_UL_FREQ_BASE       (868500000 - 25000)
#define NBFI_DL_FREQ_BASE       864000000
#endif // defined

//#define NBFI_FASTDL_FREQ_BASE   445000000

#define READ_PARAM_CMD              0x00
#define WRITE_PARAM_CMD             0x01
#define WRITE_PARAM_WITH_ACK_CMD    0x02
#define WRITE_PARAM_AND_SAVE_CMD    0x03


//#define RATE_CHANGE_PARAM_CMD   0x02


#define NBFI_PARAM_MODE         0x00
#define NBFI_PARAM_HANDSHAKE    0x01
#define NBFI_PARAM_MAXLEN       0x02
#define NBFI_PARAM_TXFREQ       0x03
#define NBFI_PARAM_RXFREQ       0x04
#define NBFI_PARAM_ANT          0x05
#define NBFI_PARAM_DL_ADD       0x06
#define NBFI_PARAM_HEART_BEAT   0x07
#define NBFI_PARAM_TX_BRATES    0x08
#define NBFI_PARAM_RX_BRATES    0x09
#define NBFI_PARAM_VERSION      0x0A
#define NBFI_ADD_FLAGS          0x0B
#define NBFI_QUALITY            0x0C
#define NBFI_UL_BASE_FREQ       0x0D
#define NBFI_DL_BASE_FREQ       0x0E
#define NBFI_QUALITY_EX         0x0F

#define SEND_INFO_INTERVAL      77760000  //one time per month
#define MAKE_OSCCAL_INTERVAL       30




typedef struct
{
    nbfi_mode_t mode;
    nbfi_phy_channel_t tx_phy_channel;
    nbfi_phy_channel_t rx_phy_channel;
    nbfi_handshake_t   handshake_mode;
    nbfi_mack_mode_t   mack_mode;
    uint8_t     num_of_retries;
    uint8_t     max_payload_len;
    uint8_t     dl_ID[3];
    uint8_t     temp_ID[3];
    uint8_t     broadcast_ID[3];
    uint8_t     full_ID[6];
    uint32_t    tx_freq;
    uint32_t    rx_freq;
    uint8_t     tx_antenna;
    uint8_t     rx_antenna;
    int8_t      tx_pwr;
    uint16_t    heartbeat_interval;
    uint8_t     heartbeat_num;
    uint8_t     additional_flags;
    uint32_t    ul_freq_base;
    uint32_t    dl_freq_base;
    uint8_t     reserved[3];
}nbfi_settings_t;


#define NBFI_FLG_FIXED_BAUD_RATE        0x01
#define NBFI_FLG_NO_RESET_TO_DEFAULTS   0x02
#define NBFI_FLG_NO_SENDINFO            0x04
#define NBFI_FLG_NO_XTEA                0x08
#define NBFI_FLG_DO_OSCCAL              0x10



#define RX_CONF 0x01
#define TX_CONF 0x02

typedef union
{
        struct
        {
            uint8_t Reserved         : 6;//LSB
            uint8_t DL_SPEED_NOT_MAX : 1;
            uint8_t UL_SPEED_NOT_MAX : 1;
        };
        uint8_t info;
}NBFi_station_info_s;


typedef enum
{
    DOWN = 0,     // data rate change down direction
    UP = 1        // data rate change up direction
}nbfi_rate_direct_t;

extern nbfi_settings_t __xdata nbfi;
extern NBFi_station_info_s nbfi_station_info;

extern bool nbfi_settings_changed;

extern uint8_t __xdata you_should_dl_power_step_up;
extern uint8_t __xdata you_should_dl_power_step_down;
extern uint8_t __xdata current_tx_rate;
extern uint8_t __xdata current_rx_rate;
extern struct wtimer_desc __xdata nbfi_send_mode_delay;
void NBFI_Config_Check_State();

bool NBFi_Config_Parser(uint8_t __xdata* buf);
bool NBFi_Config_Rate_Change(uint8_t rx_tx, nbfi_rate_direct_t dir );
void NBFi_Config_Return();
bool NBFi_Config_Send_Mode(bool, uint8_t);
void NBFi_Config_Set_Default();
bool NBFi_Config_Tx_Power_Change(nbfi_rate_direct_t dir);
void NBFi_Config_Set_FastDl(bool);
void NBFi_Go_To_Sleep(uint8_t);
void NBFi_ReadConfig();
void NBFi_WriteConfig();
void NBFi_Config_Set_TX_Chan(nbfi_phy_channel_t ch);
void NBFi_Config_Set_RX_Chan(nbfi_phy_channel_t ch);
void NBFi_Config_Send_Current_Mode(struct wtimer_desc __xdata *desc);
bool NBFi_Is_Mode_Normal();
#endif // NBFI_CONFIG_H
