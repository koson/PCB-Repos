#include "waviotdvk.h"
#include "hal.h"

uint8_t __xdata button_events = 0;
struct wtimer_desc __xdata buttons_desc;

uint32_t __xdata buttons_v = 0;
uint32_t __xdata VDD = 0;

uint32_t ADC_ReadButtons()
{
    uint32_t v;

    // Init ADC
    ANALOGA |= 0x01; //PA0 - disable digital buffer
    ADCTUNE0 = 0x01;
    ADCTUNE1 = 0x06;
    ADCTUNE2 = 0x00;
    ADCCLKSRC = 0x30; // FRCOSC
    ADCCH0CONFIG = 0xC8; //Buttons (Single ended A0)
    ADCCH1CONFIG = 0xD9;    //VddIO
    ADCCH2CONFIG = 0xD8;    //Temperature

    // Make conversion
   // EA = 0;
    ADCCONV = 0x01;
    while(ADCCLKSRC&0x80);
    v = ADCCH0VAL0 + (((uint16_t)ADCCH0VAL1)<<8);
    VDD = ADCCH1VAL0 + (((uint16_t)ADCCH1VAL1)<<8);
    //EA = 1;

    return v;
}

// Returns 0 if no button, 1 if S1, 2 if S2 is pressed right now
// Also sets events in buttons_int

void CheckButtons(struct wtimer_desc __xdata *desc)
{
    if(PINB_3 == 0)
    {
        button_events |= SW3;
        ScheduleTask(desc, 0, RELATIVE, MILLISECONDS(350));
        return;
    }

    buttons_v = ADC_ReadButtons();

    if (buttons_v > 62500)
    {
        ScheduleTask(desc, 0, RELATIVE, MILLISECONDS(30));
        return;
    }
    else if (buttons_v > 30000)
    {
        button_events |= SW1;
    }
    else
    {
        button_events |= SW2;
    }

    ScheduleTask(desc, 0, RELATIVE, MILLISECONDS(350));
}

void Buttons_Init()
{
    PIN_SET_INPUT(DIRB, 3);
    PIN_SET_HIGH(PORTB, 3);
    ScheduleTask(&buttons_desc, &CheckButtons, RELATIVE, MILLISECONDS(100));
}

uint8_t GetButton1()
{
    uint8_t button_state = button_events & SW1;
//    button_events &= ~SW1;
    if(button_state) button_events = 0;
    return button_state;
}

uint8_t GetButton2()
{
    uint8_t button_state = button_events & SW2;
//    button_events &= ~SW2;
    if(button_state) button_events = 0;
    return button_state;
}

uint8_t GetButton3()
{
    uint8_t button_state = button_events & SW3;
//    button_events &= ~SW3;
    if(button_state) button_events = 0;
    return button_state;
}

void Backlight(uint8_t enable)
{
    if(enable)
    {
        DIRC |=8;
        PORTC &=~8;
    }
    else
    {
        DIRC &=~8;
        PORTC &=~8;
    }
}

