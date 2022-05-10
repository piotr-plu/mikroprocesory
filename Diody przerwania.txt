/*
 * Atmega32_Lab1.c
 *
 * Created: 2022-04-12 12:06:41
 * Author : oem1
 */ 

#include <avr/io.h>
//#define F_CPU 16000000UL
//#include <util/delay.h>
#include <avr/interrupt.h>
#define dioda 5
#define dioda2 2
uint8_t tab7[] = {0xc0, 0xf9, 0xa4,0xb0,0x99, 0x92,0x82,0xf8,0x80,0x90};
uint8_t i = 0;
ISR(TIMER1_OVF_vect){
	if(i++ ==9 ) i=0;
	PORTA = tab7[i];
}


//ISR(TIMER0_OVF_vect){ //tutaj się robi przerwanie od przepeełnienia wektora
	//PORTA ^= (1<<dioda);
//}
//WAŻNE:
//Jak usuwamy przerwanie to musimy też usunąć wywołanie tych przerwań 


int main(void)
{
	//TCCR0 = (1<<CS02)|(1<<CS00); //prescaler ma 1024
	TCCR1B = (1<<CS12); //preskaler 256 dla licznika1
	TIMSK = (1<<TOIE1); //włączenie przerwań licznika 0 i 1 od przepełnienia
	DDRA = 0xff; //założenie że port jest wyjściowy
	sei(); //włączenie kontroli przewań
	// PORTA = 0b01010101; //co druga dioda świeci 1 -> nie świeci
    while (1) 
    {
	//if(i++ ==9) i=0;
	//_delay_ms(500);
	//PORTA = tab7[i];
	
    }
}

