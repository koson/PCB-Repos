#ifndef _M200_SERIAL_H
#define _M200_SERIAL__H
#include <ax8052f143.h>
#include <libmftypes.h>
#include "m200_settings.h"

#define P5V_EN          PORTA_0
#define P5V_EN_PORT     PORTA
#define P5V_EN_DIR      DIRA
#define P5V_EN_PIN      0

#define RXTX            PORTA_1
#define RXTX_PORT       PORTA
#define RXTX_DIR        DIRA
#define RXTX_PIN        1

enum{
    M200_MODE_START,
    M200_MODE_RECEIVING_PAYLOAD,
    M200_MODE_RECEIVING_CMD,
    M200_MODE_TIMEOUT,
    M200_MODE_ERR
};

typedef struct
{
    uint8_t cmd;
    uint8_t payload[270];
    uint16_t len;
}m200_buf_t;

extern __xdata m200_buf_t m200_rxbuf;
extern bool rs_free;

void power5V_on_off(bool on_off);
void M200_rxtx(bool rx_tx);
void M200_Send(uint8_t cmd, uint8_t __generic * payload, uint16_t len);
void M200_Send_RAW(uint8_t __generic * payload, uint16_t len);
uint8_t M200_Receive();
void M200_Process(struct wtimer_desc __xdata *desc);
void M200_Process_RAW(struct wtimer_desc __xdata *desc);
#endif
