#include "assert.h"

extern "C" {
    #include <avr/io.h>
    #include <util/delay.h>
}

void assert_failed(char const*, int) {
    DDRB |= (1 << PB5);
    while (true) {
        PORTB |= (1 << PB5);
        _delay_ms(100);
        PORTB &= ~(1 << PB5);
        _delay_ms(100);
    }
}