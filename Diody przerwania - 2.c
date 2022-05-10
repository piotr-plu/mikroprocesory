/*
 * Atmega32_Lab1.c
 *
 * Created: 2022-04-12 12:06:41
 * Author : oem1
 */ 

#include <avr/io.h>
//#define F_CPU 16000000UL
//#include <util/delay.h>
#define segmentyDDR DDRA
#define segmenty PORTA
#define cyfraDDR DDRB
#define cyfra PORTB
#include <avr/interrupt.h>
//#define dioda 5
//#define dioda2 2
uint8_t tab7[] = {0xc0, 0xf9, 0xa4,0xb0,0x99, 0x92,0x82,0xf8,0x80,0x90,0xff};
uint8_t tab4[] = {0,9,8,0};
ISR(TIMER1_COMPA_vect){
	if(tab4[3]++ == 9){ 
		tab4[3] = 0;
		if(tab4[2]++ == 9){
			tab4[2] = 0;
			if(tab4[1]++ == 9){
				tab4[1] = 0;
				if(tab4[0]++ == 9){
					tab4[0] = 0;
				}
			}
		}
	}
	
}


ISR(TIMER0_OVF_vect){ //tutaj się robi przerwanie od przepeełnienia wektora
	static uint8_t i = 0;
	cyfra |=(1<<i);
	if(i++ ==3) i=0;
	segmenty = tab7[tab4[i]];
	cyfra &= ~(1<<i);
}
//WAŻNE:
//Jak usuwamy przerwanie to musimy też usunąć wywołanie tych przerwań bo się zatrzyma w jednym miejscu :)


int main(void)
{
	segmentyDDR = 0xff;
	cyfraDDR = 0xff;
	segmenty = 0xff;
	cyfra = 0xff;
	TCCR0 = (1<<CS02);//|(1<<CS00); //prescaler ma 1024
	TCCR1B = (1<<CS12)|(1<<WGM12); //preskaler 256 dla licznika1| tryb ctc licznika 1
	OCR1A = 62500; //liczba do porównania w liczniku w trybie pracy CTC
	//DDRA = 0xff; //założenie że port jest wyjściowy
	TIMSK = (1<<TOIE0)|(1<<OCIE1A); //włączenie przerwań licznika 0 i 1 od przepełnienia
	sei(); //włączenie kontroli przewań
	 //PORTA = 0b01010101; //co druga dioda świeci 1 -> nie świeci
	 
	 
    while (1) 
    {
	//if(i++ ==9) i=0;
	//_delay_ms(500);
	//PORTA = tab7[i];
	
	
		//segmenty = tab7[2];
		//cyfra &= ~(1<<0);
		//_delay_ms(5);
		//cyfra |=(1<<0);
		//
		//segmenty = tab7[1];
		//cyfra &= ~(1<<1);
		//_delay_ms(5);
		//cyfra |=(1<<1);
		//
		//segmenty = tab7[3];
		//cyfra &= ~(1<<2);
		//_delay_ms(5);
		//cyfra |=(1<<2);
		//
		//
		//segmenty = tab7[7];
		//cyfra &= ~(1<<3);
		//_delay_ms(5);
		//cyfra |=(1<<3);
    
	}
}

