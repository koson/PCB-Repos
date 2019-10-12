#ifndef NBFI_H
#define NBFI_H
#include "defines.h"
#include <ax8052f143.h>
#include <libmftypes.h>
#include <libmfradio.h>
#include <libmfwtimer.h>
#include "easyax5043.h"
#include "axradio.h"

#include "misc.h"

typedef const uint8_t __code* full_id_ptr_t;
#define NBFI_FLASH_SETTINGS ((full_id_ptr_t)0xfd00)
#define FULL_ID     NBFI_FLASH_SETTINGS


#define NBFI_TX_PKTBUF_SIZE     64
#define NBFI_RX_PKTBUF_SIZE     32


typedef struct
{
    uint8_t     temp_ID[4];
    uint8_t     xtea_key[32];
    uint8_t     full_ID[6];
    uint8_t     hardware_type;
    uint8_t     hardware_options;
    //...
}nbfi_flash_settings_t;

typedef enum
{
    NRX         =   0,
    DRX         =   1,
    CRX         =   2,
    TRANSPARENT =   3,
}nbfi_mode_t;

typedef enum
{
    MACK_0      = 0,//no ack
    MACK_1      = 1,//0x1,
    MACK_2      = 2,//0x3,
    MACK_4      = 4,//0x0f,
    MACK_8      = 8,//0xff,
    MACK_16     = 16,//0xffff,
    MACK_32     = 32,//0,
}nbfi_mack_mode_t;



typedef enum
{
    DL_PSK_200              = 0,
    DL_PSK_500              = 1,
    DL_PSK_5000             = 2,
    DL_PSK_FASTDL           = 3,
    UL_DBPSK_50_PROT_C      = 20,
    UL_DBPSK_50_PROT_D      = 21,
    UL_PSK_200              = 22,
    UL_DBPSK_400_PROT_C     = 23,
    UL_DBPSK_400_PROT_D     = 24,
    UL_PSK_500              = 25,
    UL_DBPSK_3200_PROT_D    = 26,
    UL_PSK_5000             = 27,
    UL_DBPSK_25600_PROT_D   = 28,
    UL_PSK_FASTDL           = 29,
    UL_CARRIER              = 50,
    OSC_CAL                 = 51
}nbfi_phy_channel_t;




typedef enum
{
    HANDSHAKE_NONE      = 0,
    HANDSHAKE_SIMPLE    = 1,
    HANDSHAKE_TRIPPLE   = 2,
}nbfi_handshake_t;

typedef enum
{
    PACKET_FREE             = 0,
    PACKET_ALLOCATED        = 1,
    PACKET_QUEUED           = 2,
    PACKET_QUEUED_AGAIN     = 3,
    PACKET_TX               = 4,
    PACKET_SENT             = 5,
    PACKET_SENT_NOACKED     = 6,
    PACKET_RECEIVED         = 7,
    PACKET_DELIVERED        = 8,
    PACKET_LOST             = 9,
    PACKET_PROCESSING       = 0x0A,
    PACKET_PROCESSED        = 0x0B,
    PACKET_SKIPPED          = 0x0C,
    PACKET_REC_AND_SENT     = 0x0D,
//    PACKET_DL_DELAY         = 0x0E,
//    PACKET_DL_RX            = 0x0F,
    PACKET_WAIT_ACK         = 0x10,
    PACKET_GOT_DL           = 0x11,
    PACKET_DL_TIMEOUT       = 0x12,
    PACKET_NEED_TO_SEND_RIGHT_NOW   = 0x13,
    PACKET_WAIT_FOR_EXTRA_PACKETS   = 0x14,
    PACKET_CLEARED          = 0x15
}nbfi_packet_state_t;


/*NBFi physical layer packet struct*/
typedef struct
{
    union
    {
        struct
        {
            uint8_t ITER        : 5;//LSB
            uint8_t MULTI       : 1;
            uint8_t ACK         : 1;
            uint8_t SYS         : 1;//MSB
        };
        uint8_t header;
    };
    uint8_t payload[0];     //begining of packet payload
}nbfi_pfy_packet_t;


/*NBFi transport layer packet struct*/
typedef struct
{
    nbfi_packet_state_t state;              //packet state
    nbfi_handshake_t    handshake;          //packet handshake mode
    uint8_t             retry_num;          //retry counter
    uint8_t             mack_num;           //number of packets for multi ack mode
    uint8_t             phy_data_length;    //length of packet payload(without header)
    nbfi_pfy_packet_t   phy_data;           //physical layer packet data
}nbfi_transport_packet_t;


#define SYS_CMD      (0)

enum system_cmd_list
{
    RLC_MODE        = 0, // DL
    SET_CHANNEL     = 1, // DL
    DEVICE_STATUS   = 2, // UL
    SET_TIMING      = 3, // DL
    SYS_CMD_ATTACH  = 4
    //...
};

/*typedef enum
{
    NBFI_NOT_ATTACHED,
    NBFI_ATTACH_IN_PROCESS,
    NBFI_ATTACHED
}nbfi_attach_state_t;*/


typedef struct
{
    uint32_t UL_total;
    uint32_t UL_iter;
    uint32_t DL_total;
    uint32_t DL_iter;
    uint8_t  aver_rx_snr;
    uint8_t  aver_tx_snr;
    int16_t success_total;
    int16_t fault_total;
    int16_t last_rssi;
    uint8_t UL_rating;
    uint8_t DL_rating;
}nbfi_state_t;



extern nbfi_state_t __xdata nbfi_state;
extern nbfi_phy_channel_t nbfi_phy_channel;


extern const uint32_t NBFI_DL_DELAY[];
extern const uint32_t NBFI_DL_LISTEN_TIME[];
extern const uint32_t NBFI_DL_ADD_RND_LISTEN_TIME[];

typedef enum
{
    OK = 0,
    ERR = 1,
    ERR_RF_BUSY = 2,
    ERR_ACK_LOST = 3,
    ERR_BUFFER_FULL = 4
    //...
}nbfi_status_t;


typedef void (*rx_handler_t)(uint8_t __generic*, uint16_t);


enum{DL_MSG_TYPE_ACK=1, DL_MSG_TYPE_DATA=2, DL_MSG_TYPE_CHANNEL=3};

// NB-Fi header
#define SYS_FLAG        (1<<7)
#define ACK_FLAG        (1<<6)
#define MULTI_FLAG      (1<<5)
#define RECEPTION_FLAG  (1<<5)

//#define UL_ITER_MASK    0x1F
//#define DL_ITER_MASK    0x0F

typedef enum
{
    PCB = 0,       //PCB or ANT 1
    SMA = 1        //SMA or ANT 2
}rf_antenna_t;

typedef enum
{
    RX = 0,
    TX = 1,
    IDLE = 2
}rf_direction_t;

typedef enum
{
    NO_PADDING = 0,
    PADDING_4TO1
}rf_padding_t;

typedef enum
{
    NONBLOCKING,
    BLOCKING,
}rf_blocking_t;

extern bool __xdata rf_busy;
extern nbfi_transport_packet_t __xdata idle_pkt;
extern nbfi_transport_packet_t __xdata* __xdata nbfi_active_pkt;
extern bool wait_RxEnd;


extern  struct axradio_address fastdladdress;

__reentrantb void ax5043_set_constants(void) __reentrant;

/* Transport */
nbfi_status_t   NBFi_Send(uint8_t __generic* payload, uint8_t length);
static void     NBFi_ProcessTasks(struct wtimer_desc __xdata *desc);
void            NBFi_TX_Finished();
void            NBFi_RX_DL_EndHandler(struct wtimer_desc __xdata *desc);
void            NBFi_Receive_Timeout_cb(struct wtimer_desc __xdata *desc);
void            NBFi_Wait_Extra_Handler(struct wtimer_desc __xdata *desc);
void            NBFi_SendHeartBeats(struct wtimer_desc __xdata *desc);
void            NBFi_ProcessRxPackets();
void            NBFi_Force_process();
nbfi_status_t   NBFI_Init(rx_handler_t h);
void            NBFi_ParseReceivedPacket(struct axradio_status __xdata *st);
nbfi_status_t   NBFi_Attach();

/* Phy */
bool            NBFi_Match_ID(uint8_t __generic * addr);
nbfi_status_t   NBFi_TX_ProtocolD(nbfi_transport_packet_t __generic* pkt);
nbfi_status_t   NBFi_TX_ProtocolC(nbfi_transport_packet_t __generic* pkt);
nbfi_status_t   NBFi_TX_Correction();
nbfi_status_t   NBFi_RX_Controller();
nbfi_status_t   NBFi_RX();
nbfi_status_t   NBFi_TX(nbfi_transport_packet_t __generic* pkt);
void            NBFi_XTEA_OFB(uint8_t __xdata* buf, uint8_t len, uint8_t iter);

/* RF */
void            RF_SetFreq(uint32_t freq);
void            RF_SetModeAndPower(int8_t dBm, rf_direction_t mode, rf_antenna_t ant);
void            RF_SetDstAddress(uint8_t __generic * addr);
void            RF_SetLocalAddress(uint8_t __generic * addr);
nbfi_status_t   RF_Init(nbfi_phy_channel_t  phy_channel,
                        rf_antenna_t        antenna,
                        int8_t              power,
                        uint32_t            freq);
nbfi_status_t   RF_Deinit();
nbfi_status_t   RF_Transmit(uint8_t __generic* pkt, uint8_t len,  rf_padding_t padding, rf_blocking_t blocking);

/* Misc */
extern nbfi_transport_packet_t  __xdata* nbfi_TX_pktBuf[NBFI_TX_PKTBUF_SIZE];
extern nbfi_transport_packet_t __xdata* nbfi_RX_pktBuf[NBFI_RX_PKTBUF_SIZE];

nbfi_transport_packet_t __xdata*    NBFi_AllocateTxPkt(uint8_t payload_length);
void                                NBFi_TxPacket_Free(nbfi_transport_packet_t __xdata* pkt);
void                                NBFi_RxPacket_Free(nbfi_transport_packet_t __xdata* pkt);
nbfi_transport_packet_t __xdata*    NBFi_AllocateRxPkt(uint8_t header, uint8_t payload_length);
nbfi_transport_packet_t __xdata*    NBFi_GetQueuedTXPkt();
uint8_t                             NBFi_Packets_To_Send();
uint8_t                             NBFi_Mark_Lost_All_Unacked();
bool                                NBFi_Check_RX_Packet_Duplicate(nbfi_pfy_packet_t __xdata * pkt, uint8_t len);
nbfi_transport_packet_t __xdata*    NBFi_Get_QueuedRXPkt(uint8_t *groupe, uint16_t *total_length);
nbfi_transport_packet_t __xdata*    NBFi_GetSentTXPkt_By_Iter(uint8_t iter);
uint8_t                             NBFi_Calc_Queued_Sys_Packets_With_Type(uint8_t type);
uint8_t                             NBFi_Calc_Packets_With_State(uint8_t state);
uint32_t                            NBFi_Get_RX_ACK_Mask();
void                                NBFi_Resend_Pkt(nbfi_transport_packet_t __xdata* act_pkt, uint32_t mask);
void                                NBFi_Clear_RX_Buffer();
void                                NBFi_Clear_TX_Buffer();
void                                NBFi_Send_Clear_Cmd(uint8_t iter);
uint16_t                            CRC16(uint8_t __generic *buf, uint16_t len, uint16_t crc);
uint8_t                             CRC8(uint8_t __generic* data, uint8_t len);
//uint32_t                            NBFi_ConvertFreq(uint32_t f_ul);
uint16_t                            NBFi_Phy_To_Bitrate(nbfi_phy_channel_t ch);
uint8_t                             NBFi_Get_TX_Iter();


#endif // NBFI_H
