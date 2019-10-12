/** \file MASTER\main.c
*
* \brief Code skeleton for MASTER module, illustrating periodic or on-demand transmission of packets.
* \brief The packet format is determined by AX-RadioLAB_output\config.c, produced by the AX-RadioLab GUI
*
*/

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
//#define USE_DBGLINK
#include <ax8052f143.h>
#include <libmftypes.h>
#include <libmfradio.h>
#include <libmfflash.h>
#include <libmfwtimer.h>

#include <string.h>

#include "misc.h"

#include "application.h"
#include "slip.h"

#include "hal.h"
#include "nbfi.h"
#include "nbfi_config.h"
#include "rf.h"
#include "m200.h"



uint8_t __data coldstart = 1; // caution: initialization with 1 is necessary! Variables are initialized upon _sdcc_external_startup returning 0 -> the coldstart value returned from _sdcc_external startup does not survive in the coldstart case

static void pwrmgmt_irq(void) __interrupt(INT_POWERMGMT)
{
    uint8_t pc = PCON;
    if (!(pc & 0x80))
        return;
    GPIOENABLE = 0;
    IE = EIE = E2IE = 0;
    for (;;)
        PCON |= 0x01;
}

#ifndef NO_SLIP
#pragma vector=0x1B
__interrupt void gpio_irq(void)
{
    IE_3 = 0;
    SLIP_setCanSleep(0);
}
#endif // NO_SLIP

void axradio_statuschange(struct axradio_status __xdata *st)
{
    switch (st->status)
    {
    case AXRADIO_STAT_TRANSMITSTART:
        axradio_set_channel(0);
        if( st->error == AXRADIO_ERR_TIMEOUT )  rf_busy = false;
        break;

    case AXRADIO_STAT_TRANSMITEND:
        //PORTB_0 = 0;
        rf_busy = false;
        NBFi_TX_Finished();
        break;

    case AXRADIO_STAT_RECEIVE:
        if(st->error == AXRADIO_ERR_NOERROR)
        {
            NBFi_ParseReceivedPacket(st);
        }
        break;

    default:
        break;
    }
}

#if defined(__ICC8051__)
//
// If the code model is banked, low_level_init must be declared
// __near_func elsa a ?BRET is performed
//
#if (__CODE_MODEL__ == 2)
__near_func __root char
#else
__root char
#endif
__low_level_init(void) @ "CSTART"
#else
uint8_t _sdcc_external_startup(void)
#endif
{
    LPXOSCGM = 0x8A;
#ifdef LOWFREQ_RC
    wtimer0_setclksrc(CLKSRC_LPOSC, 1);
#elif defined LOWFREQ_QUARZ
    wtimer0_setclksrc(CLKSRC_LPXOSC, 3); // 8192 hz
#endif // LOWFREQ_RC
    wtimer1_setclksrc(CLKSRC_FRCOSC, 7);

    LPOSCCONFIG = 0x09; // Slow, PRESC /1, no cal. Does NOT enable LPOSC. LPOSC is enabled upon configuring WTCFGA (MODE_TX_PERIODIC and receive_ack() )

    coldstart = !(PCON & 0x40);

    ANALOGA = 0x18; // PA[3,4] LPXOSC, other PA are used as digital pins

    axradio_setup_pincfg1();
    DPS = 0;
    IE = 0x40;
    EIE = 0x00;
    E2IE = 0x00;
    GPIOENABLE = 1; // unfreeze GPIO

    return coldstart;
}

void main(void)
{
    EA = 1;

    if(!coldstart)
    {
        uint8_t lp0 = LPOSCFREQ0;
        uint8_t lp1 = LPOSCFREQ1;
        flash_apply_calibration();
        LPOSCFREQ1 = lp1;
        LPOSCFREQ0 = lp0;
        if(nbfi.additional_flags&NBFI_FLG_DO_OSCCAL)
        {
            FRCOSCFREQ0 = hp0;
            FRCOSCFREQ1 = hp1;
        }
    }
    else  flash_apply_calibration();

    CLKCON = 0x00;
    wtimer_init();

    if (coldstart)
    {
//        flash_apply_calibration();
        ColdSetup(); // after power-up
    }
    else
    {
        WarmSetup(); // after wake-up from sleep
    }


    IE_3 = 0;

    axradio_setup_pincfg2();

#ifndef NO_SLIP
    #ifdef AMPER_2_3
    SLIP_Wait_Before_Sleep();
    #else
    SLIP_setCanSleep(0);
    //if (!CheckTask(&uart_sleep_timer)) SLIP_Wait_Before_Sleep();
    #endif
#endif

    for(;;)
    {
    //DIRC |= 2;
    //PORTC_1 = 1;
        #ifndef NO_SLIP
        SLIP_Check_For_Correct_State();
        #endif
        Loop();
        wtimer_runcallbacks();


        EA = 0;
        uint8_t flg = WTFLAG_CANSTANDBY;

        #ifndef NO_SLEEP
        if (axradio_cansleep() && (!rf_busy) && (rf_state == STATE_OFF) && M200_isCanSleep())
        {
            GPIO_Deinit();
            INTCHGC = 1 << 4;
            do { char tmp_pinA = PINC; } while(0);
            IE_3 = 1;

            #ifndef NO_SLIP
            if(SLIP_isCanSleep())
            {
                SLIP_setCanSleep(1);
                GPIO_Deinit();
                INTCHGA = 1 << 2;
                INTCHGB = 0;
                INTCHGC = 0;
                do { char tmp_pinA = PINA; } while(0);
                IE_3 = 1;
                #ifndef NO_SLEEP
                flg |= WTFLAG_CANSLEEP;
                #endif
            }
            #else // NO_SLIP

            #ifndef NO_SLEEP
            flg |= WTFLAG_CANSLEEP;
            #endif

            #endif
        }
        #endif

        wtimer_idle(flg);
        IE_3 = 0; // no ISR!
        EA = 1;
    }
}
