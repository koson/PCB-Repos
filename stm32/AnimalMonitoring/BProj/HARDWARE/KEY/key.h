#ifndef __KEY_H
#define __KEY_H
#include "sys.h"

// Local use
#define KEY1     GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_0)     //PE0
#define KEY2     GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_1)     //PE1
#define KEY_WKUP GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)     //PA0
// Local use
#define KEY1_PRES 1
#define KEY2_PRES 2
#define KEY_WKUP_PRES 3
// local use
void KEY_Init(void);
u8 KEY_Scan(u16 divtime);

// Global use
extern uint8_t key1_down;
extern uint8_t key2_down;
extern uint8_t key_wkup_down;
// Global use
void KeyScan_Task(void *p_arg);
#endif
