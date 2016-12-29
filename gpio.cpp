#include "gpio.h"
#include "assert.h"

extern "C" {
    #include <avr/io.h>
}

namespace io {

#if defined(__AVR_ATmega328P__)
    const gpio::register_map gpio::_register_map[gpio::ports] = {
        { &PORTB, &PINB, &DDRB, 8 },
        { &PORTC, &PINC, &DDRC, 6 },
        { &PORTD, &PIND, &DDRD, 8 }
    };
#endif

    #define gpio_register(port, register) (*gpio::_register_map[port].register)

    gpio::gpio(uint8_t port, uint8_t pin, gpio::direction direction)
        : _port(port)
        , _pin(1 << pin)
        , _direction(direction)
    {
        assert(port < gpio::ports);
        assert(pin < gpio::_register_map[port].size);

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
        return (this->_direction == gpio::in)
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