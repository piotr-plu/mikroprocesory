/*
 * GccApplication1.c
 *
 * Created: 2022-05-17 12:03:14
 * Author : oem1
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#define BAUD 4800UL //prędkość transmisji
#define my_ubrr F_CPU/16/BAUD-1 //wzrór wykład
volatile uint8_t napis[] = "Budownictwo\n"; //volatile bo dobieramy się przez przerwania

ISR(TIMER1_COMPA_vect){
	static uint8_t i=0;
	if(napis[i]); else i = 0;
	UDR = napis[i++];
}
void USART_init(uint16_t ubrr){
	UCSRB = (1<<TXEN); //włączenie nadajnika
	UCSRC = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0); //8 bity danych ten pierwzzy nawias daje nam dostęp
	UBRRH = (uint8_t)(ubrr>>8);
	UBRRL = (uint8_t)ubrr;
}


int main(void)
{
    USART_init(my_ubrr);
	TCCR1B = (1<<WGM12)|(1<<CS12);
	OCR1A = 3125;
	TIMSK = (1<<OCIE1A);
	sei();
    while (1) 
    {
    }
}

