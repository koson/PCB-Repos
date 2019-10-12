#include "nbfi.h"
#include "nbfi_config.h"
#include "rf.h"
#include "hal.h"
#include "misc.h"
#include "waviotdvk.h"
#include <libmfwtimer.h>

bool __xdata rf_busy = 0;
struct axradio_address rf_destination;

nbfi_rf_state_s rf_state = STATE_RX;

void calibrate_oscillators(void);


uint8_t PSK_BAND;

void RF_SetFreq(uint32_t freq)
{
    axradio_phy_chanfreq[0] = axradio_conv_freq_fromhz(freq);//TODO fix freq calc
    // Dirty fix for insufficient arithmetics precision
    if(freq > 800000000)    axradio_phy_chanfreq[0]+=34; //868MHz
    else                    axradio_phy_chanfreq[0]+=18; //446MHz
}


#define FEM_MAX_DBM 26
#define FEM_GAIN 11
#define AX5043_MAX_DBM 15
//#define TX_RX_SEL PORTC_0

//#ifdef SDR_RASP
//const uint16_t AX5043_power[26] = {0x00aa, 0x00bf, 0x00d1, 0x00ec, 0x010f, 0x0132, 0x0156, 0x017f, 0x01af, 0x1e0, 0x207, 0x244, 0x290, 0x2eb, 0x35e, 0x3d6, 0x406, 0x4a9, 0x57c, 0x600, 0x700, 0x800, 0x9d4, 0xc00, 0xf00, 0xfff};
//#endif // DEVICE_SDR

#ifdef SDR_BS3
const uint16_t power_table[]={5, 6, 6, 7, 8, 8, 8, 9, 9, 10, 10, 11, 11, 12, 13, 14, 14, 15, 16, 18, 19, 21, 22, 24, 27, 30, 33, 37, 42, 47, 54, 61, 68, 75, 81, 87, 97, 110, 126, 142, 157, 172, 197, 239};//-15..28dbm

/*------864 MHz DL ---------*/
const uint16_t power_table_864[]={21, 22, 23, 24, 25, 26, 27, 30, 33, 36, 42, 48, 57, 63, 70, 74, 81, 85, 97, 110, 120, 130, 150, 170, 190, 200, 230, 260, 290, 330, 380, 420, 470, 510, 530, 560, 630, 720, 850, 930, 990, 1030, 1030, 1030};//-15..28dbm

bool send_auto_corr = 0;
#endif // SDR_BS3


#ifdef DEVKIT
const uint16_t RFM6599_PowerTable[26] = {0x00aa, 0x00bf, 0x00d1, 0x00ec, 0x010f, 0x0132, 0x0156, 0x017f, 0x01af, 0x1e0, 0x207, 0x244, 0x290, 0x2eb, 0x35e, 0x3d6, 0x406, 0x4a9, 0x57c, 0x600, 0x700, 0x800, 0x9d4, 0xc00, 0xf00, 0xfff};
#endif // DEVICE

//#ifdef AMPER
const uint16_t AX5043_power[26] = {0x00aa, 0x00bf, 0x00d1, 0x00ec, 0x010f, 0x0132, 0x0156, 0x017f, 0x01af, 0x1e0, 0x207, 0x244, 0x290, 0x2eb, 0x35e, 0x3d6, 0x406, 0x4a9, 0x57c, 0x600, 0x700, 0x800, 0x9d4, 0xc00, 0xf00, 0xfff};
//#endif // AMPER_2_3



struct axradio_address  fastdladdress = {
	{ 0x6f, 0x6f, 0x6f, 0x6f}
};


void RF_SetModeAndPower(int8_t dBm, rf_direction_t mode, rf_antenna_t ant)
{

    if(dBm > RF_MAX_POWER) dBm = RF_MAX_POWER;
    if(dBm < RF_MIN_POWER) dBm = RF_MIN_POWER;


    if(dBm > FEM_MAX_DBM) dBm = FEM_MAX_DBM;



    if(mode == TX)
    {
        #if defined(SDR_RASP) || defined(AMPER) || defined (BIGTIFFANY)
        PIN_SET_OUTPUT(PA_EN_DIR,  PA_EN_PIN);
        PA_EN = 1;
        #endif
        #ifdef AMPER_2_1
        PIN_SET_OUTPUT(PA_EN_DIR,  PA_EN_PIN);
        if(dBm <= AX5043_MAX_DBM)
        {
            // set power
            AX5043_TXPWRCOEFFB1 = AX5043_power[dBm + 10] >> 8;
            AX5043_TXPWRCOEFFB0 = AX5043_power[dBm + 10] &0xFF;

            // select differential PA
            AX5043_MODCFGA = 0x1;//5

            // FEM PA bypass
            TX_RX_SEL = RX;
            DIRC |= 1;
            PA_EN = 0;
        }
        else
        {
            // set power, considering FEM gain
            AX5043_TXPWRCOEFFB1 = AX5043_power[dBm - FEM_GAIN + 10] >> 8;
            AX5043_TXPWRCOEFFB0 = AX5043_power[dBm - FEM_GAIN + 10] &0xFF;

            // select single-ended PA
            AX5043_MODCFGA = 0x2;//6

            //enable FEM PA
            TX_RX_SEL = TX;
            DIRC |= 1;
            PA_EN = 1;
        }
        #endif
        #ifdef AMPER_2_2
        PIN_SET_OUTPUT(PA_EN_DIR,  PA_EN_PIN);

        if(dBm <= AX5043_MAX_DBM)
        {
            // set power
            AX5043_TXPWRCOEFFB1 = AX5043_power[dBm + 10] >> 8;
            AX5043_TXPWRCOEFFB0 = AX5043_power[dBm + 10] &0xFF;

            // select differential PA
            AX5043_MODCFGA = 0x1;//5

            // FEM PA bypass
            DIRC &= ~0x0c;
            TX_RX_SEL = RX;
            TX_RX_SEL_2 = RX;
            TX_RX_SEL_3 = RX;
            DIRC |= 0x0c;

            PA_EN = 0;
        }
        else
        {
            // set power, considering FEM gain
            AX5043_TXPWRCOEFFB1 = AX5043_power[dBm - FEM_GAIN + 10] >> 8;
            AX5043_TXPWRCOEFFB0 = AX5043_power[dBm - FEM_GAIN + 10] &0xFF;

            // select single-ended PA
            AX5043_MODCFGA = 0x2;//6

            //enable FEM PA
            DIRC &= ~0x0c;
            TX_RX_SEL = TX;
            TX_RX_SEL_2 = TX;
            TX_RX_SEL_3 = TX;
            DIRC |= 0x0c;
            PA_EN = 1;
        }
        #endif

        #ifdef AMPER_2_3
        if(dBm > AX5043_MAX_DBM) dBm = AX5043_MAX_DBM;
        AX5043_TXPWRCOEFFB1 = AX5043_power[dBm + 10] >> 8;
        AX5043_TXPWRCOEFFB0 = AX5043_power[dBm + 10] &0xFF;
        switch(nbfi.tx_phy_channel)
        {
            case UL_PSK_FASTDL:
            case UL_PSK_200:
            case UL_PSK_500:
            case UL_PSK_5000:
                // select differential PA
                AX5043_MODCFGA = PA_DIFFERENTIAL;
            break;
            default:
                // select single-ended PA
                AX5043_MODCFGA = PA_SINGLE_ENDED;
                break;
        }

        #endif
        #if defined(SDR_RASP) || defined (USPD) || defined(USB_DONGLE)
        PIN_SET_OUTPUT(PA_EN_DIR,  PA_EN_PIN);
        PA_EN = 1;
        // set power
        AX5043_MODCFGA = PA_DIFFERENTIAL;
        AX5043_TXPWRCOEFFB1 = AX5043_power[dBm+10] >> 8;
        AX5043_TXPWRCOEFFB0 = AX5043_power[dBm+10] &0xFF;
        #endif

        #ifdef BIGTIFFANY
        // set power
        #ifdef SINGLE_ENDED
        AX5043_MODCFGA = PA_SINGLE_ENDED;
        #else
        AX5043_MODCFGA = PA_DIFFERENTIAL;
        #endif
        AX5043_TXPWRCOEFFB1 = AX5043_power[dBm+10] >> 8;
        AX5043_TXPWRCOEFFB0 = AX5043_power[dBm+10] &0xFF;
        #endif

        #ifdef BIGTIFFANY_AX
        // set power
        AX5043_MODCFGA = PA_DIFFERENTIAL;
        AX5043_TXPWRCOEFFB1 = AX5043_power[dBm+10] >> 8;
        AX5043_TXPWRCOEFFB0 = AX5043_power[dBm+10] &0xFF;
        #endif

        #ifdef NEPTUN
                // set power
        AX5043_TXPWRCOEFFB1 = AX5043_power[dBm + 10] >> 8;
        AX5043_TXPWRCOEFFB0 = AX5043_power[dBm + 10] &0xFF;
        // select differential PA
        AX5043_MODCFGA = PA_DIFFERENTIAL;

        #endif // NEPTUN
        #ifdef SDR_BS3
        PIN_SET_OUTPUT(PA_SW1_DIR,  PA_SW1_PIN);
        PIN_SET_OUTPUT(PA_SW2_DIR,  PA_SW2_PIN);
        PIN_SET_OUTPUT(PA_450_EN_DIR,  PA_450_EN_PIN);
        PIN_SET_OUTPUT(PA_860_EN_DIR,  PA_860_EN_PIN);

        if(ant == PCB)
        {
            PA_SW1 = 1;
            PA_SW2 = 0;
            PA_450_EN = 1;
            PA_860_EN = 0;
            AX5043_MODCFGA = PA_SINGLE_ENDED;
            AX5043_TXPWRCOEFFB1 = power_table[dBm + 15] >> 8;
            AX5043_TXPWRCOEFFB0 = power_table[dBm + 15] &0xFF;
        }
        else
        {
            PA_SW1 = 0;
            PA_SW2 = 1;
            PA_450_EN = 0;
            if(send_auto_corr == 0)  PA_860_EN = 1;
            else PA_860_EN = 0;
            send_auto_corr = 0;
            AX5043_MODCFGA = PA_DIFFERENTIAL;
            AX5043_TXPWRCOEFFB1 = power_table_864[dBm + 15 - 6] >> 8;
            AX5043_TXPWRCOEFFB0 = power_table_864[dBm + 15 - 6] &0xFF;
        }

        #endif

        #ifdef DEVKIT

        PIN_SET_OUTPUT(PA_EN_DIR,  PA_EN_PIN);

        PIN_SET_OUTPUT(TX_ANT_DIR, TX_ANT_PIN);

        /*if(dBm <= AX5043_MAX_DBM)
        {
            // set power
            AX5043_TXPWRCOEFFB1 = RFM6599_PowerTable[dBm + 10] >> 8;
            AX5043_TXPWRCOEFFB0 = RFM6599_PowerTable[dBm + 10] &0xFF;

            // select PA output
            AX5043_MODCFGA = PA_DIFFERENTIAL;
            // FEM PA bypass
            //TX_RX_SEL = RX;
            PA_EN = HIGH;//LOW;
        }
        else
        {*/
        if(nbfi.tx_phy_channel == UL_PSK_FASTDL)
        {
            if(dBm > 15) dBm = 15;
            AX5043_TXPWRCOEFFB1 = AX5043_power[dBm+10] >> 8;
            AX5043_TXPWRCOEFFB0 = AX5043_power[dBm+10] &0xFF;
            AX5043_MODCFGA = PA_DIFFERENTIAL;
            PIN_SET_OUTPUT(RX_ANT_DIR, RX_ANT_PIN);
            RX_ANT = ant;
        }
        else
        {

            AX5043_TXPWRCOEFFB1 = RFM6599_PowerTable[dBm - FEM_GAIN + 10] >> 8;
            AX5043_TXPWRCOEFFB0 = RFM6599_PowerTable[dBm - FEM_GAIN + 10] &0xFF;

            // select single-ended PA
            AX5043_MODCFGA = PA_SINGLE_ENDED;
            //AX5043_MODCFGA = PA_DIFFERENTIAL;
            // enable FEM PA
            //TX_RX_SEL = TX;
            TX_ANT = ant;
            PA_EN = HIGH;
        }
            // set power, considering FEM gain

        //}
        #endif // DEVICE
    }
    else // mode == RX or IDLE
    {
        #if defined(SDR_RASP) || defined(AMPER) || defined (BIGTIFFANY) || defined(USPD) || defined(USB_DONGLE)
        PIN_SET_OUTPUT(PA_EN_DIR,  PA_EN_PIN);
        PA_EN = 0;
        #endif

        #ifdef AMPER_2_1
        PIN_SET_OUTPUT(PA_EN_DIR,  PA_EN_PIN);
         // select differential PA
        AX5043_MODCFGA = 0x5;

        // enable FEM PA bypass
        TX_RX_SEL = RX;
        DIRC |= 1;
        PA_EN = 0;
        #endif // AMPER_2_1

        #ifdef AMPER_2_2
        PIN_SET_OUTPUT(PA_EN_DIR,  PA_EN_PIN);
        // select differential PA
        AX5043_MODCFGA = 0x5;

        // enable FEM PA bypass
        DIRC &= ~0x0c;
        TX_RX_SEL = RX;
        TX_RX_SEL_2 = RX;
        TX_RX_SEL_3 = RX;
        DIRC |= 0x0c;
        PA_EN = 0;
        #endif // AMPER_2_2
        #ifdef AMPER_2_3
        AX5043_MODCFGA = PA_DIFFERENTIAL | PA_SHAPING;
        #endif

        #ifdef SDR_RASP
        PIN_SET_OUTPUT(PA_EN_DIR,  PA_EN_PIN);
        PA_EN = 0;
        AX5043_MODCFGA = PA_DIFFERENTIAL | PA_SHAPING;
        //PA_EN = 0;
        #endif

        #ifdef BIGTIFFANY
        AX5043_MODCFGA = PA_DIFFERENTIAL | PA_SHAPING;
        #endif
        #ifdef BIGTIFFANY_AX
        AX5043_MODCFGA = PA_DIFFERENTIAL | PA_SHAPING;
        #endif
        #ifdef NEPTUN
        AX5043_MODCFGA = PA_DIFFERENTIAL | PA_SHAPING;
        #endif

        #ifdef SDR_BS3
        PIN_SET_OUTPUT(PA_450_EN_DIR,  PA_450_EN_PIN);
        PIN_SET_OUTPUT(PA_860_EN_DIR,  PA_860_EN_PIN);
        PA_450_EN = 0;
        PA_860_EN = 0;
        #endif
        #ifdef DEVKIT
        PIN_SET_OUTPUT(RX_ANT_DIR, RX_ANT_PIN);
        RX_ANT = ant;

        // select PA output
        AX5043_MODCFGA = PA_DIFFERENTIAL | PA_SHAPING;

        // enable FEM PA bypass
        //TX_RX_SEL = RX;
        PA_EN = LOW;
        #endif // DEVICE
    }

}

nbfi_status_t RF_Init(  nbfi_phy_channel_t  phy_channel,
                        rf_antenna_t        antenna,
                        int8_t              power,
                        uint32_t            freq)
{
    uint8_t er;


    if(rf_busy) return ERR_RF_BUSY;

    //Backlight(1);

    rf_busy = true;

    if(phy_channel != OSC_CAL) nbfi_phy_channel = phy_channel;


    if(freq > 600000000) PSK_BAND = 1;
    else PSK_BAND = 0;

    PIN_SET_OUTPUT(TCXO_DIR, TCXO_PIN); // Enable TCXO...
    TCXO = HIGH;
    //delay_ms(10);   // ... and let it settle

    PORTR = 0x0B;
    DIRR = 0x15;

    switch(phy_channel)
    {
//    case UL_DBPSK_100:
//    case UL_DBPSK_50:
//    case UL_DBPSK_500:
    case UL_PSK_200:
    case UL_PSK_FASTDL:
    case UL_PSK_500:
    case UL_PSK_5000:
    case UL_DBPSK_50_PROT_C:
    case UL_DBPSK_50_PROT_D:
    case UL_DBPSK_400_PROT_C:
    case UL_DBPSK_400_PROT_D:
    case UL_DBPSK_3200_PROT_D:
        ax5043_set_constants();
        er = axradio_init();    // Init radio registers
        if (er != AXRADIO_ERR_NOERROR)
        {
            rf_busy = false; return ERR;
        }
        er = axradio_set_mode(AXRADIO_MODE_ASYNC_TRANSMIT);
        if (er != AXRADIO_ERR_NOERROR)
        {
            rf_busy = false;return ERR;
        }
        RF_SetFreq(freq);
        RF_SetModeAndPower(power, TX, antenna);
        rf_busy = false;
        rf_state = STATE_TX;
        return OK;


    case DL_PSK_200:
    case DL_PSK_FASTDL:
    case DL_PSK_500:
    case DL_PSK_5000:
        ax5043_set_constants();
        RF_SetModeAndPower(power, RX, antenna);
 /*       if((phy_channel == DL_PSK_FASTDL))
        {
            //RF_SetFreq(NBFI_FASTDL_FREQ_BASE);
            RF_SetLocalAddress((uint8_t __generic *)&fastdladdress);
        }*/

        RF_SetLocalAddress((uint8_t __generic *)&fastdladdress);
//        RF_SetLocalAddress((uint8_t __generic *)nbfi.temp_ID);

        RF_SetFreq(freq);

        er = axradio_init();    // Init radio registers
        if (er != AXRADIO_ERR_NOERROR)
        {
             rf_busy = false; return ERR;
        }
        er = axradio_set_mode(AXRADIO_MODE_ASYNC_RECEIVE);
        rf_busy = false;
        if (er != AXRADIO_ERR_NOERROR)
        {
            return ERR;
        }
        rf_state = STATE_RX;
        return OK;
    case OSC_CAL:
        axradio_set_mode(AXRADIO_MODE_ASYNC_RECEIVE);
        calibrate_oscillators();
        axradio_set_mode(AXRADIO_MODE_OFF);
        delay_ms(2);
        rf_state = STATE_OFF;
        //return OK;

    }
    TCXO = LOW;
    rf_busy = false;
    return ERR;
}

nbfi_status_t RF_Deinit()
{
    uint8_t er;
    if(rf_busy) return ERR_RF_BUSY;
    //Backlight(0);
    rf_busy = true;
    //if(nbfi.additional_flags&NBFI_FLG_DO_OSCCAL) calibrate_oscillators();
    er = axradio_set_mode(AXRADIO_MODE_OFF);
    rf_busy = false;
    RF_SetModeAndPower(0, RX, 0);
    delay_ms(1);
    TCXO = LOW;
    rf_state = STATE_OFF;
    if (er != AXRADIO_ERR_NOERROR) return ERR;
    return OK;
}

void RF_SetDstAddress(uint8_t __generic * addr)
{
    for(uint8_t i = 0; i !=3; i++) rf_destination.addr[i] = addr[i];
}


void RF_SetLocalAddress(uint8_t __generic * addr)
{
    struct axradio_address_mask     localaddress = {{0,0,0,0},{0xff, 0x00, 0x00, 0x00}};

    localaddress.addr[0] = addr[0];
    //localaddress.addr[1] = addr[1];
    //localaddress.addr[2] = addr[2];

    axradio_set_local_address(&localaddress);
}


//uint32_t ccc;
nbfi_status_t RF_Transmit(uint8_t __generic* pkt, uint8_t len,  rf_padding_t padding, rf_blocking_t blocking)
{
    if(rf_busy) return ERR_RF_BUSY;

    //ccc = (uint32_t) pkt;

    rf_busy = true;

    axradio_transmit(&rf_destination, pkt, len, padding);

    if(blocking == BLOCKING)
    {
        //uint8_t  iesave = IE & 0x80;
        while(1) // Wait for TX complete
        {
            //EA = 0;
            if(!rf_busy) break;
            //wtimer_idle(WTFLAG_CANSTANDBY);
            //IE |= iesave;
            wtimer_runcallbacks();
        }
        //IE |= iesave;
    }
    //rf_busy = false;
    return OK;
}
#define XTALFREQ   26000000UL
#define XTALDIV    (((XTALFREQ) > 40000000UL) ? 2 : ((XTALFREQ) > 20000000UL) ? 1 : 0)
#define XTALFRQDIV ((XTALFREQ) >> (XTALDIV))


static inline uint16_t myabs(int16_t a)
{
    if(a > 0) return a;
    else return -a;
}

uint8_t hp0, hp1;

void calibrate_oscillators(void)
{
    //return;
    static uint16_t __xdata frcmaxerr;
    static uint16_t __xdata lpmaxerr;
    static uint16_t __xdata frcperiod;
    static uint16_t __xdata lpperiod;
    static uint8_t __xdata frccalcnt;
    static uint8_t __xdata lpcalcnt;
    static uint8_t __xdata clkconsave;
    frcperiod = 0;
    lpperiod = 0;
    frccalcnt = 0;
    lpcalcnt = 0;
    clkconsave = 0xff;
    EA = 0;
    ax5043_rclk_enable(XTALDIV);

	static uint8_t __xdata lposccfg;
	uint8_t frcosccfg;
   // LED = 1;
    frcosccfg = 0x70 | ((CLKSRC_RSYSCLK) & 0x07);
    FRCOSCREF0 = (10000000UL << 8) / ((XTALFRQDIV) >> 6);
    FRCOSCREF1 = ((10000000UL << 8) / ((XTALFRQDIV) >> 6)) >> 8;
    lposccfg = 0x00 | ((CLKSRC_RSYSCLK) & 0x07);
    LPOSCREF0 = (XTALFRQDIV) / 320UL;
    LPOSCREF1 = ((XTALFRQDIV) / 320UL) >> 8;
	FRCOSCKFILT0 = 0x0;
	FRCOSCKFILT1 = 0x40;
	LPOSCKFILT1 = 0x05;
    LPOSCKFILT0 = 0x0;

    FRCOSCCONFIG = frcosccfg;
	LPOSCCONFIG = lposccfg;

    frcmaxerr = FRCOSCREF1;
    frcmaxerr <<= 8;
    frcmaxerr |= FRCOSCREF0;
    frcmaxerr /= 500;

    lpmaxerr = LPOSCREF1;
    lpmaxerr <<= 8;
    lpmaxerr |= LPOSCREF0;
    lpmaxerr /= 500;

    OSCCALIB |= 0x03;
    //OSCCALIB |= 0x02;


    for (;;) {
          uint8_t osccal = OSCCALIB;
        if (osccal & 0x40) {

            frcperiod = FRCOSCPER1;
            frcperiod <<= 8;
            frcperiod |= FRCOSCPER0;

            uint16_t tmp = FRCOSCREF1;
            tmp <<= 8;
            tmp |= FRCOSCREF0;

            if (myabs(frcperiod - tmp) > frcmaxerr)
                frccalcnt = 0;
            else if (frccalcnt != 0xff)
                ++frccalcnt;
        }
        if (osccal & 0x80) {
            lpperiod = LPOSCPER1;
            lpperiod <<= 8;
            lpperiod |= LPOSCPER0;

            uint16_t tmp = LPOSCREF1;
            tmp <<= 8;
            tmp |= LPOSCREF0;

            if (myabs(lpperiod - tmp) > lpmaxerr)
                lpcalcnt = 0;
            else if (lpcalcnt != 0xff)
                ++lpcalcnt;
        }
        if (frccalcnt > 4 && lpcalcnt > 4)
            break;
        wtimer_runcallbacks();
    }
    OSCCALIB &= (uint8_t)~0x03;
    if (clkconsave != 0xff) {
        // undo
        CLKCON = 0x08 | CLKSRC_FRCOSC;
        while ((CLKCON & 0x3F) != (0x08 | CLKSRC_FRCOSC));
        CLKCON = clkconsave;
    }
    ax5043_rclk_disable();
    EA = 1;
    FRCOSCCONFIG = 0;
    hp0 = FRCOSCFREQ0;
    hp1 = FRCOSCFREQ1;
}



