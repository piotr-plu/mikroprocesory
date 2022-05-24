/*
 * GccApplication1.c
 *
 * Created: 2022-05-24 12:02:27
 * Author : oem1
 */ 

#include <avr/io.h>
#include <avr/pgmspace.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>
#define SCK 7
#define MOSI 5
#define SS 4
#define SPI_DDR DDRB
#define SPI PORTB
volatile uint8_t flaga = 0;

static const PROGMEM uint8_t digits[][8]={//wiersze cyfr
	{0x0e,0x11,0x13,0x15,0x19,0x11,0x0e,0x00},//0
	{0x04,0x0c,0x14,0x04,0x04,0x04,0x1f,0x00},//1
	{0x0e,0x11,0x01,0x02,0x04,0x08,0x1f,0x00},//2
	{0x1F,0x02,0x04,0x02,0x01,0x11,0x0e,0x00},//3
	{0x02,0x06,0x0a,0x12,0x1f,0x02,0x02,0x00},//4
	{0x1f,0x10,0x1e,0x01,0x01,0x11,0x0e,0x00},//5
	{0x06,0x08,0x10,0x1e,0x11,0x11,0x0e,0x00},//6
	{0x1f,0x01,0x02,0x04,0x08,0x08,0x08,0x00},//7
	{0x0e,0x11,0x11,0x0e,0x11,0x11,0x0e,0x00},//8
	{0x0e,0x11,0x11,0x0f,0x01,0x02,0x0c,0x00}//9
};

ISR(TIMER1_COMPA_vect){
	//if(tab4[3]++ == 9){
		//tab4[3] = 0;
		//if(tab4[2]++ == 9){
			//tab4[2] = 0;
			//if(tab4[1]++ == 9){
				//tab4[1] = 0;
				//if(tab4[0]++ == 9){
					//tab4[0] = 0;
				//}
			//}
		//}
	//}
	flaga = 1;
}

void SPI_sendbyte(uint8_t dane){
	SPDR = dane;
	while(!(SPSR & (1<<SPIF)));
}

void SPI_init(){
	SPI_DDR |= (1<<SCK)|(1<<MOSI)|(1<<SS);
	SPCR =(1<<SPE)|(1<<MSTR);
	
}

void MAX7219_data(uint8_t adres, uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4){
	SPI &= ~(1<<SS);
	SPI_sendbyte(adres);
	SPI_sendbyte(data1);
	SPI_sendbyte(adres);
	SPI_sendbyte(data2);
	SPI_sendbyte(adres);
	SPI_sendbyte(data3);
	SPI_sendbyte(adres);
	SPI_sendbyte(data4);
	SPI |= (1<<SS);
	
}

void Matrix_clear(){
	for (uint8_t i=0; i<8; i++){
		MAX7219_data(i+1,0,0,0,0);
	}
}

void Matrix_init(){
	Matrix_clear();
	MAX7219_data(0x09,0,0,0,0); //nie korzystany z bcd
	MAX7219_data(0x0a,10,10,10,10); //jasność, intensywność świecenia
	MAX7219_data(0x0b,7,7,7,7); //8 wierszy
	MAX7219_data(0x0c,1,1,1,1); //tryb normalny
	MAX7219_data(0x0f,0,0,0,0); //bez trybu testowego (wszystkie znaki)
	
}

void Matrix_show(uint8_t *tab){
	uint8_t data[4];
	for (uint8_t i=0; i<8; i++){
		for(uint8_t j=0; j<4; j++){
			data[j] = pgm_read_byte(&(digits[tab[j]][i])); //j - który wyświetlacz, i - który wiersz
		}
		MAX7219_data(i+1,data[0],data[1],data[2],data[3]); 
		
	}
}

int main(void)
{
	SPI_init();
	Matrix_init();
	
	
	TCCR1B = (1<<CS12)|(1<<WGM12); //preskaler 256 dla licznika1| tryb ctc licznika 1
	OCR1A = 62500; //liczba do porównania w liczniku w trybie pracy CTC
	TIMSK |= (1<<OCIE1A); //włączenie przerwań licznika 0 i 1 od przepełnienia
	sei(); //włączenie kontroli przewań
	uint8_t liczba[]={0,0,0,0};

	while(1){
					if (flaga ==1){
						
						if (liczba[3]++==9) 
						{
							liczba[3]=0;
							if (liczba[2]++==9) 
							{
								liczba[2]=0;
								if (liczba[1]++==9)
								 {
									 liczba[1]=0;
									 if (liczba[0]++==9) 
									 {
										 liczba[0]=0;
									}
								  }
							}
						}
							
						Matrix_show(liczba);
						flaga = 0;

				}
	}
}

