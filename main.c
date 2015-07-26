#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdbool.h>

int main(void) {

	DDRA = 0xff;

	while(1) {
		_delay_ms(50);
		PORTA ^= 0xff;
	}
}
