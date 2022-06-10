

#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#define LCD_DDR DDRA
#define LCD PORTA
#define RS 4
#define E 5
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
void LCD_znak(uint8_t adres, uint8_t *tab){
	for(uint8_t i=0; i<8; i++){
		LCD &= ~(1<<RS);
		LCD_zapis(0x40+adres*8+i);
		LCD |= (1<<RS);
		LCD_zapis(tab[i]);
	}
}
int main(void)
{
	uint8_t litera0[]={0x06,0x09, 0x09, 0x06,0,0,0,0};
	//uint8_t czas[] = {1,2,4,5};
	LCD_DDR |= 0x3f;
	LCD &= ~(1<<RS);
	LCD_zapis(0x33);
	LCD_zapis(0x32);
	LCD_zapis(0x28);
	LCD_zapis(0x06);
	LCD_zapis(0x0f);
	LCD_zapis(0x01);
	_delay_ms(2);
	LCD_znak(0, litera0);
	_delay_ms(2);
	LCD &= ~(1<<RS);
	LCD_zapis(0x80);
	//LCD_napis("Bagno");
	uint8_t czas[] = {1,7,2,8};
	LCD |= (1<<RS);
	LCD_zapis(czas[0]+'0');
	LCD_zapis(czas[1]+'0');
	LCD_zapis(czas[2]+'0');
	LCD_zapis(czas[3]+'0');
	LCD_zapis(0);
	while (1)
	{
		
	}
}

