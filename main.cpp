#include "gpio.h"
using namespace io;

extern "C" {
    #include <util/delay.h>
}

int main() {
    gpio led(gpio::portb, gpio::pin5, gpio::out);

    while (true) {
        led.set();
        _delay_ms(500);
        led.clear();
        _delay_ms(500);
    }

    return 0;
}

