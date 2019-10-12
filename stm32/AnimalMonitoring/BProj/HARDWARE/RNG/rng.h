#ifndef __RNG_H
#define __RNG_H	 
#include "sys.h"
u8 rng_Init(void);
u32 rngGetRandomNum(void) ;
int rngGetRandomRange(int min, int max);
#endif
