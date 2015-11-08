#pragma once

#include <stdint.h>

namespace io {

class gpio {
    public:
        typedef enum {
            out = 0,
            in = 1
        } direction;

        typedef enum {
            on_rising_edge,
            on_faling_edge,
            on_edge_change
        } event;

        typedef enum {
            portb = 0, portc, portd
        } port;

        typedef enum {
            pin0 =0 , pin1, pin2, pin3, pin4, pin5, pin6, pin7
        } pin;

    protected:
        uint8_t _port;
        uint8_t _pin;

        gpio::direction _direction;

    public:
        //explicit gpio(gpio &other);
        explicit gpio(uint8_t port, uint8_t pin, gpio::direction direction = gpio::in);

        void set_direction(gpio::direction direction);

        void toggle();
        void clear();
        void set();
        bool get() const;

        gpio &operator=(gpio &other);
        gpio &operator=(bool value);

        bool operator==(bool rhs);
        bool operator!=(bool rhs);
};

} // namespace io