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

#include <ax8052.h>
#include <libmftypes.h>
#include <libmfwtimer.h>
//#include <libaxlcd2.h>

#include "misc.h"
#include "axradio.h"
//
//struct errtbl {
//    uint8_t errcode;
//    const char __code *msg;
//};
//static const struct errtbl __code errtbl[] = {
//    { AXRADIO_ERR_NOTSUPPORTED, "E: not supported" },
//    { AXRADIO_ERR_BUSY, "E: busy" },
//    { AXRADIO_ERR_TIMEOUT, "E: timeout" },
//    { AXRADIO_ERR_INVALID, "E: invalid" },
//    { AXRADIO_ERR_NOCHIP, "E: no chip found" },
//    { AXRADIO_ERR_RANGING, "E: ranging" },
//    { AXRADIO_ERR_LOCKLOST, "E: lock lost" },
//    { AXRADIO_ERR_NOERROR, 0 }
//};

static struct wtimer_desc __xdata delaymstimer;

static void delayms_callback(struct wtimer_desc __xdata *desc)
{
    desc;
    delaymstimer.handler = 0;
}

//__reentrantb void delay_ms(uint16_t ms) __reentrant
//{
//    // scaling: 20e6/64/1e3=312.5=2^8+2^6-2^3+2^-1
//    uint32_t x;
//    wtimer_remove(&delaymstimer);
//    x = ms;
//    delaymstimer.time = ms >> 1;
//    x <<= 3;
//    delaymstimer.time -= x;
//    x <<= 3;
//    delaymstimer.time += x;
//    x <<= 2;
//    delaymstimer.time += x;
//    delaymstimer.handler = delayms_callback;
//    wtimer1_addrelative(&delaymstimer);
//    wtimer_runcallbacks();
//    do {
//        wtimer_idle(WTFLAG_CANSTANDBY);
//        wtimer_runcallbacks();
//    } while (delaymstimer.handler);
//}

__reentrantb void delay_ms(uint16_t ms) __reentrant
{
    // scaling: 20e6/64/1e3=312.5=2^8+2^6-2^3+2^-1
    uint32_t x;

    x = ms;
    delaymstimer.time = ms >> 1;
    x <<= 3;
    delaymstimer.time -= x;
    x <<= 3;
    delaymstimer.time += x;
    x <<= 2;
    delaymstimer.time += x;

    do {
        delaymstimer.time--;
    }
    while (delaymstimer.time);
}
