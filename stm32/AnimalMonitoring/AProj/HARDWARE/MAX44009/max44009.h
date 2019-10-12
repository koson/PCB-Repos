#ifndef __MAX44009_H__
#define __MAX44009_H__
#include "sys.h"


#define getmax(a,b) ((a)>(b)?(a):(b))
#define BITMASK(a)  (1<<a)
#define BIT(a)      (1<<a)

#ifndef TRUE
#define TRUE        (1==1)
#define FALSE       (1==2)
#endif
#define LOW         (0)
#define HIGH        (1)

#define UBLB(a,b)  ( ( (a) << 8) | (b) )
#define UBLB16(a,b) ( ( (a) << 16 ) | (b) )
#define UBLB32(a,b,c,d)  ((( ((a)<<24) | ((b)<<16) ) | ((c)<<8)) | (d) )
   	   		   

#define MAX_ADDRESS                 0x94

#define INT_STATUS_REG              0x00
#define INT_ENABLE_REG              0x01
#define CONFIGURATION_REG           0x02
#define   CONFIG_CONT_MASK          (BIT(7))  // CONTINOUS MODE
#define     CONFIG_CONT_ON          (1<<7)
#define     CONFIG_CONT_OFF         (0)
#define   CONFIG_MANUAL_MASK        (BIT(6))  // MANUAL Set CDR and TIME
#define     CONFIG_MANUAL_ON        (1<<6)
#define     CONFIG_MANUAL_OFF       (0)
#define   CONFIG_CDR_MASK           (BIT(3))  // Current DIVISION RATIO (When HIGH Brightness --> Current/8
#define     CONFIG_CDR_1div1        0
#define     CONFIG_CDR_1div8        (1<<3)
#define   CONFIG_TIM_MASK           (BIT(2)|BIT(1)|BIT(0))
#define     CONFIG_TIM_800MS        (0)
#define     CONFIG_TIM_400MS        (1)
#define     CONFIG_TIM_200MS        (2)
#define     CONFIG_TIM_100MS        (3)
#define     CONFIG_TIM_50MS         (4)
#define     CONFIG_TIM_25MS         (5)
#define     CONFIG_TIM_12MS         (6)
#define     CONFIG_TIM_6MS          (7)

#define LUX_HIGH_REG                0x03
#define LUX_LOW_REG                 0x04
#define THRESHOLD_UPPER_REG         0x05
#define THRESHOLD_LOWER_REG         0x06
#define THRESHOLD_TIMER_REG         0x07

void MAX44009_WR_Reg(u8 addr,u8 val);
u8 MAX44009_RD_Reg(u8 addr);
void I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
void I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
void i2c_setRegister(uint8_t address, uint8_t registeraddress, uint8_t mask, uint8_t writevalue);
void setEnabled(uint8_t enable);
uint8_t max44009_initialize(void);
void getFluxMeasurement(uint32_t *mLux_value);

#endif
