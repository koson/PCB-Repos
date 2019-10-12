#ifndef __LED_H
#define __LED_H
#include "sys.h"
// Local use
#define ledGREEN PFout(9)
#define ledYELLOW PFout(10)
#define ledBLUE PFout(10)

// Global use
void LED_Init(void);

void LED_GREEN_ON(void);
void LED_GREEN_OFF(void);
void LED_GREEN_NOT(void);

void LED_YELLOW_ON(void);
void LED_YELLOW_OFF(void);
void LED_YELLOW_NOT(void);

void LED_BLUE_ON(void);
void LED_BLUE_OFF(void);
void LED_BLUE_NOT(void);
#endif
