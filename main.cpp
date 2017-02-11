#include "gpio.h"
using namespace io;

extern "C" {
    #include <util/delay.h>
}

class Motor {
public:
    Motor(gpio& pul, gpio& dir, gpio& ena)
        : pul(pul)
        , dir(dir)
        , ena(ena)
    {
        pul.clear();
        dir.clear();
    }

    void cw() {
        dir.clear();
    }

    void ccw() {
        dir.set();
    }

    void enable() {
        ena.clear();
    }

    void disable() {
        ena.set();
    }

    void step() {
        pul.set();
        _delay_us(10);
        pul.clear();
    }

protected:
    gpio &pul, &dir, &ena;
};

class Control {
public:
    Control(Motor &motor, uint16_t min_sps=250, uint16_t max_sps=4000, uint16_t ramp_angle=4)
        : motor(motor)
        , min_sps(min_sps)
        , max_sps(max_sps)
        , ramp_angle(ramp_angle)
    {}

    void move(int32_t input, uint16_t speed=0) {
        uint32_t sps = min_sps;
        uint32_t step = 0;
        uint32_t steps;

        if (input == 0) {
            return;
        } else if (input < 0) {
            motor.ccw();
            steps = input * -1;
        } else {
            steps = input;
            motor.cw();
        }

        if (speed == 0) {
            speed = max_sps;
        }

        uint32_t ramp = (speed - min_sps) / ramp_angle;

        while (step < steps) {
            motor.step();
            wait10us(100000 / sps);

            if (step <= (steps / 2) && step < ramp) {
                sps += ramp_angle;
            }

            if (step > (steps / 2) && (steps - step) < ramp) {
                sps -= ramp_angle;
            }

            step++;
        }
    }

protected:
    void wait10us(uint32_t us) {
        while(us--) {
            _delay_us(10);
        }
    }

protected:
    Motor &motor;
    uint16_t min_sps;
    uint16_t max_sps;
    uint16_t ramp_angle;
};

void idle(Motor &motor, gpio& led) {
    _delay_ms(250);
    motor.disable();
    led.clear();
    _delay_ms(2000);
    led.set();
    motor.enable();
    _delay_ms(250);
}

int main() {
    gpio ena(gpio::portd, gpio::pin2, gpio::out);
    gpio dir(gpio::portd, gpio::pin4, gpio::out);
    gpio pul(gpio::portd, gpio::pin6, gpio::out);

    gpio led(gpio::portb, gpio::pin5, gpio::out);
    led.set();

    Motor motor(pul, dir, ena);
    motor.enable();

    Control control(motor);

    while (true) {
        control.move(8000, 6000);
        idle(motor, led);
        control.move(-8000, 6000);
        idle(motor, led);
    }

    return 0;
}

