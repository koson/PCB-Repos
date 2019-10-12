#include <ax8052f143.h>
#include <libmftypes.h>
#include <libmfradio.h>
#include <libmfflash.h>
#include <libmfwtimer.h>
#include <libmfcrc.h>
#include "misc.h"
#include "defines.h"
#include "nbfi.h"
#include "application.h"
#include "slip.h"
#include "hal.h"
#include "waviotdvk.h"
#include "time.h"
#include "nbfi_config.h"
#include "m200.h"

uint8_t __xdata stay_awake = 0;

uint8_t __xdata unb_pckt[]= {1,2,3,4,5,6,7,8};//test packet

struct wtimer_desc __xdata everysecond_desc;
struct wtimer_desc __xdata checkHall_desc;

void EverySecond(struct wtimer_desc __xdata *desc);

extern uint16_t __xdata SW_IN;

#define HALL_OUT           PORTC_0
#define HALL_OUT_PORT      PORTC
#define HALL_OUT_DIR       DIRC
#define HALL_OUT_PIN       0

#define HALL_IN            PINC_2
#define HALL_IN_DIR        DIRC
#define HALL_IN_PIN        2

void checkHall(struct wtimer_desc __xdata *desc)
{
    static uint8_t hall_state;
    static uint8_t fast_count;

    switch(hall_state)
    {
    case 0:
    {
        HALL_OUT = 1;
        hall_state = 1;
        ScheduleTask(&checkHall_desc, checkHall, RELATIVE, MILLISECONDS(150));
        break;
    }
    case 1:
    {
        SW_IN = HALL_IN;
        HALL_OUT = 0;
        hall_state = 0;

        if(!SW_IN)
            fast_count = 30;

        if (fast_count)
        {
            ScheduleTask(&checkHall_desc, checkHall, RELATIVE, MILLISECONDS(300));
            fast_count--;
        }
        else
            ScheduleTask(&checkHall_desc, checkHall, RELATIVE, SECONDS(10));
        break;
    }
    default:
    {
        ScheduleTask(&checkHall_desc, checkHall, RELATIVE, MILLISECONDS(300));
        hall_state = 0;
        break;
    }
    }
}

void GPIO_Init()
{
    #ifdef USPD
    PORTA = 0x05;
    DIRA = 0x3B;
    PORTB = 0x00;
    DIRB = 0xFF;
    PORTC |= 0x04;
    PORTC &= 0x05;
    DIRC = 0x1B;
    #endif

    PORTR = 0x0B;
    DIRR = 0x15;

}

void GPIO_Deinit()
{
    #ifdef USPD
    PORTA = 0x01;
    DIRA = 0x3F;
    PORTB = 0x00;
    DIRB = 0xFF;
    PORTC |= 0x04;
    PORTC &= 0x05;
    DIRC = 0x1B;
    PALTA = 0; // Alternative function
    #endif

}

void ColdSetup()
{
    GPIO_Init();

    #ifdef OSCCALONSTART
    RF_Init(OSC_CAL, 0, 0, 0);
    #endif

    #ifndef NO_SLIP
    UART_Init();
    SLIP_Init();
    #endif

    M200_Init();

    NBFI_Init(&RX_Callback);
    delay_ms(1);
}

void WarmSetup()
{
    ax5043_commsleepexit();
    IE_4 = 1; // enable radio interrupt
    #ifndef NO_SLIP
    if (SLIP_isCanSleep()) {
        delay_ms(1);
    }
    UART_Init();
    #endif
}

bool power_off;
void Loop()
{
    if(!CheckTask(&checkHall_desc))
        checkHall(0);
    M200_Loop();
}

void RX_Callback(uint8_t __generic* data, uint16_t length)
{
    M200_RX_Callback(data, length);
}

