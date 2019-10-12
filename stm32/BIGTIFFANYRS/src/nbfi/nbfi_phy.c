#include <ax8052f143.h>
#include <libmftypes.h>
#include <libmfcrc.h>
#include <stdlib.h>

#include "nbfi.h"
#include "nbfi_config.h"
#include "rf.h"
#include "xtea.h"
#include "zigzag.h"
#include "hal.h"


/* PHYSICAL LAYER */


void DL_Receive_Timeout_cb(struct wtimer_desc __xdata *desc);

uint32_t __xdata last_pkt_crc = 0;

const uint8_t __code protD_preambula[] = {0x97, 0x15, 0x7A, 0x6F};
const uint8_t __code protocolC_preambula[] = {0x97, 0x15, 0x7A};

static uint8_t __xdata ul_buf[64];

// Send basic NBFi uplink for remote nodes
uint32_t __xdata tx_freq, rx_freq;
nbfi_status_t NBFi_TX_ProtocolD(nbfi_transport_packet_t __generic* pkt)
{
    uint8_t len = 0;
    static bool parity = 0;
    uint8_t lastcrc8;
    /* Prepare packet */
    memset_xdata(ul_buf,0,sizeof(ul_buf));

    for(int i=0; i<sizeof(protD_preambula); i++)
    {
        ul_buf[len++] = protD_preambula[i];
    }

    ul_buf[len++] = nbfi.temp_ID[0];
    ul_buf[len++] = nbfi.temp_ID[1];
    ul_buf[len++] = nbfi.temp_ID[2];
    ul_buf[len++] = pkt->phy_data.header;

    memcpy_xdatageneric(&ul_buf[len], pkt->phy_data.payload, pkt->phy_data_length);

    lastcrc8 =  CRC8(&ul_buf[len], 8);

    if(XTEA_Enabled() && XTEA_Available() && !(nbfi.additional_flags&NBFI_FLG_NO_XTEA))
    {
        XTEA_Encode(&ul_buf[len]);
    }
    len += 8;

    ul_buf[len++] = lastcrc8;

    last_pkt_crc = crc_crc32_msb(ul_buf + 4, 13, 0xFFFFFFFF) ^ 0xFFFFFFFF;

    //ul_buf[len++] = (uint8_t)(last_pkt_crc >> 24);
    ul_buf[len++] = (uint8_t)(last_pkt_crc >> 16);
    ul_buf[len++] = (uint8_t)(last_pkt_crc >> 8);
    ul_buf[len++] = (uint8_t)(last_pkt_crc);

    ZIGZAG_Append(&ul_buf[4], &ul_buf[len], parity);  // len is not updated here, but it is assumed that ul_buf grew by 16, don't forget when sending to transmit!


    if(nbfi.tx_freq)    tx_freq = nbfi.tx_freq ;
    else                tx_freq = nbfi.ul_freq_base + (((*((const uint32_t __code*)FULL_ID)+lastcrc8)%226)*100);//tx_freq = NBFI_UL_FREQ_BASE + (((*((const uint32_t __code*)FULL_ID)+lastcrc8)%226)*100);

    if(parity) tx_freq = tx_freq + 27500;

    parity = !parity;

    RF_Init(nbfi.tx_phy_channel, nbfi.tx_antenna, nbfi.tx_pwr, tx_freq);
    RF_Transmit(ul_buf, len + ZIGZAG_LEN, PADDING_4TO1, NONBLOCKING);

    nbfi_state.UL_total++;

    return OK;

}
/*
uint8_t __xdata hop=0, packet_hop=0;
struct wtimer_desc __xdata hopping_desc;
void Hopping(struct wtimer_desc __xdata *desc)
{
    RF_SetFreq(927000000 + (30000*(uint32_t)hop));
    RF_Transmit((uint8_t __generic*)(&corr_packet[2*packet_hop]), 2, PADDING_4TO1, NONBLOCKING);

    hop = (hop + 7) % 50;
    packet_hop++;
    if(packet_hop<10)
    {
        ScheduleTask(desc, 0, ABSOLUTE, desc->time + MILLISECONDS(440));
    }
}

nbfi_status_t NBFi_TX_Hopping()
{
    packet_hop = 0;
    RF_Init(TX_DBPSK_50, 0, nbfi.tx_pwr, 927000000);

    ScheduleTask(&hopping_desc, &Hopping, RELATIVE, 20);
    while(packet_hop < 10 || rf_busy)
    {
        wtimer_runcallbacks();
    }

    RF_Deinit();

    return OK;
}
*/


nbfi_status_t NBFi_TX_ProtocolC(nbfi_transport_packet_t __generic* pkt)
{
    uint8_t len = 0;
    static bool parity = 0;

    /* Prepare packet */
    memset_xdata(ul_buf,0,sizeof(ul_buf));

    for(int i=0; i<sizeof(protocolC_preambula); i++)
    {
        ul_buf[len++] = protocolC_preambula[i];
    }

    ul_buf[len++] = 0x6F;//nbfi.full_ID[2];
    ul_buf[len++] = nbfi.temp_ID[1];
    ul_buf[len++] = nbfi.temp_ID[2];
    ul_buf[len++] = pkt->phy_data.header;

    memcpy_xdatageneric(&ul_buf[len], pkt->phy_data.payload, pkt->phy_data_length);



    if(XTEA_Enabled() && XTEA_Available()&& !(nbfi.additional_flags&NBFI_FLG_NO_XTEA))
    {
        XTEA_Encode(&ul_buf[len]);
    }
    len += 8;


    last_pkt_crc = crc_crc32_msb(ul_buf + 3, 12, 0xFFFFFFFF) ^ 0xFFFFFFFF;

    ul_buf[len++] = (uint8_t)(last_pkt_crc >> 24);
    ul_buf[len++] = (uint8_t)(last_pkt_crc >> 16);
    ul_buf[len++] = (uint8_t)(last_pkt_crc >> 8);
    ul_buf[len++] = (uint8_t)(last_pkt_crc);

    ZIGZAG_Append(&ul_buf[3], &ul_buf[len], parity);  // len is not updated here, but it is assumed that ul_buf grew by 16, don't forget when sending to transmit!


    if(nbfi.tx_freq)    tx_freq = nbfi.tx_freq ;
    else                tx_freq = nbfi.ul_freq_base + ((*((const uint32_t __code*)FULL_ID)%226)*100);//tx_freq = NBFI_UL_FREQ_BASE + ((*((const uint32_t __code*)FULL_ID)%226)*100);

    if(parity) tx_freq = tx_freq + 27500;

    parity = !parity;

    RF_Init(nbfi.tx_phy_channel, nbfi.tx_antenna, nbfi.tx_pwr, tx_freq);
    RF_Transmit(ul_buf, len + ZIGZAG_LEN, PADDING_4TO1, NONBLOCKING);

    nbfi_state.UL_total++;

    return OK;
}



//TODO make const
//const uint8_t __code corr_packet[] = {0x97, 0x15, 0x7A, 0x6f, 0, 0, 0, 0, 0, 2, 3, 4, 5, 6, 7, 0xb5, 0x28, 0xa9, 0x51};

nbfi_status_t NBFi_TX_Correction()
{

    uint8_t len = 0;
    static bool parity = 0;

    /* Prepare packet */
    //memset_xdata(ul_buf,0,sizeof(ul_buf));

    for(int i=0; i<sizeof(protocolC_preambula); i++)
    {
        ul_buf[len++] = protocolC_preambula[i];
    }

    ul_buf[len++] = 0x6f;//nbfi.full_ID[2];
    ul_buf[len++] = nbfi.temp_ID[1];
    ul_buf[len++] = nbfi.temp_ID[2];
    ul_buf[len++] = 0;

    len += 8;

    last_pkt_crc = crc_crc32_msb(ul_buf + 3, 12, 0xFFFFFFFF) ^ 0xFFFFFFFF;

    ul_buf[len++] = (uint8_t)(last_pkt_crc >> 24);
    ul_buf[len++] = (uint8_t)(last_pkt_crc >> 16);
    ul_buf[len++] = (uint8_t)(last_pkt_crc >> 8);
    ul_buf[len++] = (uint8_t)(last_pkt_crc);

    #ifdef SDR_BS3
    #ifdef AUTOCORR_LNA_OFF
    send_auto_corr = 1;
    #endif
    #endif // SDR_BS3

    RF_Init(UL_DBPSK_50_PROT_C, nbfi.tx_antenna, nbfi.tx_pwr, nbfi.tx_freq);
    RF_Transmit(ul_buf, len, PADDING_4TO1, NONBLOCKING);
    return OK;
}


//uint8_t __xdata hop;
//struct wtimer_desc __xdata hopping_desc;
//void Hopping(struct wtimer_desc __xdata *desc)
//{
//    RF_SetFreq(868800000 + (1000*(uint32_t)hop));
//
//    hop++;
//    if(hop<10)
//    {
//        ScheduleTask(desc, 0, ABSOLUTE, desc->time + MILLISECONDS(440));
//    }
//}
//
//nbfi_status_t NBFi_TX_Hopping()
//{
//    hop = 0;
//    RF_Init(UL_DBPSK_50_PROT_C, 0, nbfi.tx_pwr, 868800000);
//
//    ScheduleTask(&hopping_desc, &Hopping, RELATIVE, 20);
//    while(hop < 10 || rf_busy)
//    {
//        wtimer_runcallbacks();
//    }
//
//    RF_Deinit();
//
//    return OK;
//}






bool NBFi_Match_ID(uint8_t __generic * addr)
{
    uint8_t i;
    for( i = 0; i !=3; i++) if(nbfi.temp_ID[i] != addr[i]) break;
    if(i == 3)  return true;

    for(i = 0; i !=3; i++) if(nbfi.broadcast_ID[i] != addr[i]) break;
    if(i == 3)  return true;

    return false;
}

nbfi_status_t NBFi_TX(nbfi_transport_packet_t __generic* pkt)
{
    nbfi_status_t result;

    if((pkt->phy_data_length==0)&&(pkt->phy_data_length>240)) return ERR; // len check
    switch(nbfi.tx_phy_channel)
    {
    case UL_DBPSK_50_PROT_C:
    case UL_DBPSK_400_PROT_C:
        return NBFi_TX_ProtocolC(pkt);
    case UL_DBPSK_50_PROT_D:
    case UL_DBPSK_400_PROT_D:
    case UL_DBPSK_3200_PROT_D:
        return NBFi_TX_ProtocolD(pkt);
    case UL_PSK_FASTDL:
    case UL_PSK_200:
    case UL_PSK_500:
    case UL_PSK_5000:

        if(nbfi.tx_freq == 0) tx_freq = nbfi.dl_freq_base;//tx_freq = NBFI_DL_FREQ_BASE;
        else tx_freq = nbfi.tx_freq;

        if(nbfi.tx_phy_channel == UL_PSK_FASTDL)
        {
            tx_freq += 1000000;
        }


        /*if(nbfi.tx_freq > 1000) tx_freq = nbfi.tx_freq;
        else
        {
            //tx_freq = NBFI_DL_FREQ_BASE + NBFI_DL_CHANNEL_WIDTH[4];
        }*/
        //RF_SetDstAddress(nbfi.dl_ID);

        RF_SetDstAddress((uint8_t __generic *)&fastdladdress);

        if(result = RF_Init(nbfi.tx_phy_channel, nbfi.tx_antenna, nbfi.tx_pwr, tx_freq) != OK) return result;
        //if(nbfi.tx_phy_channel == UL_PSK_5000)
        //{
        uint8_t __xdata* buf = (uint8_t __xdata*) malloc(pkt->phy_data_length + 1 + 3);
        if(!buf) return ERR_BUFFER_FULL;
        if(nbfi.dl_ID[0] == 0)
        {
            for(uint8_t i = 0; i != 2; i++) buf[i] = nbfi.dl_ID[i + 1];
            memcpy_xdata(buf + 2, (const void __xdata *)&pkt->phy_data.header, pkt->phy_data_length + 1);
            result = RF_Transmit(buf, pkt->phy_data_length + 1 + 2, NO_PADDING, NONBLOCKING);
        }
        else
        {
            for(uint8_t i = 0; i != 3; i++) buf[i] = nbfi.dl_ID[i];
            memcpy_xdata(buf + 3, (const void __xdata *)&pkt->phy_data.header, pkt->phy_data_length + 1);
            result = RF_Transmit(buf, pkt->phy_data_length + 1 + 3, NO_PADDING, NONBLOCKING);
        }

        free(buf);
        if(result != OK) return result;
        //}
        //else if(result = RF_Transmit(&pkt->phy_data.header, pkt->phy_data_length + 1, NO_PADDING, BLOCKING) != OK) return result;
        nbfi_state.UL_total++;
        break;

    }

    //RF_Deinit();
    return OK;
}


nbfi_status_t NBFi_RX_Controller()
{
    switch(nbfi.mode)
    {
    case  DRX:
    case  NRX:
        if(wait_RxEnd ) if(rf_state != STATE_RX)return NBFi_RX();
        else break;
        switch(nbfi_active_pkt->state)
        {
//        case PACKET_DL_RX:
        case PACKET_WAIT_ACK:
        case PACKET_QUEUED_AGAIN:
        case PACKET_WAIT_FOR_EXTRA_PACKETS:
            if(rf_state != STATE_RX) return NBFi_RX();
            break;
        default:
            if(rf_state != STATE_OFF)
                return RF_Deinit();
        }
        break;
    case CRX:
    case TRANSPARENT:
        if(rf_state != STATE_RX) return NBFi_RX();
    }
}



nbfi_status_t NBFi_RX()
{
    nbfi_status_t result;
    //return OK;
    switch(nbfi.rx_phy_channel)
    {
        case DL_PSK_200:
        case DL_PSK_500:
        case DL_PSK_5000:
        case DL_PSK_FASTDL:
             if(nbfi.rx_freq == 0) rx_freq = nbfi.dl_freq_base + ((*((const uint32_t __code*)FULL_ID)%276)*363);//rx_freq = NBFI_DL_FREQ_BASE + ((*((const uint32_t __code*)FULL_ID)%276)*363);
             else rx_freq = nbfi.rx_freq;
             if(nbfi.rx_phy_channel == DL_PSK_FASTDL) rx_freq += 1000000;
             break;

    }
    result = RF_Init(nbfi.rx_phy_channel, nbfi.rx_antenna, 0, rx_freq);
    return result;
}



void NBFi_XTEA_OFB(uint8_t __xdata* buf, uint8_t len, uint8_t iter)
{
 uint8_t vector[8];
 for(uint8_t i = 0; i != 3; i++)
 {
    vector[i] = 0;
    vector[i+5] = nbfi.temp_ID[i];
 }
 vector[3] = 0;
 vector[4] = iter;

 uint8_t n = 0;// number of cyphered bytes

 while(n < len)
 {

  if((n % 8) == 0) XTEA_Encode(vector); // next block

  buf[n] = vector[n%8] ^ buf[n];
  n++;
 }
}
