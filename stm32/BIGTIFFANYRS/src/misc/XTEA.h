#ifndef XTEA_H
#define XTEA_H

#include <libmftypes.h>

typedef const uint32_t __code*  xtea_key_t;

#define XTEA_KEY_LENGTH 8   // In 32-bit words
#define XTEA_KEY_PTR ((const uint32_t __code*)0xfd04) // Pointer to KEY in flash

#define NUM_ITERATIONS 64

void XTEA_Encode(uint8_t __xdata *buf);
void XTEA_Decode(uint8_t __xdata *buf);

bool XTEA_Available();
bool XTEA_Enabled();
void XTEA_Enable(bool enable);

#endif
