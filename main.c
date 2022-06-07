#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>
// do miernika
#define wyborDDR DDRC
#define wybor PORTC
// do przycisku
#define przycisk 7
//do wyświetlacza
#define LCD_DDR DDRB
#define LCD PORTB
#define RS 4
#define E 5

// zmienne 
volatile uint16_t srednia_0 = 0;
volatile uint16_t srednia_1 = 0;
volatile uint16_t pomiar = 0;
volatile uint16_t wynik_1 = 0;
volatile uint16_t wynik_0 = 0;
volatile uint8_t kanal=0;
volatile uint16_t min =0;
volatile uint16_t max =0;
volatile uint8_t pomoc = 1.5;
volatile uint8_t pozycja = 0;
volatile uint8_t klawisz = 0;

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
// Do poprawy wejście funkcji albo musi przyjąć float albo zmienić dokładność na 255/16 = 15.93 - i tu jest problem bo jak damy 16 to pasek nigdy do końca nie dojdzie
void LCD_pasek(const *srednia){
	uint8_t ilosc = (1.5) / (0.159375); //TESTOWO WPISANE NA STAŁE
	uint8_t puste = 16 - ilosc;
	while(ilosc){
		LCD_zapis(0xFF);
		ilosc--;
	}
	
	while(puste){
		LCD_zapis(0x5F);
		puste--;
	}
}
void LCD_czysc(){
	LCD &= ~(1<<RS);
	LCD_zapis(0x01);
	_delay_ms(2);
	LCD |= (1<<RS);
}
void LCD_adres(uint8_t adres){
		LCD &= ~(1<<RS); // Ustawienie LCD w tryby instrukcji
		LCD_zapis(adres);
		LCD |= (1<<RS); //dane
}
void LCD_init(){
	LCD_DDR |= 0x3f;
	LCD_DDR |= (1<<E)|(1<<RS); // Ustawienie wyjścia E i RS jako wyjścia
	LCD &= ~(1<<RS); // Ustawienie wyświetlacza w tryby instrukcji
	LCD_zapis(0x33); // Ustawienie wyświetlacza w tryb 4 bitowy
	LCD_zapis(0x32); // -//-
	LCD_zapis(0x28); // -//-
	LCD_zapis(0x06); // Inkrementacja adresu ( Entry mode set)
	LCD_zapis(0xC);	 //	Wylaczenie migotania kursora
	LCD_zapis(0x01); // Wyczyszczenie ekranu
	_delay_ms(2);
}
void ADC_init(){
	wyborDDR |= 0x0f;
	ADMUX = (1<<REFS1)|(1<<REFS0)|(0<<MUX0); //Vref=2,55V i 0 port
	ADCSRA = (1<<ADEN)|(1<<ADIE)|(1<<ADATE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); //włączenie ADC i preskaler 128
}
void Button_init(){
	PORTB |= (1<<przycisk); // Wlaczenie rezystora polaryzujacego
}
void AVG(uint16_t wartosc, uint16_t *srednia){
	static uint8_t licznik = 0;
	if(licznik++==16){
		kanal = ~kanal;
		licznik = 0;
		(*srednia) = (pomiar>>4);
		pomiar = 0;
		ADMUX &= ~(1>>MUX0);
		if((*srednia)>max)max=(*srednia);
		if((*srednia)<min)min=(*srednia);
	}
	pomiar += wartosc;
}

// Funkcja aktulizująca pomiar i pasek na ekranie
void ADC_update(){
		switch(pozycja){
			//case 0:
			case 1: //Wyświetlenie obydwu kanałów
				LCD_adres(0x86);
				LCD_zapis(srednia_0);
				LCD_adres(0xC6);
				LCD_zapis(srednia_1);
				break;
			case 2: //Wyświetlenie kanału 1
				LCD_adres(0x86);
				LCD_zapis(srednia_0);
				LCD_adres(0xC0);
				LCD_pasek(srednia_0);
				break;
			case 3: //Wyświetlenie kanału 2
				LCD_adres(0x86);
				LCD_zapis(srednia_1);
				LCD_adres(0xC0);
				LCD_pasek(srednia_1);
			break;
		}
}

// Przerwanie od porównania, rozpoczyna pomiar ADC oraz aktulizuje wartości na ekranie
ISR(TIMER1_COMPA_vect){
	ADCSRA |= (1<<ADSC); // ADSC - wpisanie 1 rozpoczyna przetwarzanie, po jego zakończeniu pojawia się tam 0
	ADC_update();
}
// Przerwanie od końca pomiaru ADC
ISR(ADC_vect){
	switch(kanal){
		case 0:
		srednia_0 = ADC;
			//AVG(ADC, srednia_0);                           
		case 1:
		srednia_1 = 1;
			//AVG(ADC, srednia_1);
	}
	
	
}

//  Przerwanie od przepełnienia kontrolujące stan przycisku z eliminacja efektu drgania stykow
ISR(TIMER0_OVF_vect){
	if (!(PINB & (1<<przycisk))){
		switch(klawisz){
			case 0:
			klawisz = 1;
			case 1:
			klawisz = 2;
		}
	}
	else{
		switch(klawisz){
			case 3:
			klawisz = 4;
			case 4:
			klawisz = 0;
		}
	}
}

int main(void)
{

	TCCR1B=(1<<WGM12)|(1<<CS12); //WGM12 - tryb pracy CTC ( zerowanie licznika po wykryciu zgodności porównania), CS12 - źródło sygnału taktującego z prescalera 256
	OCR1A = 62500; //wartość do porównania
	
	TCCR0 = (1<<CS02) | (1<<CS00); // Ustawienie prescalera na 1024 dla licznika 0
	TIMSK = (1<<TOIE0)|(1<<OCIE1A); //włączenie przerwania OVF T0 NIE WIEMY CZY POTRZEBNY JEST TIMER 1

	LCD_init();
	Button_init();
	ADC_init();
	sei();
	
	// Menu powitalne
	LCD_napis("   Woltomierz");
	LCD_adres(0xC0);
	LCD_napis("  Wersja: 0.5");
	
	while (1)
	{
		if(klawisz == 2){
			if (++pozycja > 3)pozycja = 1;
	
		switch(pozycja){
			case 1: //Wyświetlenie obydwu kanałów
			LCD_czysc();
			LCD_napis("CH 0: ");
			LCD_adres(0xC0);
			LCD_napis("CH 1: ");
			break;
			case 2: //Wyświetlenie kanału 1
			LCD_czysc();
			LCD_napis("CH 0: ");
			break;
			case 3: //Wyświetlenie kanału 2
			LCD_czysc();
			LCD_napis("CH 1: ");
			break;
		}
				klawisz = 3;
			}
	}
}
