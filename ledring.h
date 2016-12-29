#pragma once

#include <stdint.h>

namespace io {

class LedRing {
public:
    struct Pixel {
        uint8_t g, r, b;
    };

public:
    LedRing(uint8_t port, uint8_t pin);

    void setBrightness(uint8_t value);
    void toggle();
    void on(bool fade = true);
    void off(bool fade = true);

    void setColor(uint8_t r, uint8_t g, uint8_t b);
    void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t index);

    void write();

protected:
    enum { 
        size = 24, 
        bytes = size * sizeof(Pixel)
    };

    uint8_t port;
    uint8_t pin;

    uint8_t static_value;
    uint8_t brightness;
    
    uint8_t pixels[bytes];
};

}