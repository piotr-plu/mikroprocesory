

#include <avr/io.h>
#include <avr/interrupt.h>
#define przycisk 0
#define przycisk1 1
volatile uint8_t klawisz = 0;
volatile uint8_t klawisz1 = 0; //do zmiennych globanych dodajemy tego bandytę
ISR(TIMER0_OVF_vect){
	if (!(PINB & (1<<przycisk))){ // jeśli przycisk będzie naciśnięty to będzie zero więc gitówa
		switch(klawisz){
			case 0:
			klawisz = 1;
			break;
			case 1:
			klawisz = 2;
			break;
		}
	}
		else{
			switch(klawisz){
				case 3:
				klawisz = 4;
				break;
				case 4:
				klawisz = 0;
				break;
			}
		}
	if (!(PINB & (1<<przycisk1))){ // jeśli przycisk będzie naciśnięty to będzie zero więc gitówa
		switch(klawisz1){
			case 0:
			klawisz1 = 1;
			break;
			case 1:
			klawisz1 = 2;
			break;
		}
	}
	else{
		switch(klawisz1){
			case 3:
			klawisz1 = 4;
			break;
			case 4:
			klawisz1 = 0;
			break;
		}
	}
	 
	
}
int main(void)
{
	DDRA = 0xff;
	PORTA = 0xff;
	PORTB |= (1<<przycisk)|(1<<przycisk1); //odpalenie rezystora polaryzującego
	TCCR0 = (1<<CS02) | (1<<CS00); //prescaler 1024
	TIMSK |= (1<<TOIE0); //włączenie przerwania OVF T0
	sei();
    while (1) 
    {
			if(klawisz == 2){
			PORTA--;
			klawisz = 3;
			}
			
			if(klawisz1 == 2){
				PORTA++;
				klawisz1 = 3;
			}
			
    }
	//pinb -> rejestr portu 
}

