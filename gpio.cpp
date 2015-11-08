#include "gpio.h"
#include "assert.h"

extern "C" {
    #include <avr/io.h>
}

namespace io {

    typedef struct {
        volatile uint8_t *out;  // Output register
        volatile uint8_t *in;   // Input register
        volatile uint8_t *ddr;  // Direction register
        uint8_t size;           // Amount of pins in the port
    } gpio_register_map;

    #define gpio_register(port, register) (*_gpio_register_map[port].register)

#if defined(__AVR_ATmega328P__)
    #define gpio_ports 3

    const static gpio_register_map portx = { &PORTB, &PINB, &DDRB, 8 };

    const static gpio_register_map _gpio_register_map[gpio_ports] = {
        { &PORTB, &PINB, &DDRB, 8 },
        { &PORTC, &PINC, &DDRC, 6 },
        { &PORTD, &PIND, &DDRD, 8 }
    };
#else
    #error "Unsupported target"
#endif

    gpio::gpio(uint8_t port, uint8_t pin, gpio::direction direction)
        : _port(port)
        , _pin(1 << pin)
        , _direction(direction)
    {
        assert(port < gpio_ports);
        assert(pin < _gpio_register_map[pin].size);

        this->set_direction(direction);
    }

    void gpio::set_direction(gpio::direction direction) {
        (direction == gpio::out)
            ? (gpio_register(this->_port, ddr) |= this->_pin)
            : (gpio_register(this->_port, ddr) &= ~this->_pin);

        this->_direction = direction;
    }

    void gpio::toggle() {
        (this->_direction)
           ? (gpio_register(this->_port, in) ^= this->_pin)
           : (gpio_register(this->_port, out) ^= this->_pin);
    }

    void gpio::clear() {
        (this->_direction)
            ? (gpio_register(this->_port, in) &= ~this->_pin)
            : (gpio_register(this->_port, out) &= ~this->_pin);
    }

    void gpio::set() {
        (this->_direction)
            ? (gpio_register(this->_port, in) |= this->_pin)
            : (gpio_register(this->_port, out) |= this->_pin);
    }

    bool gpio::get() const {
        return (this->_direction)
            ? (gpio_register(this->_port, in) & this->_pin)
            : (gpio_register(this->_port, out) & this->_pin);
    }

    gpio &gpio::operator=(gpio &other) {
        (other.get()) ? this->set() : this->clear();
        return *this;
    }

    gpio &gpio::operator=(bool value) {
        (value) ? this->set() : this->clear();
        return *this;
    }

    bool gpio::operator==(bool rhs) {
        return this->get() == rhs;
    }

    bool gpio::operator!=(bool rhs) {
        return this->get() != rhs;
    }

} // namespace io