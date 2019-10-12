// this code scans ADC1 for an analog signal upon request, using 8Mhz processor clock
// POMIESZANE USTAWIENIA SILNIKOW

#include <avr/io.h>
#include <stdint.h>       // needed for uint8_t
#include <util/delay.h>
#include <avr/interrupt.h>

#define F_CPU	16000000

#define DCMOTORPWM_PRESCALER (1 << CS11)
#define DCMOTORPWM_ICR1 1000
#define l_val	0
#define l_val1	250

volatile int ADCval1, ADCval2, ADCval3, ADCval4, ADCval5, ADCval6;

void PWM_setup()
{
	DDRB |= (1 << PB1);
	// PB1 is now an output
	ICR1 = DCMOTORPWM_ICR1; //fixed top value
	TCCR1A |= (1<<COM1A1); //non-inverting mode for OC1A
	TCCR1A |= (1<<WGM11); //fast pwm mode
	TCCR1B |= (1<<WGM13) | (1<<WGM12);
	TCCR1B |= DCMOTORPWM_PRESCALER; //set prescaler
	//stop motor
	//OCR1A = 0;
}

int ADCsingleREAD(uint8_t adctouse)
{
	int ADCval;
	
	ADMUX = adctouse;         // use #1 ADC
	ADMUX |= (1 << REFS0);    // use AVcc as the reference
	ADMUX &= ~(1 << ADLAR);   // clear for 10 bit resolution
	
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);    // 128 prescale for 8Mhz
	ADCSRA |= (1 << ADEN);    // Enable the ADC
	// Thanks T, this line waits for the ADC to finish
	ADCval = ADCL;
	ADCval = (ADCH << 8) + ADCval;    // ADCH is read so ADC can be updated again
	ADCSRA |= (1 << ADSC);    // Start the ADC conversion
	while(ADCSRA & (1 << ADSC));
	
	return ADCval;
}

//////////////////////////////////////////////////////////////////////////
void s_ADC(void)
{
	ADMUX &= ~(1 << ADLAR);   // clear for 10 bit resolution
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);    // 128 prescale for 8Mhz
	ADCSRA |= (1 << ADEN);    // Enable the ADC
	
}

int ch_ADC(uint8_t adctouse)
{
	int ADCvalue;
	ADMUX = adctouse;
	ADMUX |= (1 << REFS0);    // use AVcc as the reference
	
	ADCSRA |= (1 << ADSC);    // Start the ADC conversion
	while(ADCSRA & (1 << ADSC));
	
	ADCvalue = ADCL;
	ADCvalue = (ADCH << 8) + ADCvalue;    // ADCH is read so ADC can be updated again
	
	//ADCvalue = ADCW;
	return ADCvalue;
}

//////////////////////MOTOR_SETUP////////////////////////////////////////////////////
//Now consider the following cases for motor M1:
//M1-A = 1 and M1-B = 0 ? M1 moves clockwise (say). Then
//M1-A = 0 and M1-B = 1 ? M1 moves counter-clockwise.
//M1-A = 0 and M1-B = 0 ? M1 stops.
//M1-A = 1 and M1-B = 1 ? M1 stops.
//Similar cases can arise for motor M2:
//M2-A = 1 and M2-B = 0 ? M2 moves clockwise (say). Then
//M2-A = 0 and M2-B = 1 ? M2 moves counter-clockwise.
//M2-A = 0 and M2-B = 0 ? M2 stops.
//M2-A = 1 and M2-B = 1 ? M2 stops.

void m1_forw()
{
	PORTD |= ((1 << PD0)|(0 << PD1));
}


void m1_back()
{
	PORTD |= ((0 << PD0)|(1 << PD1));
}

void m1_stop()
{
	PORTD |= ((0 << PD0)|(0 << PD1));
}

void m2_forw()
{
	PORTD |= ((0 << PD2)|(1 << PD3));
}

void m2_back()
{
	PORTD |= ((1 << PD2)|(0 << PD3));
}

void m2_stop()
{
	PORTD |= ((0 << PD2)|(0 << PD3));
}

void m_reset()
{
	PORTD = 0b00000000;
}


//////////////(////////////////////////////////////////////////////////////
void line_foll(void)
{
	if ((ADCval1 <= l_val)&&(ADCval2 <= l_val)&&(ADCval3 > l_val)&&(ADCval4 <= l_val)&&(ADCval5 <= l_val))
	{
		m_reset();
		m1_forw();
		m2_forw();
	}
	//////////////////////////////////////////////////////////////////////////
	else if ((ADCval1 <= l_val)&&(ADCval2 <= l_val)&&(ADCval3 > l_val)&&(ADCval4 > l_val)&&(ADCval5 <= l_val))
	{
		m_reset();
		m1_forw();
	}
	else if ((ADCval1 <= l_val)&&(ADCval2 > l_val)&&(ADCval3 > l_val)&&(ADCval4 <= l_val)&&(ADCval5 <= l_val))
	{
		m_reset();
		m2_forw();
	}
	//////////////////////////////////////////////////////////////////////////
	else if ((ADCval1 <= l_val)&&(ADCval2 > l_val)&&(ADCval3 <= l_val)&&(ADCval4 <= l_val)&&(ADCval5 <= l_val))
	{
		m_reset();
		m2_forw();
	}
	else if ((ADCval1 <= l_val)&&(ADCval2 <= l_val)&&(ADCval3 <= l_val)&&(ADCval4 > l_val)&&(ADCval5 <= l_val))
	{
		m_reset();
		m1_forw();
	}
	//////////////////////////////////////////////////////////////////////////
	else if ((ADCval1 <= l_val)&&(ADCval2 <= l_val)&&(ADCval3 <= l_val)&&(ADCval4 <= l_val)&&(ADCval5 > l_val))
	{
		m_reset();
		m2_forw();
	}
	else if ((ADCval1 > l_val)&&(ADCval2 <= l_val)&&(ADCval3 <= l_val)&&(ADCval4 <= l_val)&&(ADCval5 <= l_val))
	{
		m_reset();
		m1_forw();
	}
	//////////////////////////////////////////////////////////////////////////
	else if ((ADCval1 > l_val)&&(ADCval2 > l_val)&&(ADCval3 > l_val)&&(ADCval4 <= l_val)&&(ADCval5 <= l_val))
	{
		m_reset();
		m2_forw();
	}
	else if ((ADCval1 <= l_val)&&(ADCval2 <= l_val)&&(ADCval3 > l_val)&&(ADCval4 > l_val)&&(ADCval5 > l_val))
	{
		m_reset();
		m1_forw();
	}
	//////////////////////////////////////////////////////////////////////////
	else if ((ADCval1 > l_val)&&(ADCval2 > l_val)&&(ADCval3 <= l_val)&&(ADCval4 <= l_val)&&(ADCval5 <= l_val))
	{
		m_reset();
		m2_forw();
	}
	else if ((ADCval1 <= l_val)&&(ADCval2 <= l_val)&&(ADCval3 <= l_val)&&(ADCval4 > l_val)&&(ADCval5 > l_val))
	{
		m_reset();
		m1_forw();
	}
	//////////////////////////////////////////////////////////////////////////
	else if ((ADCval1 <= l_val)&&(ADCval2 <= l_val)&&(ADCval3 <= l_val)&&(ADCval4 <= l_val)&&(ADCval5 <= l_val))
	{
		m_reset();
		m1_stop();
		m2_stop();
	}
}
//////////////////////////////////////////////////////////////////////////
void line_foll_1(void)
{
	if ((ADCval1 > l_val)&&(ADCval2 <= l_val)&&(ADCval3 <= l_val)&&(ADCval4 <= l_val)&&(ADCval5 <= l_val))
	{
		m_reset();
		m2_forw();
	}
	if ((ADCval1 <= l_val)&&(ADCval2 <= l_val)&&(ADCval3 <= l_val)&&(ADCval4 <= l_val)&&(ADCval5 > l_val))
	{
		m_reset();
		m1_forw();
	}
	if ((ADCval1 <= l_val)&&(ADCval2 > l_val)&&(ADCval3 <= l_val)&&(ADCval4 <= l_val)&&(ADCval5 <= l_val))
	{
		m_reset();
		m2_forw();
	}
	if ((ADCval1 <= l_val)&&(ADCval2 <= l_val)&&(ADCval3 <= l_val)&&(ADCval4 > l_val)&&(ADCval5 <= l_val))
	{
		m_reset();
		m1_forw();
	}
	if ((ADCval1 <= l_val)&&(ADCval2 <= l_val)&&(ADCval3 > l_val)&&(ADCval4 <= l_val)&&(ADCval5 <= l_val))
	{
		m_reset();
		m1_forw();
		m2_forw();
	}
	//////////////////////////////////////////////////////////////////////////
	if ((ADCval1 <= l_val)&&(ADCval2 <= l_val)&&(ADCval3 <= l_val)&&(ADCval4 > l_val)&&(ADCval5 > l_val))
	{
		m_reset();
		m1_forw();
	}
	if ((ADCval1 <= l_val)&&(ADCval2 <= l_val)&&(ADCval3 > l_val)&&(ADCval4 > l_val)&&(ADCval5 > l_val))
	{
		m_reset();
		m1_forw();
	}
	if ((ADCval1 <= l_val)&&(ADCval2 <= l_val)&&(ADCval3 > l_val)&&(ADCval4 > l_val)&&(ADCval5 <= l_val))
	{
		m_reset();
		m1_forw();
	}
	
	if ((ADCval1 > l_val)&&(ADCval2 > l_val)&&(ADCval3 <= l_val)&&(ADCval4 <= l_val)&&(ADCval5 <= l_val))
	{
		m_reset();
		m2_forw();
	}
	if ((ADCval1 > l_val)&&(ADCval2 > l_val)&&(ADCval3 > l_val)&&(ADCval4 <= l_val)&&(ADCval5 <= l_val))
	{
		m_reset();
		m2_forw();
	}
	if ((ADCval1 <= l_val)&&(ADCval2 > l_val)&&(ADCval3 > l_val)&&(ADCval4 <= l_val)&&(ADCval5 <= l_val))
	{
		m_reset();
		m2_forw();
	}
	//////////////////////////////////////////////////////////////////////////
	else if ((ADCval1 <= l_val)&&(ADCval2 <= l_val)&&(ADCval3 <= l_val)&&(ADCval4 <= l_val)&&(ADCval5 <= l_val))
	{
		m_reset();
		m1_stop();
		m2_stop();
	}
}
//////////////////////////////////////////////////////////////////////////
void check_sensors()
{
	if ((ADCval1 > l_val)&&(ADCval2 <= l_val)&&(ADCval3 <= l_val)&&(ADCval4 <= l_val)&&(ADCval5 <= l_val))
	{
		m_reset();
		m1_forw();
		
	}else if((ADCval1 <= l_val)&&(ADCval2 > l_val)&&(ADCval3 <= l_val)&&(ADCval4 <= l_val)&&(ADCval5 <= l_val))
	{
		m_reset();
		m2_forw();
	}else if ((ADCval1 <= l_val)&&(ADCval2 <= l_val)&&(ADCval3 > l_val)&&(ADCval4 <= l_val)&&(ADCval5 <= l_val))
	{
		m_reset();
		m1_back();
	}else if ((ADCval1 <= l_val)&&(ADCval2 <= l_val)&&(ADCval3 <= l_val)&&(ADCval4 > l_val)&&(ADCval5 <= l_val))
	{
		m_reset();
		m2_back();
	}else if ((ADCval1 <= l_val)&&(ADCval2 <= l_val)&&(ADCval3 <= l_val)&&(ADCval4 <= l_val)&&(ADCval5 > l_val))
	{
		m_reset();
		m1_forw();
		m2_forw();
	}else if ((ADCval1 <= l_val)&&(ADCval2 <= l_val)&&(ADCval3 <= l_val)&&(ADCval4 <= l_val)&&(ADCval5 <= l_val))
	{
		m_reset();
		m1_stop();
		m2_stop();
	}
}

//////////////////////////////////////////////////////////////////////////

int main(void)
{
	sei();
	DDRD = 0b00001111;
	PWM_setup();
	s_ADC();
	
	while (1)
	{
		
		ADCval1 = bit_is_set(PIND,4);
		ADCval2 = bit_is_set(PIND,5);
		ADCval3 = bit_is_set(PIND,6);
		ADCval4 = bit_is_set(PIND,7);
		ADCval5 = bit_is_set(PINB,0);
		
		ADCval6=ch_ADC(5);
		
		OCR1A=ADCval6;
		
		//check_sensors();
		//line_foll();
		line_foll_1();
		
		
	}
}