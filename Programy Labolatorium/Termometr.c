/*
 * GccApplication1.c
 *
 * Created: 2022-05-10 12:04:25
 * Author : oem1
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#define segmentyDDR DDRB
#define segmenty PORTB
#define wyborDDR DDRC
#define wybor PORTC
uint8_t seg7[]={0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xd8, 0x80, 0x90, 0x39};
volatile uint16_t pomiar;
volatile uint8_t pokaz=0;
volatile uint8_t wynik[]={8,8,8,8};
ISR(TIMER0_OVF_vect){
	static uint8_t i=0;
	wybor |= (1<<i);
	if(i++==3) i=0;
	if (i==1) segmenty = seg7[wynik[i]] &0x7f; else segmenty = seg7[wynik[i]];
	if (i==3) segmenty = ~seg7[10];
	//segmenty = seg7[wynik[i]];
	wybor &= ~(1<<i);
}
ISR(TIMER1_COMPA_vect){
	ADCSRA |= (1<<ADSC);
}
ISR(ADC_vect){
	pomiar=ADC;
	pokaz=1;
}
int main(void)
{
    /* Replace with your application code */
	segmentyDDR =0xff;
	wyborDDR |= 0x0f;
	TCCR0 = (1<<CS02);
	TCCR1B=(1<<WGM12)|(1<<CS12);
	OCR1A = 62500;
	TIMSK = (1<<TOIE0)|(1<<OCIE1A);
	sei();
	ADMUX = (1<<REFS1)|(1<<REFS0); //Vref=2,56V
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADIE); //włączenie ADC i preskaler 16
	//ADCSRA |= (1<<ADSC);
    while (1) 
    {
		if(pokaz==1){
			pomiar=(pomiar*2.5);
			
			wynik[0] = pomiar%1000/100;
			wynik[1] = pomiar%1000%100/10;
			wynik[2] = pomiar%1000%100%10;
			//wynik[3] = 0x39;
			pokaz =0;}
		}
}

