#ifndef _M200_H
#define _M200_H
#include <ax8052f143.h>
#include <libmftypes.h>
#include "m200_serial.h"

typedef struct {
    uint8_t timestamp_flag : 1;
    uint8_t obis_C : 7; //param + phase
    uint8_t rezerv : 1;
    uint8_t obis_E : 3; //tariff
    uint8_t obis_B : 2; //channel = 0
    uint8_t obis_A : 1;
    uint8_t obis_D : 1; //cur = 0, intgr = 1
} PARAM_IDENTIFIER;

void M200_Init();
bool M200_isCanSleep();
void M200_Report_Regular(uint8_t __xdata* buf);
void M200_Stamp();
void M200_RX_Callback(uint8_t __generic* data, uint16_t length);
void M200_WriteConfig();
void M200_Loop();
void M200_Send_Event(uint8_t event, uint32_t time, uint16_t data);
void M200_Write_Tarif_Packet(TARIF_ZONE *zone, uint16_t address);
void M200_Write_Tarif_Main(struct wtimer_desc __xdata *desc);
#endif
