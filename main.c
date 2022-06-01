#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>
// do miernika
#define wyborDDR DDRC
#define wybor PORTC
volatile uint16_t srednia_0 = 0;
volatile uint16_t srednia_1 = 0;
volatile uint16_t pomiar = 0;
volatile uint16_t wynik_1[]={0,0,0};
volatile uint16_t wynik_0[]={0,0,0};
volatile uint8_t kanal=0;
volatile uint16_t min =0;
volatile uint16_t max =0;
volatile uint8_t tryb = 1;
volatile uint8_t pomoc = 0b00001100;


// do przycisku
#define przycisk 7
volatile uint8_t klawisz = 0;
//do wyœwietlacza
#define LCD_DDR DDRB
#define LCD PORTB
#define RS 4
#define E 5


void AVG(uint16_t wartosc, uint16_t srednia){
	static uint8_t licznik = 0;
	if(licznik++==16){
		kanal = ~kanal;
		licznik ==0;
		srednia = (pomiar>>4);
		pomiar = 0;
		ADMUX &= ~(1>>MUX0);
		if(srednia>max)max=srednia;
		if(srednia<min)min=srednia;
	}
	pomiar += wartosc;
}

ISR(ADC_vect){
	switch(kanal){
		case 0:
		AVG(ADC, srednia_0);
		case 1:
		AVG(ADC, srednia_1);
	}
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

///////////
ISR(TIMER1_COMPA_vect){
	ADCSRA |= (1<<ADSC);
}
///////////

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
	ADMUX = (1<<REFS1)|(1<<REFS0); //Vref=2,55V i 1 port
	ADCSRA = (1<<ADEN)|(1<<ADIE)|(1<<ADATE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); //w³¹czenie ADC i preskaler 128
}

void init_Button(){
	PORTB |= (1<<przycisk); //odpalenie rezystora polaryzuj¹cego
	TCCR0 = (1<<CS02) | (1<<CS00); //prescaler 1024
	TIMSK = (1<<TOIE0)|(1<<OCIE1A); //w³¹czenie przerwania OVF T0 NIE WIEMY CZY POTRZEBNY JEST TIMER 1
	/////////////
	TCCR1B=(1<<WGM12)|(1<<CS12);
	OCR1A = 62500;
	/////////////
}
void clear_LCD(){
	LCD &= ~(1<<RS);
	LCD_zapis(0x01);
	_delay_ms(2);
	LCD |= (1<<RS);
	
}

int main(void)
{
	init_LCD();
	init_Button();
	
	LCD_napis("Witaj");
	sei();
	while (1)
	{
		if(klawisz == 2){
			switch(tryb){
				case 0:
				tryb += 1;
				case 1:
				clear_LCD();
				//Wyœwietlenie kana³u 0
				LCD_zapis((srednia_0%100/10)+'0');
				LCD_zapis((srednia_0%10)+'0');
				//Wywietlenie kana³u 1
				tryb += 1;
				LCD_napis("Tryb_1");
				break;
				case 2:
				clear_LCD();
				//Wyœwietlenie kana³u 0
				//Pasek
				tryb += 1;
				LCD_napis("Tryb_2");
				break;
				case 3:
				clear_LCD();
				//Wyœwietlenie kana³u 1
				//Pasek
				tryb = 1;
				LCD_napis("Tryb_3");
				break;
			}
			klawisz = 3;
}
	}
}