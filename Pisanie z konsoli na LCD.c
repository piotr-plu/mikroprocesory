///*
 //* GccApplication1.c
 //*
 //* Created: 2022-05-17 12:03:14
 //* Author : oem1
 //*/ 
//
//#include <avr/io.h>
//#include <avr/interrupt.h>
//#define F_CPU 16000000UL
//#define BAUD 4800UL //prędkość transmisji
//#define my_ubrr F_CPU/16/BAUD-1 //wzrór wykład
//
//
//ISR(USART_RXC_vect){
	//PORTA =~UDR;
//}
//
//void USART_init(uint16_t ubrr){
	//UCSRB = (1<<RXEN)|(1<<RXCIE); //włączenie odbiornika i przerwań
	//UCSRC = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0); //8 bity danych ten pierwzzy nawias daje nam dostęp
	//UBRRH = (uint8_t)(ubrr>>8);
	//UBRRL = (uint8_t)ubrr;
//}
//
//
//int main(void)
//{
    //USART_init(my_ubrr);
	//DDRA = 0xff;
	//PORTA = 0xff;
	//sei();
    //while (1) 
    //{
    //}
//}
//


#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#define BAUD 4800UL //prędkość transmisji
#define my_ubrr F_CPU/16/BAUD-1 //wzrór wykład
#include <util/delay.h>
#define magistralaDDR DDRA
#define magistrala PORTA
#define sterowanieDDR DDRB
#define sterowanie PORTB
#define RS 0
#define E 1
volatile uint8_t FLAGA_PRZERWANIA = 0;
volatile uint8_t USART_DANA = 0;

ISR(USART_RXC_vect){
	FLAGA_PRZERWANIA = 1;
	USART_DANA = UDR;
}

void zmiana_lini(uint8_t wybor){
	switch(wybor)
	{
		case 0:
		sterowanie &= ~(1<<RS); //instrukcje
		LCD_zapis(0x80);
		sterowanie |= (1<<RS); //dane
		break;
		
		case 1:
		sterowanie &= ~(1<<RS); //instrukcje
		LCD_zapis(0xc0);
		sterowanie |= (1<<RS); //dane
		break;	
	}	
}

void USART_init(uint16_t ubrr){
UCSRB = (1<<RXEN)|(1<<RXCIE); //włączenie odbiornika i przerwań
UCSRC = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0); //8 bity danych ten pierwzzy nawias daje nam dostęp
UBRRH = (uint8_t)(ubrr>>8);
UBRRL = (uint8_t)ubrr;
}

void LCD_zapis(uint8_t dana){
	magistrala = dana;
	sterowanie |=(1<<E); // na zboczu opadającym zapisujemy
	_delay_us(2);
	sterowanie &= ~(1<<E);//zbocze opadające przez ok 2us xD
	_delay_us(45);
}
int main(void)
{
	magistralaDDR = 0xff; //10 połączeń jako wyjścia
	sterowanieDDR |= (1<<RS)|(1<<E);
	sterowanie &= ~(1<<RS); //instrukcje
	LCD_zapis(0x38); //magistrala 8 bit
	LCD_zapis(0x06); //inktementacja adresu
	LCD_zapis(0x0f); //włączenie wyświetlacza oraz kursora
	LCD_zapis(0x01); //czyszczenie ektranu
	_delay_ms(3);
	
	USART_init(my_ubrr);
	magistrala = 0xff;
	sei();
	
	uint8_t licznik = 0;
	while (1)
	{
		if(FLAGA_PRZERWANIA == 1){
			licznik++;
			sterowanie |= (1<<RS); //dane
			
			LCD_zapis(USART_DANA);
			FLAGA_PRZERWANIA = 0;
			
			if(licznik == 16){
				zmiana_lini(1);
				LCD_zapis('>');
			}
				
			if(licznik == 32){
				licznik = 0;
				zmiana_lini(0);
				LCD_zapis('>');
			}
		}
	}
}




