#ifndef WAVIOTDVK_H
#define WAVIOTDVK_H

#include <ax8052f143.h>
#include <libmftypes.h>
#include <libmfwtimer.h>

#define SW1     (1)
#define SW2     (2)
#define SW3     (4)


void CheckButtons(struct wtimer_desc __xdata *desc);
void Buttons_Init();
uint8_t GetButton1();
uint8_t GetButton2();
uint8_t GetButton3();
void Backlight(uint8_t enable);

#endif // WAVIOTDVK_H
