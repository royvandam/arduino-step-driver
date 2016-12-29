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

#if defined(__AVR_ATmega328P__)
        typedef enum {
            portb, portc, portd
        } port;
#else
#error "Unsupported target"
#endif

        typedef enum {
            pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7
        } pin;

        typedef struct {
            volatile uint8_t* const out;  // Output register
            volatile uint8_t* const in;   // Input register
            volatile uint8_t* const ddr;  // Direction register
            uint8_t size;           // Amount of pins in the port
        } register_map;


#if defined(__AVR_ATmega328P__)
        enum { ports = 3 };
        static const register_map _register_map[ports];
#endif

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