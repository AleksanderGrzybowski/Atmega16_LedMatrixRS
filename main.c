#define F_CPU 16000000UL

#define USART_BAUDRATE 9600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdbool.h>

#include "HD44780.h"


volatile char display[8];
volatile int cur_row = 0; // current displayed digit

ISR(TIMER0_OVF_vect) {

	/* regulate speed of multiplexing, less = slower
	 * this is way simpler than fiddling with prescalers :)
	 */
	TCNT0 = 220;

	cur_row++;
	if (cur_row == 8) {
		cur_row = 0;
	}

//	/* turn off all to avoid ghosting TODO is that needed? */
//	PORTA = 0xff;

	/* and on */
	PORTC = ~(1 << cur_row);
	PORTA = ~display[cur_row];
}

char recv;
int is_block_mode = 0;
int cur_row_serial;

ISR(USART_RXC_vect) {
	recv = UDR;

	if (is_block_mode) {
		display[cur_row_serial++] = recv; // TODO double buffering?

		if (cur_row_serial == 8) {
			is_block_mode = 0;
		}
	} else {
		if (recv & (1 << 7)) { // init block mode
			is_block_mode = 1;
			cur_row_serial = 0;
		} else { // receive single 'dot'
			int row = (recv >> 3) & 7;
			int col = (recv >> 0) & 7;
			int state = recv & (1 << 6);

			if (state) {
				display[row] |= (1 << col);
			} else {
				display[row] &= ~(1 << col);
			}
		}

	}

	UDR = recv; // Echo back the received byte back to the computer
}

int main(void) {

	DDRA = 0xff;
	DDRC = 0xff;

	/* LCD-595 */
	DDRD |= (1 << PD4);
	DDRD |= (1 << PD5);
	DDRD |= (1 << PD6);
	LCD_Initalize();
	LCD_WriteText("Hello lcd");

	/* disable JTAG so we can use all pins */
	MCUCSR |= (1 << JTD);
	MCUCSR |= (1 << JTD);

	/* enable timer overflow interrupts for PWM */
	TIMSK |= (1 << TOIE0);
	/* set prescaler, selected by experimenting, but works perfectly */
	TCCR0 |= (1 << CS02);
	TCCR0 &= ~(1 << CS01);
	TCCR0 &= ~(1 << CS00);

	/* serial */
	UCSRB = (1 << RXEN) | (1 << TXEN); // Turn on the transmission and reception circuitry
	UCSRC = (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1); // Use 8-bit character sizes

	UBRRH = (BAUD_PRESCALE >> 8); // Load upper 8-bits of the baud rate value into the high byte of the UBRR register
	UBRRL = BAUD_PRESCALE; // Load lower 8-bits of the baud rate value into the low byte of the UBRR register

	UCSRB |= (1 << RXCIE); // Enable the USART Recieve Complete interrupt (USART_RXC)

	/* this is needed! */
	sei();


	/* testing routine */
	int i, j;
	for (i = 0; i < 8; ++i) {
		for (j = 0; j < 8; ++j) {
			display[i] |= (1 << j);
			_delay_ms(10);
			display[i] &= ~(1 << j);
		}
	}


	while (1) {

	}
}
