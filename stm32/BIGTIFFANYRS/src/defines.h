#ifndef _DEFINES_H
#define _DEFINES_H

#define NBFI_REV        2
#define NBFI_SUBREV     1

#define UL868800_DL446000            0
#define UL868800_DL864000            1
#define UL868800_DL446000_DL864000   2
#define UL867950_DL446000            3
#define UL868500_DL446000            4
#define UL868100_DL446000            5
#define UL864000_DL446000            6
#define UL863175_DL446000            7
#define UL864000_DL875000            8 //GLONASS

#define UL868500_DL864000            10

#ifdef UL864000_DL875000
#define GLONASS
#endif

#ifdef USPD
    #define NO_SLIP
    #define OSCCALONSTART
    #define HARDWARE_ID     7   //7 = uspd
    #define HARDWARE_REV    0
    #define RF_MAX_POWER 15
    #define RF_MIN_POWER 0
    #define BAND_ID         UL868800_DL864000//UL868800_DL864000
#endif

#ifdef NOKEYDL
#define NBFI_REV        1
#endif

#endif // _DEFINES_H
