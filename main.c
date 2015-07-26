#define F_CPU 16000000UL

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

	/* turn off all to avoid ghosting */
	PORTA = 0xff;
	PORTC = ~(1 << cur_row);

	/* and on */
	PORTA = ~display[cur_row];
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
	/* this is needed! */
	sei();


	while(1) {
		int i, j;

		for (i = 0; i < 8; ++i) {
			for (j = 0; j < 256; j++) {
				display[i] = j;
				_delay_ms(10);
			}
		}

	}
}
