#include "HD44780.h"

int _value;

#define DS_PORT PORTD
#define DS_DDR DDRD
#define DS_NUM (1 << PD4)

#define SHCP_PORT PORTD
#define SHCP_DDR DDRD
#define SHCP_NUM (1 << PD5)

#define STCP_PORT PORTD
#define STCP_DDR DDRD
#define STCP_NUM (1 << PD6)

void DS_1() {
	DS_PORT |= DS_NUM;
}
void DS_0() {
	DS_PORT &= ~DS_NUM;
}
void SHCP_1() {
	SHCP_PORT |= SHCP_NUM;
}
void SHCP_0() {
	SHCP_PORT &= ~SHCP_NUM;
}
void STCP_1() {
	STCP_PORT |= STCP_NUM;
}
void STCP_0() {
	STCP_PORT &= ~STCP_NUM;
}

void transmit(char b) {
	int cur;
	for (cur = 128; cur != 0; cur /= 2) {
		if (b & cur) {
			DS_1();
		} else {
			DS_0();
		}

//		delay_ms(1);
		SHCP_1();
//		delay_ms(1);
		SHCP_0();
//		delay_ms(1);
	}
}

void commit() {
//	delay_ms(1);
	STCP_1();
//	delay_ms(1);
	STCP_0();
//	delay_ms(1);
}

void lcd_push() {
	transmit(_value);
	commit();
}

void _LCD_OutNibble(unsigned char nibbleToWrite) {
	if (nibbleToWrite & 0x01)
		_value |= LCD_DB4;
	else
		_value &= ~LCD_DB4;

	if (nibbleToWrite & 0x02)
		_value |= LCD_DB5;
	else
		_value &= ~LCD_DB5;

	if (nibbleToWrite & 0x04)
		_value |= LCD_DB6;
	else
		_value &= ~LCD_DB6;

	if (nibbleToWrite & 0x08)
		_value |= LCD_DB7;
	else
		_value &= ~LCD_DB7;

}
//-------------------------------------------------------------------------------------------------
//
// Funkcja zapisu bajtu do wy�wietacza (bez rozr�nienia instrukcja/dane).
//
//-------------------------------------------------------------------------------------------------
void _LCD_Write(unsigned char dataToWrite) {
	_value |= LCD_E;
	_LCD_OutNibble(dataToWrite >> 4);
	lcd_push();
	_value &= ~LCD_E;
	lcd_push();
	_value |= LCD_E;
	_LCD_OutNibble(dataToWrite);
	lcd_push();
	_value &= ~LCD_E;
	lcd_push();
	_delay_us(50);
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja zapisu rozkazu do wy�wietlacza
//
//-------------------------------------------------------------------------------------------------
void LCD_WriteCommand(unsigned char commandToWrite) {
	_value &= ~LCD_RS;
	_LCD_Write(commandToWrite);
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja zapisu danych do pami�ci wy�wietlacza
//
//-------------------------------------------------------------------------------------------------
void LCD_WriteData(unsigned char dataToWrite) {
	_value |= LCD_RS;
	_LCD_Write(dataToWrite);
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja wy�wietlenia napisu na wyswietlaczu.
//
//-------------------------------------------------------------------------------------------------
void LCD_WriteText(char * text) {
	while (*text)
		LCD_WriteData(*text++);
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja ustawienia wsp�rz�dnych ekranowych
//
//-------------------------------------------------------------------------------------------------
void LCD_GoTo(unsigned char x, unsigned char y) {
	LCD_WriteCommand(HD44780_DDRAM_SET | (x + (0x40 * y)));
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja czyszczenia ekranu wy�wietlacza.
//
//-------------------------------------------------------------------------------------------------
void LCD_Clear(void) {
	LCD_WriteCommand(HD44780_CLEAR);
	_delay_ms(2);
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja przywr�cenia pocz�tkowych wsp�rz�dnych wy�wietlacza.
//
//-------------------------------------------------------------------------------------------------
void LCD_Home(void) {
	LCD_WriteCommand(HD44780_HOME);
	_delay_ms(2);
}
//-------------------------------------------------------------------------------------------------
//
// Procedura inicjalizacji kontrolera HD44780.
//
//-------------------------------------------------------------------------------------------------
void LCD_Initalize(void) {
	// kelog
	MCUCSR |= (1 << JTD);
	MCUCSR |= (1 << JTD);
	DS_DDR |= DS_NUM;
	STCP_DDR |= STCP_NUM;
	SHCP_DDR |= SHCP_NUM;
	// endkelog

	unsigned char i;

	_value |= LCD_E;   //
	lcd_push();
	_value |= LCD_RS;  //
	lcd_push();
	_delay_ms(15); // oczekiwanie na ustalibizowanie si� napiecia zasilajacego
	_value &= ~LCD_RS; // wyzerowanie linii RS
	lcd_push();
	_value &= ~LCD_E;  // wyzerowanie linii E
	lcd_push();

	for (i = 0; i < 3; i++) // trzykrotne powt�rzenie bloku instrukcji
			{
		_value |= LCD_E; //  E = 1
		_LCD_OutNibble(0x03); // tryb 8-bitowy
		lcd_push();
		_value &= ~LCD_E; // E = 0
		lcd_push();
		_delay_ms(5); // czekaj 5ms
	}
	lcd_push();
	_value |= LCD_E; // E = 1
	_LCD_OutNibble(0x02); // tryb 4-bitowy
	lcd_push();
	_value &= ~LCD_E; // E = 0
	lcd_push();

	_delay_ms(1); // czekaj 1ms
	LCD_WriteCommand(
	HD44780_FUNCTION_SET | HD44780_FONT5x7 | HD44780_TWO_LINE | HD44780_4_BIT); // interfejs 4-bity, 2-linie, znak 5x7
	LCD_WriteCommand(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_OFF); // wy��czenie wyswietlacza
	LCD_WriteCommand(HD44780_CLEAR); // czyszczenie zawartos�i pamieci DDRAM
	_delay_ms(2);
	LCD_WriteCommand(
	HD44780_ENTRY_MODE | HD44780_EM_SHIFT_CURSOR | HD44780_EM_INCREMENT); // inkrementaja adresu i przesuwanie kursora
	LCD_WriteCommand(
			HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_ON | HD44780_CURSOR_OFF
					| HD44780_CURSOR_NOBLINK); // w��cz LCD, bez kursora i mrugania
}

//-------------------------------------------------------------------------------------------------
//
// Koniec pliku HD44780.c
//
//-------------------------------------------------------------------------------------------------
