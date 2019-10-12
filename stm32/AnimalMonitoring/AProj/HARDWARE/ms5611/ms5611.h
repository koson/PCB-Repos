#ifndef __MS5611_H
#define __MS5611_H
void MS5611_Init(void);
void MS5611_PROM_READ(void);
void MS5611_Temperature_Get(void);
void MS5611_Pressure_Get(void);
signed int MS5611_Compensation_Calcu(void);
float getEstimatedAltitude(signed int baroPressure);

#endif
