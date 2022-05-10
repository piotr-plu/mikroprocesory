

#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#define magistralaDDR DDRA
#define magistrala PORTA
#define sterowanieDDR DDRB
#define sterowanie PORTB
#define RS 0
#define E 1

void LCD_zapis(uint8_t dana){
	magistrala = dana;
	sterowanie |=(1<<E); // na zboczu opadającym zapisujemy
	_delay_us(2);
	sterowanie &= ~(1<<E);//zbocze opadające przez ok 2us xD
	_delay_us(45);
}
void LCD_napis(char *tab){
	uint8_t i = 0;
	sterowanie |=(1<<RS);
	while(tab[i]){
		if (i == 15){
			LCD_napis("-");
			sterowanie &= ~(1<<RS); //instrukcje
			LCD_zapis(0xc0);
			sterowanie |= (1<<RS);//dane
		}
		LCD_zapis(tab[i++]);
	}
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
	sterowanie |= (1<<RS);//dane
	LCD_napis("Moj mail to alibaba 100");
    while (1) 
    {
			
    }
}

