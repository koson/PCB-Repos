#ifndef __ADC_H
#define __ADC_H
#include "sys.h"

// #define ADC_CH1 ADC_Channel_0
#define ADC_CH_TP ADC_Channel_16

void adc_Init(void);
u16 adcGetChValue(u8 ch);
u16 adcGetChValueAvg(u8 ch, u8 times);
#endif
