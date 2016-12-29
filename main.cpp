#include "gpio.h"
#include "ledring.h"
using namespace io;

extern "C" {
    #include <util/delay.h>
}

int main() {
    gpio touch(gpio::portd, gpio::pin2);
    gpio led(gpio::portb, gpio::pin5, gpio::out);
    LedRing ring(gpio::portc, gpio::pin3);
    led.set();

    while (true) {
        _delay_ms(50);
        if (touch.get()) {
            led.toggle();
            ring.toggle();

            _delay_ms(50);
            while (touch.get());
        }
    }

    return 0;
}

