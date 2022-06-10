/*
 * GccApplication1.c
 *
 * Created: 2022-05-24 12:02:27
 * Author : oem1
 */ 

#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#define RS 0
#define RW 1
#define E 2
#define AK 3

void TWI_Start(){ //generacja bitu start
	TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
}

void TWI_Stop(){ //generacja bitu stop
	TWCR=(1<<TWINT)|(1<<TWSTO)|(1<<TWEN);
	while((TWCR & (1<<TWSTO)));
}

void TWI_Write(uint8_t data){ //funkcja zapisu danych
	TWDR = data;
	TWCR = (1<<TWINT)|(1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
}

void TWI_init(){ //uruchomienie połączenia
	//DUPA
	TWBR = 92; //do wzoru z wykładu, TWPS=0
	TWCR = (1<<TWEA);
}

void LCD_write(uint8_t dane, uint8_t rs){
	uint8_t co;
	TWI_Start();
	TWI_Write(0x4e);
	co=0;
	co |= (rs<<RS)|(1<<AK)|(1<<E)|(dane & 0xf0);
	TWI_Write(co);
	co &= ~(1<<E);
	TWI_Write(co);

	co=0;
	co |= (rs<<RS)|(1<<AK)|(1<<E)|((dane<<4) & 0xf0);//to samo co wyżej tylko bity przesuwamy xD
	TWI_Write(co);
	co &= ~(1<<E);
	TWI_Write(co);
	TWI_Stop();


}

void LCD_init(){
	LCD_write(0x33,0);
	LCD_write(0x32,0);
	LCD_write(0x28,0);
	LCD_write(0x06,0);
	LCD_write(0x0c,0);//wl kursor
	LCD_write(0x01,0);//czyscimy ekran
	_delay_ms(2);
}
void LCD_text(char *tab){ //z nawrotem choroby
	uint8_t i = 0;
	while(*tab){
		if(i == 15){
		LCD_write(0xc0, 0);
		
		}
		else if(i == 31){
			i = 0;
			LCD_write(0x80, 0);
			
		}
		LCD_write(*tab++,1);
		i++;
	}
}


int main(void)
{
    /* Replace with your application code */
	TWI_init();
	LCD_init();
	LCD_text("Konstantynopolitanczykiewiczasdasdasdasdasdasd");
    while (1) 
    {
    }
}

