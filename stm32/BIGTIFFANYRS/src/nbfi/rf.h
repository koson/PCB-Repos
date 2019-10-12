#ifndef RF_H
#define RF_H

#include "nbfi.h"

#define PA_DIFFERENTIAL (0x1)
#define PA_SINGLE_ENDED (0x2)
#define PA_SHAPING      (0x4)

typedef enum
{
    STATE_OFF,
    STATE_RX,
    STATE_TX,
    STATE_CHANGED
}nbfi_rf_state_s;


extern bool send_auto_corr;
extern uint8_t PSK_BAND;
extern uint8_t hp0, hp1;
extern nbfi_rf_state_s rf_state;
#endif // NBFI_H
