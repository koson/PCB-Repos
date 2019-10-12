#ifndef __MQTTTRANSPORT_H
#define __MQTTTRANSPORT_H
#include "sys.h"
int transport_sendPacketBuffer(char* buf, u32 buflen);
int transport_getdata(char* buf, u32 count);
int transport_open(void);
int transport_close(void);
#endif
