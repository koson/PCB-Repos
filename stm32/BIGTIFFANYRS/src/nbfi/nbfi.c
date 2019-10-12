//
// S. Omelchenko
// 18.02.2016

#include "nbfi.h"
#include "nbfi_config.h"
#include "rf.h"
#include "xtea.h"
#include "slip.h"
#include "hal.h"
#include <stdlib.h>
#include <libmfadc.h>


#ifdef NBFi_DEBUG
uint8_t  __xdata string[100];
#endif

nbfi_phy_channel_t nbfi_phy_channel;

nbfi_state_t __xdata nbfi_state = {0,0,0,0,0,0,0,0};


const uint32_t NBFI_DL_DELAY[10] = {SECONDS(30), SECONDS(30), SECONDS(30), SECONDS(5), SECONDS(5), SECONDS(5), SECONDS(1), SECONDS(1), MILLISECONDS(500), MILLISECONDS(500)};
const uint32_t NBFI_DL_LISTEN_TIME[4] = {SECONDS(40), SECONDS(40), SECONDS(40), SECONDS(40)};
const uint32_t NBFI_DL_ADD_RND_LISTEN_TIME[4] = {SECONDS(20), SECONDS(20), SECONDS(20), SECONDS(20)};
#define DRXLISTENAFTERSEND  20

#define WAITALITTLEBIT  3000

const uint8_t NBFI_NOISE_DINAMIC[4] = {20, 8, 5, 5};


nbfi_transport_packet_t __xdata idle_pkt = {PACKET_FREE, HANDSHAKE_NONE, 0, 0, 0, {0,0} };
nbfi_transport_packet_t __xdata* __xdata nbfi_active_pkt = &idle_pkt;
nbfi_packet_state_t nbfi_active_pkt_old_state;

struct wtimer_desc __xdata nbfi_processTask_desc;
struct wtimer_desc __xdata dl_receive_desc;
struct wtimer_desc __xdata dl_drx_desc;
struct wtimer_desc __xdata wait_for_extra_desc;
struct wtimer_desc __xdata nbfi_heartbeat_desc;

//received_packet_t __xdata* rx_pkt;

//uint8_t __xdata rx_pkt_len = 0;// TODO, to refactor
int16_t __xdata rssi = 0;
int16_t __xdata offset = 0;

rx_handler_t __xdata  rx_handler;

uint8_t __xdata not_acked = 0;

//uint8_t __xdata has_smth_to_transmit;

uint8_t __xdata aver_tx_snr = 0;
uint8_t __xdata aver_rx_snr = 0;
int16_t __xdata noise = -150;

int16_t __xdata noise_summ = 0;
uint8_t __xdata noise_cntr = 0;
int16_t __xdata noise_min = -150;
uint8_t __xdata noise_min_cntr = 2;

bool wait_Receive = 0;
bool wait_Extra = 0;
bool wait_RxEnd = 0;

uint32_t __xdata info_timer;

nbfi_status_t NBFi_Send(uint8_t __generic* payload, uint8_t length)
{
    nbfi_transport_packet_t __xdata* packet;
    uint8_t groupe = 0;
    uint8_t len = length;
    uint8_t free = NBFI_TX_PKTBUF_SIZE - NBFi_Packets_To_Send();
    if((length <= nbfi.max_payload_len) && (free < nbfi.mack_mode + 3 ) ) return ERR_BUFFER_FULL;
    else if((length/nbfi.max_payload_len + 3) > free) return ERR_BUFFER_FULL;
    if(length < nbfi.max_payload_len)
    {
        packet =  NBFi_AllocateTxPkt(length + 1);
        if(!packet)
        {
            return ERR_BUFFER_FULL;
        }
        packet->phy_data.SYS = 1;
        packet->phy_data.payload[0] = 0x80 + (length & 0x7f);
        memcpy_xdata(&packet->phy_data.payload[1], (void const*)payload, length);
        packet->state = PACKET_QUEUED;
        packet->handshake = nbfi.handshake_mode;
        packet->phy_data.ITER = nbfi_state.UL_iter++ & 0x1f;
        if(nbfi.handshake_mode != HANDSHAKE_NONE)
        {
            if(((nbfi_state.UL_iter) % nbfi.mack_mode) == 0)
            {
                packet->phy_data.ACK = 1;
                packet->mack_num = not_acked + 1;//nbfi.mack_mode;
                not_acked = 0;
            }
            else not_acked++;
        }

    }
    else do
    {
        uint8_t l;
        uint8_t first = 0;
        if(length > nbfi.max_payload_len)
        {
            first = (groupe == 0)*2;
            l = nbfi.max_payload_len - first;
        }
        else l = length;
        packet =  NBFi_AllocateTxPkt(l + first);
        if(!packet)
        {
            return ERR_BUFFER_FULL;
        }
        memcpy_xdata(&packet->phy_data.payload[first], (void const*)&payload[groupe * nbfi.max_payload_len - 2*(groupe != 0)], l);
        packet->state = PACKET_QUEUED;
        packet->handshake = nbfi.handshake_mode;
        packet->phy_data.ITER = nbfi_state.UL_iter++ & 0x1f;
        if(l < length)
        {
            packet->phy_data.MULTI = 1;
            if(groupe == 0) //the start packet of the groupe must be system
            {
                packet->phy_data.SYS = 1;
                packet->phy_data.payload[0] = 0x05;
                packet->phy_data.payload[1] = len;
            }
        }

        length -= l;
        groupe++;
        if((length == 0) && (groupe == 1))
        {
            if(nbfi.handshake_mode != HANDSHAKE_NONE)
            {
                if(((nbfi_state.UL_iter) % nbfi.mack_mode) == 0)
                {
                    packet->phy_data.ACK = 1;
                    packet->mack_num = not_acked + 1;//nbfi.mack_mode;
                    not_acked = 0;
                }
                else not_acked++;
            }
        }
        else   //the last packet of groupe must be acked
        {
            packet->phy_data.MULTI = 1;
            if(nbfi.handshake_mode != HANDSHAKE_NONE)
            {
                if(length == 0)
                {
                    packet->phy_data.ACK = 1;
                    packet->mack_num = groupe + not_acked;
                    not_acked = 0;
                }
            }
        }

    }while(length);

    NBFi_Force_process();
    return OK;
}

//uint8_t numa;
//nbfi_transport_packet_t __xdata* ppp;

void NBFi_ProcessRxPackets()
{
    nbfi_transport_packet_t __xdata* pkt;
    uint8_t __xdata *data;
    uint8_t groupe;
    uint16_t total_length;
    while(1)
    {

        pkt = NBFi_Get_QueuedRXPkt(&groupe, &total_length);

        if(!pkt)    return;

        data = malloc(total_length);
        //numa = total_length;
        //ppp = pkt;
        if(!data)   return;
        if((pkt->phy_data.SYS) && (pkt->phy_data.payload[0] & 0x80))
        {
            total_length = pkt->phy_data.payload[0] & 0x7f;
            memcpy_xdata(data, (void const*)(&pkt->phy_data.payload[1]), total_length);
            if(nbfi.mack_mode < MACK_2) NBFi_RxPacket_Free(pkt);
        }
        else
        {
            uint8_t iter = pkt->phy_data.ITER;
            uint16_t memcpy_len = total_length;
            //numa = total_length;
            for(uint8_t i = 0; i != groupe; i++)
            {
                uint8_t len;
                uint8_t first = 0;
                if((i == 0)&&(groupe > 1)) {len = 6; first = 2;}
                else len = (memcpy_len>=nbfi.max_payload_len)?nbfi.max_payload_len:memcpy_len%nbfi.max_payload_len;
                memcpy_xdata(data + i*nbfi.max_payload_len - 2*(i != 0), (void const*)(&nbfi_RX_pktBuf[(iter + i)&0x1f]->phy_data.payload[first]), len);
                memcpy_len -= len;
                if(nbfi_RX_pktBuf[(iter + i)&0x1f]->phy_data.ACK) nbfi_RX_pktBuf[(iter + i)&0x1f]->state = PACKET_CLEARED;
                else nbfi_RX_pktBuf[(iter + i)&0x1f]->state = PACKET_PROCESSED;

                if((nbfi.mack_mode < MACK_2) && (groupe == 1)) NBFi_RxPacket_Free(nbfi_RX_pktBuf[(iter + i)&0x1f]);

            }
        }
#ifndef NO_SLIP
        #ifdef TEXT_MODE
        SLIP_Send_debug((uint8_t __generic *)data, total_length);
        #else
        SLIP_Send(SLIP_CMD_RECEIVED_DL, (uint8_t __generic *)data, total_length);
        #endif
#endif
        rx_handler((uint8_t __generic *)data, total_length);

        free(data);
    }

}


//uint16_t ccrc;
//uint8_t __xdata *ppr;

void NBFi_ParseReceivedPacket(struct axradio_status __xdata *st)
{
    uint16_t ccrc;
    if((nbfi.mode != TRANSPARENT) && (!NBFi_Match_ID((uint8_t __generic *)st->u.rx.pktdata))) return;

    #ifdef USB_DONGLE
    PIN_SET_OUTPUT(REC_DIR,  REC_PIN);
    REC = 1;
    #endif // USB_DONGLE

#ifndef NOKEYDL
    if(!(nbfi.additional_flags&NBFI_FLG_NO_XTEA))
    {
        st->u.rx.pktlen -= 2;
        if(XTEA_Enabled() && XTEA_Available())
        {
        NBFi_XTEA_OFB((uint8_t __xdata*)(st->u.rx.pktdata + 4), st->u.rx.pktlen - 5 - 3, st->u.rx.pktdata[3]&0x1f);
        }
        ccrc = CRC16((uint8_t __generic *)(st->u.rx.pktdata + 4), st->u.rx.pktlen - 5 - 3, 0xFFFF);

        if(*((uint16_t*)(&st->u.rx.pktdata[st->u.rx.pktlen - 4])) != CRC16((uint8_t __generic *)(st->u.rx.pktdata + 4), st->u.rx.pktlen - 5 - 3, 0xFFFF)) return;
    }
#endif

    nbfi_state.DL_total++;
    if(++noise_min_cntr > NBFI_NOISE_DINAMIC[nbfi.rx_phy_channel]) noise_min_cntr =   NBFI_NOISE_DINAMIC[nbfi.rx_phy_channel];
    uint8_t snr;
    if(st->u.rx.phy.rssi < noise) snr = 0;
    else snr = (st->u.rx.phy.rssi - noise) & 0xff;

    nbfi_state.last_rssi = st->u.rx.phy.rssi;
    nbfi_state.aver_rx_snr = (((uint16_t)nbfi_state.aver_rx_snr)*3 + snr)>>2;

    nbfi_transport_packet_t __xdata* pkt = 0;

    if(nbfi.mode == TRANSPARENT)
    {
        #ifdef NBFi_DEBUG
                    my_sprintf((char __generic *)string, "Receive data, iter = %d", st->u.rx.pktdata[3]&0x1f);
                    SLIP_Send_debug((uint8_t __generic *)string, 50);
        #else
        *(((uint8_t __xdata *)st->u.rx.pktdata) - 1) = snr;
        SLIP_Send(SLIP_CMD_RECEIVED_DL, (uint8_t __generic *)st->u.rx.pktdata - 1, st->u.rx.pktlen - 4 + 1);
        #endif
        goto end;
    }

    //ppp = (uint8_t __xdata*)st->u.rx.pktdata;


    nbfi_pfy_packet_t __xdata *phy_pkt = (nbfi_pfy_packet_t __xdata *)st->u.rx.pktdata + 3;

   // ppr = (uint8_t __xdata*)phy_pkt;

    wtimer0_remove(&wait_for_extra_desc);
    wait_Extra = 0;

    if(nbfi_active_pkt->state == PACKET_WAIT_FOR_EXTRA_PACKETS)
    {
        nbfi_active_pkt->state = nbfi_active_pkt_old_state;
    }

    if(phy_pkt->SYS)
    {
            /* System messages */
            if(phy_pkt->payload[0] & 0x80) goto place_to_stack;
            switch(phy_pkt->payload[0]) // Message type
            {
            case 0x00:  //ACK received
                uint32_t mask = 0;
                uint8_t i = 1;
                if(((nbfi_active_pkt->state == PACKET_WAIT_ACK) /*|| (nbfi_active_pkt->state == PACKET_WAIT_FOR_EXTRA_PACKETS)*/) && (phy_pkt->ITER == nbfi_active_pkt->phy_data.ITER))
                {
                    wtimer0_remove(&dl_receive_desc);
                    wait_Receive = 0;

                    if(nbfi_active_pkt->mack_num == 0)
                    {
                        nbfi_state.success_total++;
                        nbfi_active_pkt->state =  PACKET_DELIVERED;
                    }


#ifdef NBFi_DEBUG
                    my_sprintf((char __generic *)string, "receive ack%d, snr=%d", phy_pkt->ITER, phy_pkt->payload[5]);
                    SLIP_Send_debug((uint8_t __generic *)string, 50);
#endif
                    //NBFi_Calc_Packets_With_State(PACKET_SENT_NOACKED);

                    nbfi_state.aver_tx_snr = (((uint16_t)nbfi_state.aver_tx_snr)*3 + phy_pkt->payload[5])>>2;
                    nbfi_station_info.info = phy_pkt->payload[7];
                    do
                    {
                        mask = (mask << 8) + phy_pkt->payload[i];
                    }   while (++i < 5);

                    NBFi_Resend_Pkt(nbfi_active_pkt, mask);
                    //NBFi_Calc_Packets_With_State(PACKET_SENT_NOACKED);

                }
                break;

            case 03:    //ACK on system packet received
                if((nbfi_active_pkt->state == PACKET_WAIT_ACK))
                {
                    wtimer0_remove(&dl_receive_desc);
                    wait_Receive = 0;
                    nbfi_active_pkt->state =  PACKET_DELIVERED;
                    nbfi_state.success_total++;
                }
                break;
            case 0x04:  //clear RX buffer message received
                NBFi_Clear_RX_Buffer();
                #ifdef NBFi_DEBUG
                my_sprintf((char __generic *)string, "receive clear%d", phy_pkt->ITER);
                SLIP_Send_debug((uint8_t __generic *)string, 50);
                #endif
                break;
            case 0x05:  //start packet of the groupe
                goto place_to_stack;
            case 0x06:  //nbfi configure

                if(NBFi_Config_Parser(&phy_pkt->payload[1]))
                {
                    nbfi_transport_packet_t __xdata* ack_pkt =  NBFi_AllocateTxPkt(8);
                    if(!ack_pkt) break;
                    ack_pkt->phy_data.payload[0] = 0x06;
                    memcpy_xdata(&ack_pkt->phy_data.payload[1], &phy_pkt->payload[1], 7);
                    ack_pkt->phy_data.ITER = phy_pkt->ITER;
                    ack_pkt->phy_data.header |= SYS_FLAG;
                    if(nbfi_settings_changed)
                    {
                            ack_pkt->handshake = HANDSHAKE_SIMPLE;
                            ack_pkt->phy_data.header |= ACK_FLAG;
                    }else   ack_pkt->handshake = HANDSHAKE_NONE;
                    nbfi_settings_changed = 0;
                    ack_pkt->state = PACKET_NEED_TO_SEND_RIGHT_NOW;
                    //nbfi_active_pkt = ack_pkt;
                }
                break;
            case 0x07:
                PCON |= 0x10; // SOFTWARE RESET
                break;


            }
            if(phy_pkt->ACK && !NBFi_Calc_Queued_Sys_Packets_With_Type(0))    //send ACK on system packet
            {
                    nbfi_transport_packet_t __xdata* ack_pkt =  NBFi_AllocateTxPkt(8);
                    if(ack_pkt)
                    {

                        ack_pkt->phy_data.payload[0] = 0x03; //ACK on SYS
                        ack_pkt->phy_data.payload[1] = phy_pkt->payload[0];//type of sys packet
                        ack_pkt->phy_data.payload[2] = 0;
                        ack_pkt->phy_data.payload[3] = 0;
                        ack_pkt->phy_data.payload[4] = 0;
                        ack_pkt->phy_data.payload[5] = snr;
                        ack_pkt->phy_data.payload[6] = (uint8_t)(noise + 150);
                        ack_pkt->phy_data.payload[7] = you_should_dl_power_step_down + you_should_dl_power_step_up + (nbfi.tx_pwr & 0x3f); //you_should_dl_power_step_down = (1 << 7);
                        ack_pkt->phy_data.ITER = phy_pkt->ITER;
                        ack_pkt->phy_data.header |= SYS_FLAG;
                        ack_pkt->handshake = HANDSHAKE_NONE;
                        ack_pkt->state = PACKET_NEED_TO_SEND_RIGHT_NOW;
                        //nbfi_active_pkt = ack_pkt;
                    }
            }

    }
    else
    {
        //Get application packet
place_to_stack:
        if(!NBFi_Check_RX_Packet_Duplicate(phy_pkt, st->u.rx.pktlen - 4 - 3))   //if there is no rx duplicate
        {
            pkt = NBFi_AllocateRxPkt(phy_pkt->header, st->u.rx.pktlen - 5 - 3);
            if(!pkt) return;
            memcpy_xdata(&pkt->phy_data.header, phy_pkt, st->u.rx.pktlen - 4 - 3);
            pkt->state = PACKET_RECEIVED;
            #ifdef NBFi_DEBUG
            my_sprintf((char __generic *)string, "receive data%d", pkt->phy_data.ITER);
            SLIP_Send_debug((uint8_t __generic *)string, 50);
            #endif
        }

        if(phy_pkt->ACK && !NBFi_Calc_Queued_Sys_Packets_With_Type(0))
        {
            // Send ACK
            nbfi_transport_packet_t __xdata* ack_pkt =  NBFi_AllocateTxPkt(8);
            if(ack_pkt)
            {
                uint32_t mask = NBFi_Get_RX_ACK_Mask();
                ack_pkt->phy_data.payload[0] = 0x00;
                ack_pkt->phy_data.payload[1] = (mask >> 24)&0xff;
                ack_pkt->phy_data.payload[2] = (mask >> 16)&0xff;
                ack_pkt->phy_data.payload[3] = (mask >> 8)&0xff;
                ack_pkt->phy_data.payload[4] = (mask >> 0)&0xff;
                ack_pkt->phy_data.payload[5] = snr;
                ack_pkt->phy_data.payload[6] = (uint8_t)(noise + 150);
                ack_pkt->phy_data.payload[7] = you_should_dl_power_step_down + you_should_dl_power_step_up + (nbfi.tx_pwr & 0x3f); //you_should_dl_power_step_down = (1 << 7);
                ack_pkt->phy_data.ITER = nbfi_state.DL_iter&0x1f;
                ack_pkt->phy_data.header |= SYS_FLAG;
                if(nbfi.handshake_mode == HANDSHAKE_TRIPPLE)
                {
                    ack_pkt->handshake = HANDSHAKE_SIMPLE;
                    ack_pkt->phy_data.header |= ACK_FLAG;
                }
                else ack_pkt->handshake = HANDSHAKE_NONE;
                ack_pkt->state = PACKET_NEED_TO_SEND_RIGHT_NOW;
                //nbfi_active_pkt = ack_pkt;
                #ifdef NBFi_DEBUG
                my_sprintf((char __generic *)string, "send ack%d:%04X%04X", ack_pkt->phy_data.ITER, (uint16_t)(mask>>16), (uint16_t)mask&0xffff);
                SLIP_Send_debug((uint8_t __generic *)string, 50);
                #endif
                NBFi_Force_process();
            }
        }
    }

    NBFi_ProcessRxPackets();

    if(/*phy_pkt->SYS && */phy_pkt->MULTI && !phy_pkt->ACK)
    {
        //wait for extra packets
         #ifdef NBFi_DEBUG
                    my_sprintf((char __generic *)string, "nbfi_active_pkt->state = PACKET_WAIT_FOR_EXTRA_PACKETS");
                    SLIP_Send_debug((uint8_t __generic *)string, 100);
        #endif
        nbfi_active_pkt_old_state = nbfi_active_pkt->state;
        nbfi_active_pkt->state = PACKET_WAIT_FOR_EXTRA_PACKETS;
        ScheduleTask(&wait_for_extra_desc, NBFi_Wait_Extra_Handler, RELATIVE, NBFI_DL_LISTEN_TIME[nbfi.rx_phy_channel]);
        wait_Extra = 1;
    }
    else
    {
        if(nbfi_active_pkt->state == PACKET_WAIT_FOR_EXTRA_PACKETS) nbfi_active_pkt->state = nbfi_active_pkt_old_state;

    }

    if(!phy_pkt->ACK) NBFI_Config_Check_State();
    if(NBFi_GetQueuedTXPkt()) NBFi_Force_process();
    else
    {
        if(nbfi.mode == DRX)
        {
            wait_RxEnd = 1;
            ScheduleTask(&dl_drx_desc, NBFi_RX_DL_EndHandler, RELATIVE, MILLISECONDS(WAITALITTLEBIT));
        }
    //if(wait_RxEnd) {wait_RxEnd = 0; wtimer0_remove(&dl_drx_desc);}
end:   NBFi_RX_Controller();
    }

}


static void NBFi_ProcessTasks(struct wtimer_desc __xdata *desc)
{
   #ifdef AMPER_2_3
    if((rf_busy == false) && (nbfi.mode > DRX))
   #else
   if(rf_busy == false)
   #endif
    {
        switch(nbfi_active_pkt->state)
        {
//        case PACKET_DL_DELAY:
        case PACKET_WAIT_ACK:
            if(!wait_Receive)
            {
                ScheduleTask(&dl_receive_desc, NBFi_Receive_Timeout_cb, RELATIVE, NBFI_DL_DELAY[nbfi.tx_phy_channel - 20] + NBFI_DL_LISTEN_TIME[nbfi.rx_phy_channel] + random()%(NBFI_DL_ADD_RND_LISTEN_TIME[nbfi.rx_phy_channel]));
                wait_Receive = 1;
            }
            break;
        case PACKET_WAIT_FOR_EXTRA_PACKETS:
            if(!wait_Extra)
            {
                ScheduleTask(&wait_for_extra_desc, NBFi_Wait_Extra_Handler, RELATIVE, NBFI_DL_LISTEN_TIME[nbfi.rx_phy_channel]);
                wait_Extra = 1;
            }
            break;
        default:

            nbfi_transport_packet_t __xdata* pkt = NBFi_GetQueuedTXPkt();
            if(pkt)
            {
                if((pkt->handshake != HANDSHAKE_NONE) && (nbfi.mode != TRANSPARENT))
                {
                    if(pkt->phy_data.ACK)
                    {
                        switch(nbfi.mode)
                        {
                        case DRX:
                        case CRX:
                            pkt->state = PACKET_WAIT_ACK;
                            ScheduleTask(&dl_receive_desc, NBFi_Receive_Timeout_cb, RELATIVE, NBFI_DL_DELAY[nbfi.tx_phy_channel - 20] + NBFI_DL_LISTEN_TIME[nbfi.rx_phy_channel] + random()%(NBFI_DL_ADD_RND_LISTEN_TIME[nbfi.rx_phy_channel]));
                            wait_Receive = 1;
                            break;
                        case NRX:
                            pkt->state = PACKET_SENT;
                            break;
                        }
                    }else pkt->state = PACKET_SENT_NOACKED;
                }
                else pkt->state = PACKET_SENT;
                nbfi_active_pkt = pkt;
                if(pkt->phy_data.SYS && !pkt->phy_data.ACK && NBFi_GetQueuedTXPkt()) pkt->phy_data.header |= MULTI_FLAG;
                #ifdef NBFi_DEBUG
                my_sprintf((char __generic *)string, "send data%d", pkt->phy_data.ITER);
                SLIP_Send_debug((uint8_t __generic *)string, 50);
                #endif

                if(wait_RxEnd) {wait_RxEnd = 0; wtimer0_remove(&dl_drx_desc);}
                NBFi_TX(pkt);

                if(pkt->state == PACKET_SENT)
                {
                    NBFi_TxPacket_Free(pkt);
                    nbfi_active_pkt = &idle_pkt;
                }

            }
            else
            {
                    NBFi_RX_Controller();
            }
        }
    }
    else
    {
        GetVoltageOrTemp(2);
    }

    if(rf_state == STATE_RX)
    {
        if(noise_cntr >= 10)
        {
            int16_t n = noise_summ/noise_cntr;
            noise_summ = 0;
            noise_cntr = 0;
            if(n < noise_min) noise_min = n;
            if(--noise_min_cntr == 0)
            {
                if(noise_min == -150) noise = n;
                else noise = noise_min;
                noise_min = 0;
                noise_min_cntr =  NBFI_NOISE_DINAMIC[nbfi.rx_phy_channel];
                #ifdef NBFi_DEBUG
                    my_sprintf((char __generic *)string, "aver noise=%d", noise);
                    SLIP_Send_debug((uint8_t __generic *)string, 50);
                #endif
            }

        }
        else
        {
            int8_t r = AX5043_RSSI;
            noise_summ += r - (int16_t)axradio_phy_rssioffset;
            noise_cntr++;
            //#ifdef NBFi_DEBUG
            //    my_sprintf((char __generic *)string, "noise=%d", r - (int16_t)axradio_phy_rssioffset);
            //    SLIP_Send_debug((uint8_t __generic *)string, 50);
            //#endif
        }
    }
    #ifdef AMPER_2_3
    if(nbfi.mode <= DRX) NBFi_Clear_TX_Buffer();
    #endif // AMPER_2_3
    if(nbfi.mode <= DRX && !NBFi_GetQueuedTXPkt() && (rf_busy == false) )
    {
        NBFi_RX_Controller();
        if(rf_state == STATE_OFF) ScheduleTask(desc, 0, RELATIVE, SECONDS(10));
        else ScheduleTask(desc, 0, RELATIVE, MILLISECONDS(50));

    }
    else ScheduleTask(desc, 0, RELATIVE, MILLISECONDS(50));

}

void NBFi_TX_Finished()
{
    if(!nbfi_active_pkt->phy_data.ACK && NBFi_GetQueuedTXPkt())
    {
        NBFi_Force_process();
    }
    else
    {
        if(!nbfi_active_pkt->phy_data.ACK && (nbfi.mode == DRX))
        {
            wait_RxEnd = 1;
            ScheduleTask(&dl_drx_desc, NBFi_RX_DL_EndHandler, RELATIVE, SECONDS(DRXLISTENAFTERSEND));
        }
        else NBFI_Config_Check_State();
        NBFi_RX_Controller();
    }
}

void NBFi_RX_DL_EndHandler(struct wtimer_desc __xdata *desc)
{
    //nbfi_active_pkt->state = PACKET_DL_RX;
    //ScheduleTask(desc, NBFi_Receive_Timeout_cb, RELATIVE, NBFI_DL_LISTEN_TIME[nbfi.rx_phy_channel]);
    wait_RxEnd = 0;
    //wait_Receive = 1;
    NBFi_RX_Controller();
}


void NBFi_Receive_Timeout_cb(struct wtimer_desc __xdata *desc)
{
    if(rf_busy)
    {
        ScheduleTask(desc, NBFi_Receive_Timeout_cb, RELATIVE, NBFI_DL_LISTEN_TIME[nbfi.rx_phy_channel]);
        return;
    }
    wtimer0_remove(&dl_receive_desc);
    wait_Receive = 0;
    if(nbfi_active_pkt->state != PACKET_WAIT_ACK)
    {
        #ifdef NBFi_DEBUG
                    my_sprintf((char __generic *)string, "nbfi_active_pkt->state != PACKET_WAIT_ACK");
                    SLIP_Send_debug((uint8_t __generic *)string, 50);
        #endif
        return;
    }
    nbfi_state.fault_total++;
    NBFi_Config_Tx_Power_Change(UP);
    if(++nbfi_active_pkt->retry_num > nbfi.num_of_retries)
    {
       nbfi_active_pkt->state = PACKET_LOST;

       if(nbfi_active_pkt->phy_data.SYS && (nbfi_active_pkt->phy_data.payload[0] == 0x06)/* && (nbfi_active_pkt->phy_data.payload[1] != RATE_CHANGE_PARAM_CMD)*/)
        {
            //nbfi_active_pkt->state = PACKET_LOST;
            NBFi_Mark_Lost_All_Unacked();
            NBFi_Config_Return(); //return to previous work configuration
            nbfi_state.success_total = -10;
        }
        else
        {
            if(!(nbfi.additional_flags&NBFI_FLG_NO_RESET_TO_DEFAULTS))
            {
                if((current_tx_rate == 0)&&(current_rx_rate == 0))
                {

                    NBFi_Mark_Lost_All_Unacked();

                }
                else
                {
                    nbfi_active_pkt->retry_num = 0;
                    nbfi_active_pkt->state = PACKET_QUEUED;
                }
                NBFi_Config_Set_Default(); //set default configuration
                NBFi_Config_Send_Mode(false, NBFI_PARAM_MODE);
            }

        }
    }
    else
    {
        nbfi_active_pkt->state = PACKET_QUEUED_AGAIN;
    }
    NBFi_Force_process();
    return;
    //NBFi_RX_Controller();
}

void NBFi_Wait_Extra_Handler(struct wtimer_desc __xdata *desc)
{
    wtimer0_remove(&wait_for_extra_desc);
    wait_Extra = 0;
    if(nbfi_active_pkt->state == PACKET_WAIT_FOR_EXTRA_PACKETS)     {nbfi_active_pkt->state = nbfi_active_pkt_old_state;}
    if(NBFi_GetQueuedTXPkt()) NBFi_Force_process();
}


void NBFi_SendHeartBeats(struct wtimer_desc __xdata *desc)
{

    static uint16_t hb_timer = 0;
    static uint16_t osccal_timer = 0;
    if(hb_timer == 0) hb_timer = random()%nbfi.heartbeat_interval;
    if(nbfi.mode <= DRX)
    {
        ScheduleTask(&nbfi_heartbeat_desc, NBFi_SendHeartBeats, RELATIVE, SECONDS(60));
        //return;
    }
    else ScheduleTask(&nbfi_heartbeat_desc, NBFi_SendHeartBeats, RELATIVE, SECONDS(1));


    if(CheckTask(&nbfi_send_mode_delay)) return;

    if(++hb_timer >= nbfi.heartbeat_interval + 1)
    {
        hb_timer = 1;
        if(nbfi.heartbeat_num == 0) return;
        if(nbfi.heartbeat_num != 0xff) nbfi.heartbeat_num--;
        if(NBFi_Calc_Queued_Sys_Packets_With_Type(1)) return;
        nbfi_transport_packet_t __xdata* ack_pkt =  NBFi_AllocateTxPkt(8);
        if(!ack_pkt)   return;
        ack_pkt->phy_data.payload[0] = 0x01;
        ack_pkt->phy_data.payload[1] = 0;                      //heart beat type
        ack_pkt->phy_data.payload[2] = Supply_Voltage;         //min supply voltage since last heartbeat
        GetVoltageOrTemp(1);
        int8_t t = adc_measure_temperature() >> 8;                        //reset min voltage detection
        ack_pkt->phy_data.payload[3] = t;//GetVoltageOrTemp(0);    //temperature
        ack_pkt->phy_data.payload[4] = nbfi_state.aver_rx_snr; // DL average snr
        ack_pkt->phy_data.payload[5] = nbfi_state.aver_tx_snr; // UL average snr
        ack_pkt->phy_data.payload[6] = (uint8_t)(noise + 150); // rx noice
        ack_pkt->phy_data.payload[7] = nbfi.tx_pwr;            // output power
        ack_pkt->phy_data.ITER = nbfi_state.UL_iter++ & 0x1f;;
        ack_pkt->phy_data.header |= SYS_FLAG;
        if(nbfi.mode != NRX)
        {
            if(nbfi.handshake_mode != HANDSHAKE_NONE)  ack_pkt->handshake = HANDSHAKE_SIMPLE;
            ack_pkt->phy_data.header |= ACK_FLAG;
        }
        ack_pkt->state = PACKET_QUEUED;
    }

    if(!(nbfi.additional_flags&NBFI_FLG_NO_SENDINFO))
    {
        if(nbfi.mode <= DRX) info_timer += 60;
        else info_timer++;
        if(info_timer >= SEND_INFO_INTERVAL)
        {
                info_timer = 0;
                NBFi_Config_Send_Mode(false, NBFI_PARAM_TX_BRATES);
                NBFi_Config_Send_Mode(false, NBFI_PARAM_RX_BRATES);
                NBFi_Config_Send_Mode(false, NBFI_PARAM_VERSION);
        }
    }


    if((nbfi.additional_flags&NBFI_FLG_DO_OSCCAL)&&(nbfi.mode <= DRX))
    {
        if((osccal_timer++%MAKE_OSCCAL_INTERVAL) == 0)
        {
            if(!rf_busy)
            {
                RF_Init(OSC_CAL, 0, 0, 0);
            }
        }
    }




}

void NBFi_Force_process()
{
    ScheduleTask(&nbfi_processTask_desc, NBFi_ProcessTasks, RELATIVE, MILLISECONDS(5));
}


nbfi_status_t NBFI_Init(rx_handler_t h)
{

    NBFi_Config_Set_Default();
    for(uint8_t i = 0; i < NBFI_TX_PKTBUF_SIZE; i++) nbfi_TX_pktBuf[i] = 0;
    for(uint8_t i = 0; i < NBFI_RX_PKTBUF_SIZE; i++) nbfi_RX_pktBuf[i] = 0;

    rx_handler = h;

    info_timer = SEND_INFO_INTERVAL - 300 - random()%600;
    RF_Init(nbfi.tx_phy_channel, nbfi.tx_antenna, nbfi.tx_pwr, nbfi.dl_freq_base);

    NBFi_RX_Controller();

    NBFi_Config_Send_Mode(false, NBFI_PARAM_MODE);
    NBFi_Send_Clear_Cmd(0);

    NBFi_Force_process();
    GetVoltageOrTemp(1);
    ScheduleTask(&nbfi_heartbeat_desc, NBFi_SendHeartBeats, RELATIVE, SECONDS(1));
    return OK;
}


