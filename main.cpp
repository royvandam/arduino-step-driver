#include "gpio.h"
using namespace io;

extern "C" {
    #include <util/delay.h>
}

class Motor {
public:
    Motor(gpio& a0, gpio& a1, gpio& b0, gpio& b1)
        : a0(a0), a1(a1)
        , b0(b0), b1(b1)
        , step(0)
    {
        a0.set();
        b0.set();
        a1.clear();
        b1.clear();
    }

    void step_forward() {
        if (++step == 4) step = 0;
        take_step();
    }

    void step_back() {
        if (--step == -1) step = 3;
        take_step();
    }

protected:
    void take_step() {
        switch (step) {
            case 0:
                b0.set();
                b1.clear();
                break;
            case 1:
                a0.clear();
                a1.set();
                break;
            case 2:
                b0.clear();
                b1.set();
                break;
            case 3:
                a0.set();
                a1.clear();
                break;
        }
    }

protected:
    gpio &a0, &a1, &b0, &b1;
    int8_t step;
};

int main() {
    gpio a0(gpio::portd, gpio::pin6, gpio::out);
    gpio a1(gpio::portd, gpio::pin7, gpio::out);
    gpio b0(gpio::portb, gpio::pin0, gpio::out);
    gpio b1(gpio::portb, gpio::pin1, gpio::out);

    gpio led(gpio::portb, gpio::pin5, gpio::out);
    led.clear();

    Motor motor(a0, a1, b0, b1);

    uint16_t steps = 0;
    bool dir = false;

    while (true) {
        if (dir)
            motor.step_back();
        else
            motor.step_forward();

        if (++steps == 200) {
            steps = 0;
            dir = !dir;
            _delay_ms(1000);
        }

        led.toggle();
        _delay_ms(2);
    }

    return 0;
}

