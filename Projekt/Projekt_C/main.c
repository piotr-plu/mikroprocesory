

#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>
// do miernika
#define wyborDDR DDRC
#define wybor PORTC
volatile uint16_t srednia = 0;
volatile uint16_t pomiar = 0;
volatile uint8_t pokaz=0;

// do przycisku
#define przycisk 7
volatile uint8_t klawisz = 0;
//do wyœwietlacza
#define LCD_DDR DDRB
#define LCD PORTB
#define RS 4
#define E 5

void AVG(uint16_t wartosc){
	static uint8_t licznik = 0;
	if(licznik++==16){
		licznik ==0;
		srednia = (pomiar>>4); 
		pomiar = 0;
	}
	pomiar += wartosc;
}

ISR(ADC_vect){
	AVG(ADC);
}



// timer do przycisku
ISR(TIMER0_OVF_vect){
	if (!(PINB & (1<<przycisk))){ // jeœli przycisk bêdzie naciœniêty to bêdzie zero wiêc gitówa
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
}

void LCD_zapis(uint8_t dana){
	LCD &= 0xf0;
	LCD |= (dana>>4) & 0x0f;
	LCD |= (1<<E);
	_delay_us(2);
	LCD &= ~(1<<E);
	_delay_us(45);
	
	LCD &= 0xF0;
	LCD |= dana & 0x0f;
	LCD |= (1<<E);
	_delay_us(2);
	LCD &= ~(1<<E);
	_delay_us(45);
	
}
void LCD_napis(char *tab){
	uint8_t i = 0;
	LCD |= (1<<RS);
	while (tab[i]){
		LCD_zapis(tab[i++]);
	}
}

void init_LCD(){
	LCD_DDR |= 0x3f;
	LCD &= ~(1<<RS);
	LCD_zapis(0x33);
	LCD_zapis(0x32);
	LCD_zapis(0x28);
	LCD_zapis(0x06);
	LCD_zapis(0x0f);
	LCD_zapis(0x01);
	LCD_zapis(0x0e); // wylaczenie migotania kursora
	_delay_ms(2);
}

void init_Volt(){
	wyborDDR |= 0x0f;
	ADMUX = (1<<REFS1)|(1<<REFS0)|(1<<MUX4); //Vref=2,55V
	ADCSRA = (1<<ADEN)|(1<<ADIE)|(1<<ADATE); //w³¹czenie ADC i preskaler 128
}

void init_Button(){
	PORTB |= (1<<przycisk); //odpalenie rezystora polaryzuj¹cego
	TCCR0 = (1<<CS02) | (1<<CS00); //prescaler 1024
	TIMSK |= (1<<TOIE0); //w³¹czenie przerwania OVF T0
}

int main(void)
{
	init_LCD();
	init_Button();
	sei();
	
	//LCD_napis("Bagno");
	while (1)
	{
		if(klawisz == 2){
			
			LCD_napis((srednia%1000/100)+'0');
			//LCD_napis(srednia%1000/100);
			//LCD_napis(srednia%1000%100/10);
			//LCD_napis(srednia%1000%100%10);
			klawisz = 3;
		}
		
	}
}

