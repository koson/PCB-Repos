#ifndef ZIGZAG_H
#define ZIGZAG_H

#include <ax8052f143.h>
#include <libmftypes.h>

#define ZIGZAG_LEN 16

void ZIGZAG_Append(uint8_t __xdata* src_buf, uint8_t __xdata* dst_buf, bool parity);

#endif // ZIGZAG_H
