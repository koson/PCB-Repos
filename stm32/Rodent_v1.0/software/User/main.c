/**
 *	Keil project for GPIO pins
 *
 *	Before you start, select your target, on the right of the "Load" button
 *
 *	@author		Tilen Majerle
 *	@email		tilen@majerle.eu
 *	@website	http://stm32f4-discovery.com
 *	@ide		Keil uVision 5
 *	@conf		PLL parameters are set in "Options for Target" -> "C/C++" -> "Defines"
 *	@packs		STM32F4xx Keil packs version 2.2.0 or greater required
 *	@stdperiph	STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 */
 
/* Include core modules */
#include "stm32f4xx.h"
/* Include my libraries here */
#include "defines.h"
#include "tm_stm32f4_delay.h"
#include "tm_stm32f4_usart.h"
#include "tm_stm32f4_gpio.h"
#include "tm_stm32f4_pwm.h"
#include "tm_stm32f4_adc.h"
#include "tm_stm32f4_exti.h"
//#include "tm_stm32f4_disco.h"
#include "tm_stm32f4_mpu6050.h"
#include "tm_stm32f4_pcd8544.h"
#include "tm_stm32f4_ds1307.h"
#include <stdio.h>
#include <stdlib.h>
//#include <math.h>

#define MinError 3 //Minimal % of acceptable error
#define MaxError	60 //Maximal % of acceptable error
//#define MiddleCorrect 200 //Correction value to stay in middle
#define SensorStraightMax 60 //Max % value of straight analog sensor
#define SensorSideMin 30 //Min % value of side analog sensor
//#define MotorStraightSearchMax 80 //Max % value of driving straight 
#define max_power_percent 80 //Max % value of driving straight
#define medium_power_percent 40 //Max % value of driving straight
#define segment_lenght 500 //Distance made by encoders ((ER+EL)/2) to drive to next segment 

TM_PWM_TIM_t TIM4_Data; //pwm struct
TM_MPU6050_t MPU6050_Data; //mpu struct
TM_DS1307_Time_t time; //rtc struct
uint8_t last; //rtc var

float adcRLerror;
float abs_adcRLerror;
float active_power_percentR;
float active_power_percentL;

float pwmr, pwml; //PWM percent value of right and left motor
float adc0, adc1, adc2, adc3;
//float adcRLerror;
float test_value;

float ax, ay, az;
float gx, gy, gz;

float distance= 0;
int update_segment = 0;


/*--------------------MOTORS_OPERATING---------------------------------
power_percent - percent of pwm duty cycle
MOTOR_A - Right 
				 Stering Port A: PE7, PE8
				 PWM_A Port: PA5
MOTOR_B - Left 
				 Stering Port B: PE9, PE10
				 PWM_B Port: PB5*/

void MotorSystemInit()
{
	/* Set PWM to 100kHz frequency on timer TIM4 */
	/* 1 kHz = 1ms = 1000us */
	TM_PWM_InitTimer(TIM4, &TIM4_Data, 40000);
	
	/* Initialize PWM on TIM2, Channel 1 and PinsPack 2 = PB5 */
	TM_PWM_InitChannel(&TIM4_Data, TM_PWM_Channel_3, TM_PWM_PinsPack_1);
	
	//TM_PWM_InitTimer(TIM4, &TIM4_Data, 40000);
	/* Initialize PWM on TIM4, Channel 2 and PinsPack 2 = PB5 */
	TM_PWM_InitChannel(&TIM4_Data, TM_PWM_Channel_4, TM_PWM_PinsPack_1);
	
//	/* Initialize PWM on TIM4, Channel 2 and PinsPack 2 = PB3 */
//	TM_PWM_InitChannel(&TIM2_Data, TM_PWM_Channel_2, TM_PWM_PinsPack_2);
	
		/* Initialize Motors GPIO*/
	TM_GPIO_Init(GPIOE, GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10, TM_GPIO_Mode_OUT, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High);
	
}

void RightForward(float power_percent)
{
	TM_PWM_SetChannelPercent(&TIM4_Data, TM_PWM_Channel_3, power_percent);
	TM_GPIO_SetPinHigh(GPIOE, GPIO_Pin_7);
	TM_GPIO_SetPinLow(GPIOE, GPIO_Pin_8);
}
void LeftForward(float power_percent)
{
	TM_PWM_SetChannelPercent(&TIM4_Data, TM_PWM_Channel_4, power_percent);
	TM_GPIO_SetPinHigh(GPIOE, GPIO_Pin_9);
	TM_GPIO_SetPinLow(GPIOE, GPIO_Pin_10);
}

void RightBackward(float power_percent)
{
	TM_PWM_SetChannelPercent(&TIM4_Data, TM_PWM_Channel_3, power_percent);
	TM_GPIO_SetPinLow(GPIOE, GPIO_Pin_7);
	TM_GPIO_SetPinHigh(GPIOE, GPIO_Pin_8);
}
void LeftBackward(float power_percent)
{
	TM_PWM_SetChannelPercent(&TIM4_Data, TM_PWM_Channel_4, power_percent);
	TM_GPIO_SetPinLow(GPIOE, GPIO_Pin_9);
	TM_GPIO_SetPinHigh(GPIOE, GPIO_Pin_10);
}

void RightHardStop()
{
	TM_PWM_SetChannelPercent(&TIM4_Data, TM_PWM_Channel_3, 0);
	TM_GPIO_SetPinLow(GPIOE, GPIO_Pin_7);
	TM_GPIO_SetPinLow(GPIOE, GPIO_Pin_8);
}

void LeftHardStop()
{
	TM_PWM_SetChannelPercent(&TIM4_Data, TM_PWM_Channel_4, 0);
	TM_GPIO_SetPinLow(GPIOE, GPIO_Pin_9);
	TM_GPIO_SetPinLow(GPIOE, GPIO_Pin_10);
}

void RightSoftStop()
{
	TM_GPIO_SetPinLow(GPIOE, GPIO_Pin_7);
	TM_GPIO_SetPinLow(GPIOE, GPIO_Pin_8);
}

void LeftSoftStop()
{
	TM_GPIO_SetPinLow(GPIOE, GPIO_Pin_9);
	TM_GPIO_SetPinLow(GPIOE, GPIO_Pin_10);
}

void MicromouseSystemInit()
{	
	/* Initialize system */
	SystemInit();
	
	/* Initialize delay */
	TM_DELAY_Init();
	/* Initialize motors system*/
	MotorSystemInit();	
	
	/* Initialize USART6, 115200 baud, TX: PC6 RX: PC7*/
	TM_USART_Init(USART6, TM_USART_PinsPack_1, 115200);
	
/* Initialize RTC DS1307 */
		/* RTC SQW External interrupt PE0*/
	if (TM_EXTI_Attach(GPIOE, GPIO_Pin_0, TM_EXTI_Trigger_Falling) != TM_EXTI_Result_Ok) {
			TM_USART_Puts(USART6, "RTC EXIT Error\n");
	while(1);
	}

	if (TM_DS1307_Init() != TM_DS1307_Result_Ok) {
			/* Red LED on */
			TM_USART_Puts(USART6, "RTC Error\n");
			while (1);
	}
		/* Set output pin to 1 Hz */
	TM_DS1307_EnableOutputPin(TM_DS1307_OutputFrequency_1Hz);
	

	
/* Initialize mpu*/
	if (TM_MPU6050_Init(&MPU6050_Data, TM_MPU6050_Device_0, TM_MPU6050_Accelerometer_8G, TM_MPU6050_Gyroscope_2000s) != TM_MPU6050_Result_Ok) {
		/* Display error to user */
		TM_USART_Puts(USART6, "MPU6050 Error\n");
	}
	
/* Initialize LCD with 0x38 software contrast */
	PCD8544_Init(0x40);
	
/* Initialize ADC */
	TM_ADC_Init(ADC1, ADC_Channel_11); //ADC Port PC1
	TM_ADC_Init(ADC1, ADC_Channel_1);	//ADC Port PA1
	TM_ADC_Init(ADC1, ADC_Channel_2);	//ADC Port PA2
	TM_ADC_Init(ADC1, ADC_Channel_3);	//ADC Port PA3	
	
/* Initialize buttons GPIO*/
	TM_GPIO_Init(GPIOE, GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4, TM_GPIO_Mode_IN, TM_GPIO_OType_PP, TM_GPIO_PuPd_DOWN, TM_GPIO_Speed_High);
			//	/* If button pressed */
			//	GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);
			//	/* Turn on leds */
			//	GPIO_SetBits(GPIOD, GPIO_Pin_13 | GPIO_Pin_14);
			//	/* Turn off leds */
			//	GPIO_ResetBits(GPIOD, GPIO_Pin_13 | GPIO_Pin_14);
}

void menu(){
		/* Go to x=14, y=3 position */
	PCD8544_GotoXY(14, 3);
	
	/* Print data with Pixel Set mode and Fontsize of 5x7px */
	PCD8544_Puts("RODENT", PCD8544_Pixel_Set, PCD8544_FontSize_5x7);
	/* Go to new location and write new string */
	PCD8544_GotoXY(15, 13);
	PCD8544_Puts("MICROMOUSE", PCD8544_Pixel_Set, PCD8544_FontSize_5x7);
	/* Go to new location and write new string */
	PCD8544_GotoXY(30, 26);
	PCD8544_Puts("2015", PCD8544_Pixel_Set, PCD8544_FontSize_5x7);

	/* Go to x=45, y=42 position */
	PCD8544_GotoXY(45, 42);
	
	/* Put string with Pixel set mode and Fontsize of 3x5 */
	PCD8544_Puts("aketudo", PCD8544_Pixel_Set, PCD8544_FontSize_3x5);
	
	/* Put data to LCD */
	PCD8544_Refresh();
}

int MakeTurn(){
	return 1;
}

void DriveMiddle(float power_percentR, float power_percentL)
{
	adcRLerror = adc1 - adc2; //positive gives right offset(right side too close)
	abs_adcRLerror = abs(adcRLerror);
	
	if((adc0 < SensorStraightMax) && (adc3 < SensorStraightMax)){
		active_power_percentR = max_power_percent;
		active_power_percentL = max_power_percent;
	}else if((adc0 > SensorStraightMax) && (adc3 > SensorStraightMax)){
		RightHardStop();
		LeftHardStop();
		//MakeTurn();
	}
	if((adcRLerror > 0) && (adcRLerror > MinError)){
		RightForward(active_power_percentR + abs_adcRLerror);
		LeftForward(active_power_percentL - abs_adcRLerror);
	}else if((adcRLerror < 0) && (abs_adcRLerror > MinError)){
		RightForward(active_power_percentR - abs_adcRLerror);
		LeftForward(active_power_percentL + abs_adcRLerror);
	}else if((abs_adcRLerror <= MinError)){
		RightForward(active_power_percentR);
		LeftForward(active_power_percentL);
	}
}

double seconds = 0;
char buffer[100];

int main(void) {
	MicromouseSystemInit();
	
	menu();
	
	while (1) {
/*MPU6050*/
		/* Read all data from sensor */
//	TM_MPU6050_ReadAll(&MPU6050_Data);
	TM_MPU6050_ReadAccelerometer(&MPU6050_Data);
	TM_MPU6050_ReadGyroscope(&MPU6050_Data);
	
	ax = MPU6050_Data.Accelerometer_X;
	ay = MPU6050_Data.Accelerometer_Y;
	az = MPU6050_Data.Accelerometer_Z;
	
	gx = MPU6050_Data.Gyroscope_X;
	gy = MPU6050_Data.Gyroscope_Y;
	gz = MPU6050_Data.Gyroscope_Z;
			
			
//		/* Format data */		
	sprintf(buffer, "ax: %d ay: %d az: %d || gx: %d gy: %d gz: %d", (int)ax, (int)ay, (int)az, (int)gx, (int)gy, (int)gz);
//		/* Show to usart */
	TM_USART_Puts(USART6, buffer);
	Delayms(500);
/*MPU6050*/

/*RTC*/
		//Get only seconds
		//seconds = TM_DS1307_GetSeconds();
		//Get only minutes
		//minutes = TM_DS1307_GetMinutes();
		//sprintf(buffer, "seconds: %d | minutes: %d", seconds, minutes);
		//TM_USART_Puts(USART6, buffer);
		//Delayms(500);
				
/* ADC */	
		/* Read ADC1 Channel0					Read ADC1 Channel3 */
//		adc0 = TM_ADC_Read(ADC1, ADC_Channel_11);
//		adc1 = TM_ADC_Read(ADC1, ADC_Channel_1);
//		adc2 = TM_ADC_Read(ADC1, ADC_Channel_2);
//		adc3 = TM_ADC_Read(ADC1, ADC_Channel_3);
//		
//		/*converting adc to percent*/
//		adc0 = adc0*100/4096;
//		adc1 = adc1*100/4096;
//		adc2 = adc2*100/4096;
//		adc3 = adc3*100/4096;
//	
//		sprintf(buffer, "adc0: %d | adc1: %d | adc2: %d | adc3: %d", (int)adc0, (int)adc1, (int)adc2, (int)adc3);
//		TM_USART_Puts(USART6, buffer);
//		Delayms(500);		
		
/*MOTORS_TEST*/
//		test_value = 50;		
//		LeftForward(test_value);
//		Delayms(2000);
//		LeftSoftStop();
//		//LeftFastStop();
//		RightForward(test_value);
//		Delayms(2000);
//		RightSoftStop();
//		//RightFastStop();
//		LeftBackward(test_value);
//		Delayms(2000);
//		LeftSoftStop();
//		//LeftFastStop();
//		RightBackward(test_value);
//		Delayms(2000);
//		RightSoftStop();
		//RightFastStop();
	}
}

/* TM EXTI Handler for all EXTI lines */
void TM_EXTI_Handler(uint16_t GPIO_Pin) {
/*Read RTC SQW interupt*/
//	if (GPIO_Pin == GPIO_Pin_0) {
//		/* Process data */
//		seconds++;
//		sprintf(buffer, "seconds: %d ", (int)seconds);
//		TM_USART_Puts(USART6, buffer);
//	}			
}
