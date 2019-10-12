// Copyright (c) 2007,2008,2009,2010,2011,2012,2013, 2014 AXSEM AG
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     1.Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     2.Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     3.Neither the name of AXSEM AG, Duebendorf nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//     4.All advertising materials mentioning features or use of this software
//       must display the following acknowledgement:
//       This product includes software developed by AXSEM AG and its contributors.
//     5.The usage of this source code is only granted for operation with AX5043
//       and AX8052F143. Porting to other radio or communication devices is
//       strictly prohibited.
//
// THIS SOFTWARE IS PROVIDED BY AXSEM AG AND CONTRIBUTORS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL AXSEM AG AND CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#ifndef EASYAX5043_H
#define EASYAX5043_H

#include "axradio.h"

#include <ax8052f143.h>
#include <libmftypes.h>

//#define DBGPKT
//#define FREQ_FULL_ADJUST

#define PKTDATA_BUFLEN 260

// typedefs
typedef enum {
    trxstate_off,
    trxstate_rx,
    trxstate_rxwor,
    trxstate_wait_xtal,
    trxstate_xtal_ready,
    trxstate_pll_ranging,
    trxstate_pll_ranging_done,
    trxstate_pll_settling,
    trxstate_pll_settled,
    trxstate_tx_xtalwait,
    trxstate_tx_longpreamble,
    trxstate_tx_shortpreamble,
    trxstate_tx_packet,
    trxstate_tx_waitdone,
    trxstate_txcw_xtalwait,
    trxstate_txstream_xtalwait,
    trxstate_txstream
} axradio_trxstate_t;

#define AXRADIO_ERR_PACKETDONE               0xf0

#if defined SDCC || defined __ICC8051__
struct u32endian {
    uint8_t b0;
    uint8_t b1;
    uint8_t b2;
    uint8_t b3;
};

struct u16endian {
    uint8_t b0;
    uint8_t b1;
};
#elif defined __CX51__ || defined __C51__
struct u32endian {
    uint8_t b3;
    uint8_t b2;
    uint8_t b1;
    uint8_t b0;
};

struct u16endian {
    uint8_t b1;
    uint8_t b0;
};
#else
#error "Unsupported Compiler"
#endif


// declaration of global variables
//hardware
volatile extern axradio_trxstate_t __data axradio_trxstate;

extern uint8_t __data EASY_RADIO_ax5043_powermode_after_send_packet; //used by EASY_RADIO_send_packet() default is powerdown
extern uint8_t __data EASY_RADIO_ax5043_powermode_after_rx_packet; // used by EASY_RADIO_radio_isr() upon FIFO not empty, default is powerdown

//packet
extern volatile uint8_t __data EASY_RADIO_saved_radiostate;



// additional AX5043 registers & definitions
SFRX(AX5043_0xF00,              0x4f00)
SFRX(AX5043_0xF0C,              0x4f0c)
SFRX(AX5043_0xF10,              0x4f10)
SFRX(AX5043_0xF11,              0x4f11)
SFRX(AX5043_0xF18,              0x4f18)
SFRX(AX5043_0xF1C,              0x4f1c)
SFRX(AX5043_0xF21,              0x4f21)
SFRX(AX5043_0xF22,              0x4f22)
SFRX(AX5043_0xF23,              0x4f23)
SFRX(AX5043_0xF26,              0x4f26)
SFRX(AX5043_0xF30,              0x4f30)
SFRX(AX5043_0xF31,              0x4f31)
SFRX(AX5043_0xF32,              0x4f32)
SFRX(AX5043_0xF33,              0x4f33)
SFRX(AX5043_0xF34,              0x4f34)
SFRX(AX5043_0xF35,              0x4f35)
SFRX(AX5043_0xF44,              0x4f44)

SFRX(AX5043_REF,                0x4f0d)
SFRX(AX5043_POWCTRL1,           0x4f08)
SFRX(AX5043_MODCFGP,            0x4f5f)
SFRX(AX5043_XTALOSC,              0x4f10)
SFRX(AX5043_XTALAMPL,              0x4f11)


// power states
#define AX5043_PWRSTATE_POWERDOWN           0x0
#define AX5043_PWRSTATE_DEEPSLEEP           0x1
#define AX5043_PWRSTATE_REGS_ON           	0x4
#define AX5043_PWRSTATE_XTAL_ON           	0x5
#define AX5043_PWRSTATE_FIFO_ON           	0x7
#define AX5043_PWRSTATE_SYNTH_RX            0x8
#define AX5043_PWRSTATE_FULL_RX             0x9
#define AX5043_PWRSTATE_WOR_RX           	0xb
#define AX5043_PWRSTATE_SYNTH_TX            0xc
#define AX5043_PWRSTATE_FULL_TX             0xd

//fifo commands
#define AX5043_FIFOCMD_NOP			0x00
#define AX5043_FIFOCMD_DATA			0x01
#define AX5043_FIFOCMD_REPEATDATA	0x02
#define AX5043_FIFOCMD_TIMER		0x10
#define AX5043_FIFOCMD_RSSI			0x11
#define AX5043_FIFOCMD_FREQOFFS		0x12
#define AX5043_FIFOCMD_RFFREQOFFS	0x13
#define AX5043_FIFOCMD_DATARATE		0x14
#define AX5043_FIFOCMD_ANTRSSI		0x15
#define AX5043_FIFOCMD_TXCTRL		0x1C
#define AX5043_FIFOCMD_TXPWR		0x1D


void axradio_wait_for_xtal(void);

__reentrantb void ax5043_receiver_on_continuous(void) __reentrant;
//__reentrantb void ax5043_receiver_on_wor(void) __reentrant;
__reentrantb void ax5043_off(void) __reentrant;
__reentrantb void ax5043_off_xtal(void) __reentrant;
__reentrantb void ax5043_prepare_tx(void) __reentrant;

// funtions implemented in AX_Radio_output/config.c
__reentrantb int32_t axradio_conv_freq_fromreg(int32_t f) __reentrant;
__reentrantb void ax5043_set_registers(void) __reentrant;
__reentrantb void ax5043_set_registers_tx(void) __reentrant;
__reentrantb void ax5043_set_registers_rx(void) __reentrant;
//__reentrantb void ax5043_set_registers_rxwor(void) __reentrant;
__reentrantb void ax5043_set_registers_rxcont(void) __reentrant;
__reentrantb void ax5043_set_registers_rxcont_singleparamset(void) __reentrant;
__reentrantb uint16_t axradio_framing_check_crc(uint8_t __xdata *pkt, uint16_t cnt) __reentrant;
__reentrantb uint16_t axradio_framing_append_crc(uint8_t __xdata *pkt, uint16_t cnt) __reentrant;



// physical layer
extern  uint8_t __xdata axradio_phy_pn9;
extern  uint8_t __xdata axradio_phy_nrchannels;
extern  uint32_t __xdata axradio_phy_chanfreq[];
extern  uint8_t __xdata axradio_phy_chanpllrnginit[];
extern  uint8_t __xdata axradio_phy_chanvcoiinit[];
extern uint8_t __xdata axradio_phy_chanpllrng[];
extern uint8_t __xdata axradio_phy_chanvcoi[];
extern  uint8_t __xdata axradio_phy_vcocalib;
extern  uint8_t __xdata axradio_phy_innerfreqloop;
extern  int32_t __xdata axradio_phy_maxfreqoffset;
extern  int8_t __xdata axradio_phy_rssioffset;
extern  int8_t __xdata axradio_phy_rssireference;
extern  int8_t __xdata axradio_phy_channelbusy;
extern  uint8_t __xdata axradio_phy_cs_enabled;
extern  uint16_t __xdata axradio_phy_cs_period;
extern  uint8_t __xdata axradio_phy_lbt_retries;
extern  uint8_t __xdata axradio_phy_lbt_forcetx;


extern  uint16_t __xdata axradio_phy_preamble_wor_longlen;
extern  uint16_t __xdata axradio_phy_preamble_longlen;
extern  uint16_t __xdata axradio_phy_preamble_wor_len;
extern  uint16_t __xdata axradio_phy_preamble_len;
extern  uint8_t __xdata axradio_phy_preamble_byte;
extern  uint8_t __xdata axradio_phy_preamble_flags;
extern  uint8_t __xdata axradio_phy_preamble_appendbits;
extern  uint8_t __xdata axradio_phy_preamble_appendpattern;

// framing
extern  uint8_t __xdata axradio_framing_lenpos;
extern  uint8_t __xdata axradio_framing_lenoffs;
extern  uint8_t __xdata axradio_framing_lenmask;
extern  uint8_t __xdata axradio_framing_destaddrpos;
extern  uint8_t __xdata axradio_framing_sourceaddrpos;

extern  uint8_t __xdata axradio_framing_synclen;
extern  uint8_t __xdata axradio_framing_syncword[];
extern  uint8_t __xdata axradio_framing_syncflags;
extern  uint8_t __xdata axradio_framing_enable_sfdcallback;
extern  uint8_t __xdata axradio_framing_swcrclen;


extern  uint32_t __xdata axradio_framing_ack_timeout;
extern  uint32_t __xdata axradio_framing_ack_delay;
extern  uint8_t __xdata axradio_framing_ack_retransmissions;
extern  uint8_t __xdata axradio_framing_ack_seqnrpos;

extern  uint8_t __xdata axradio_framing_minpayloadlen;

//WOR
extern  uint16_t __xdata axradio_wor_period;

// synchronous mode
extern  uint32_t __xdata axradio_sync_period;
extern  uint32_t __xdata axradio_sync_xoscstartup;

extern  uint32_t __xdata axradio_sync_slave_syncwindow;
extern  uint32_t __xdata axradio_sync_slave_initialsyncwindow;
extern  uint32_t __xdata axradio_sync_slave_syncpause;
extern  int16_t __xdata axradio_sync_slave_maxperiod;
extern  uint8_t __xdata axradio_sync_slave_resyncloss;
extern  uint8_t __xdata axradio_sync_slave_nrrx;
extern  uint32_t __xdata axradio_sync_slave_rxadvance[];
extern  uint32_t __xdata axradio_sync_slave_rxwindow[];
extern  uint32_t __xdata axradio_sync_slave_rxtimeout;

void __xdata *memset_xdata(void __xdata *p, uint8_t x, uint16_t l);
void __xdata *memcpy_xdata(void __xdata *p, const void __xdata *ps, uint16_t l);
void __xdata *memcpy_xdatageneric(void __xdata *p, const void __generic *ps, uint16_t l);
void __generic *memcpy_genericxdata(void __generic *p, const void __xdata *ps, uint16_t l);


#endif /* EASYAX5043_H */
