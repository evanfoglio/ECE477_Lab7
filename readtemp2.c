#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdio.h>
#include <math.h>
#include <avr/sleep.h>
#define F_CPU 8000000UL
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>

int serial_putchar(char, FILE *);
int serial_getchar(FILE *);
static FILE serial_stream = FDEV_SETUP_STREAM (serial_putchar, serial_getchar, _FDEV_SETUP_RW);

void init_serial(void);
void init_adc(void);
unsigned int read_adc(void);

void update_clock_speed(void);


int main()
{ 
	int i;
	float values[100];
	int value_len = 0;
	int current_value = 0;
	char buffer[100]="notStart";
	float estimate;
	char num_buf[8];
	float sum;
	unsigned char startbuf[6];
	update_clock_speed();  //adjust OSCCAL
        init_serial();
        init_adc();
	_delay_ms(1000); //let serial work itself out

	while(1) //raspberry pi controls reset line
	{
		estimate = read_adc();
		dtostrf(estimate, 5, 0, num_buf);
		printf("%s\n", num_buf);
	}
}


//read the first two bytes of eeprom, if they have been programmed
////use the first byte as an offset for adjusting OSCCAL and the second as
////the direction to adjust 0=increase, 1=decrease.
////Any offset is allowed and users are cautioned that it is possible to
//// adjust the oscillator beyond safe operating bounds.

void update_clock_speed(void)
{
	char temp;
	temp=eeprom_read_byte((void *)1); //read oscillator offset sign 


	if(temp==0||temp==1)      //if sign is invalid, don't change oscillator
	{
		if(temp==0)
		{
			temp=eeprom_read_byte((void *)0);
			if(temp != 0xff) OSCCAL+=temp;
		}
		else
		{
			temp=eeprom_read_byte((void *)0);
			if(temp!=0xff) OSCCAL -=temp;
		}
	}
}




/* Initializes AVR USART for 9600 baud (assuming 8MHz clock) */
/* 8MHz/(16*(51+1)) = 9615 about 0.2% error                  */
void init_serial(void)
{
	UBRR0H=0;
	UBRR0L= 51;
	UCSR0A=0;
	UCSR0C= (1<<USBS0)|(3<<UCSZ00) ;  // 8 BIT NO PARITY 2 STOP
	UCSR0B=(1<<RXEN0)|(1<<TXEN0)  ; //ENABLE TX AND RX ALSO 8 BIT
	stdin=&serial_stream;
	stdout=&serial_stream;
}   

//simplest possible putchar, waits until UDR is empty and puts character
int serial_putchar(char val, FILE * fp)
{
	while((UCSR0A&(1<<UDRE0)) == 0); //wait until empty 
	UDR0 = val;
	return 0;
}
int serial_getchar(FILE * fp)
{
	while(!(UCSR0A&(1<<RXC0)));  //WAIT FOR CHAR
	return UDR0;
}
void init_adc(void)
{

	ADMUX = (1<<REFS1) | (1<<REFS0) | (0<<ADLAR) | (1<<MUX3) | (0<<MUX2) | (0<<MUX1) | (0<<MUX0);
	ADCSRA = (1<<ADPS2) | (1<<ADPS1) | (1<<ADEN);
//	ADMUX = (3<<REFS0) | 8; //Refference voltage is 1.1V
//	ADCSRA = (1<<ADEN) | (6<<ADPS0); // enable ADC, prescaler=64
	ADCSRB = 0;
	DIDR0 = 0;
}

//Read ADC pin and returning the value
unsigned int read_adc(void)
{
	ADCSRA |= (1<<ADSC);
        while(ADCSRA & (1<<ADSC) !=0); //wait for coversion
        return ADC;
}

