#include "ledring.h"
#include "gpio.h"
using namespace io;

extern "C" {
    #include <avr/interrupt.h>
    #include <avr/pgmspace.h>
    #include <util/delay.h>
}

namespace {
    // Gamma correction improves appearance of midrange colors
    const uint8_t PROGMEM gamma8[] = {
          0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
          0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,
          3,  3,  4,  4,  4,  4,  5,  5,  5,  5,  5,  6,  6,  6,  6,  7,
          7,  7,  8,  8,  8,  9,  9,  9, 10, 10, 10, 11, 11, 11, 12, 12,
         13, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20,
         20, 21, 21, 22, 22, 23, 24, 24, 25, 25, 26, 27, 27, 28, 29, 29,
         30, 31, 31, 32, 33, 34, 34, 35, 36, 37, 38, 38, 39, 40, 41, 42,
         42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
         58, 59, 60, 61, 62, 63, 64, 65, 66, 68, 69, 70, 71, 72, 73, 75,
         76, 77, 78, 80, 81, 82, 84, 85, 86, 88, 89, 90, 92, 93, 94, 96,
         97, 99,100,102,103,105,106,108,109,111,112,114,115,117,119,120,
        122,124,125,127,129,130,132,134,136,137,139,141,143,145,146,148,
        150,152,154,156,158,160,162,164,166,168,170,172,174,176,178,180,
        182,184,186,188,191,193,195,197,199,202,204,206,209,211,213,215,
        218,220,223,225,227,230,232,235,237,240,242,245,247,250,252,255
    };
}

LedRing::LedRing(uint8_t port, uint8_t pin)
    : port(port), pin(1 << pin)
    , static_value(0x00)
    , brightness(0xff)
{
    *gpio::_register_map[this->port].ddr |= this->pin;
    off(false);
}

void
LedRing::setBrightness(uint8_t value) {
  brightness = value;
}

void
LedRing::toggle() {
  if (static_value == 0x00) {
    on();
  } else {
    off();
  }
}

void
LedRing::off(bool fade) {
  if (fade) {
    while (static_value > 25) {
        setColor(static_value, static_value, static_value);
        write();
        _delay_ms(2);
        static_value--;
    }
  }

  static_value = 0x00;
  setColor(0, 0, 0);
  write();
}

void LedRing::on(bool fade) {
  if (fade) {
    while (static_value < brightness) {
      static_value++;
      setColor(static_value, static_value, static_value);
      write();
      _delay_ms(2);
    }
  } else {
    static_value = brightness;
    setColor(static_value, static_value, static_value);
    write();
  }
}

void
LedRing::setColor(uint8_t r, uint8_t g, uint8_t b) {
    for (uint8_t i = 0; i < LedRing::size; i++) {
        setColor(r, g, b, i);
    }
}

void
LedRing::setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t index) {
    Pixel* pixel = reinterpret_cast<Pixel*>(pixels + (index * 3));
    pixel->r = (uint8_t)(pgm_read_byte(&gamma8[r]) * 1.0);
    pixel->g = (uint8_t)(pgm_read_byte(&gamma8[g]) * 0.6);
    pixel->b = (uint8_t)(pgm_read_byte(&gamma8[b]) * 0.3);
}

void
LedRing::write() {
    cli();

    volatile uint16_t
        i  = LedRing::bytes; // Loop counter

    volatile uint8_t
       *ptr = pixels,   // Pointer to next byte
        b   = *ptr++,   // Current byte value
        hi,             // PORT w/output bit set high
        lo;             // PORT w/output bit set low

    // WS2811 and WS2812 have different hi/lo duty cycles; this is
    // similar but NOT an exact copy of the prior 400-on-8 code.

    // 20 inst. clocks per bit: HHHHHxxxxxxxxLLLLLLL
    // ST instructions:         ^   ^        ^       (T=0,5,13)

    volatile uint8_t next, bit;
    const volatile uint8_t* out = gpio::_register_map[this->port].out;

    hi   = *out | this->pin;
    lo   = *out & ~this->pin;
    next = lo;
    bit  = 8;

    asm volatile(
     "head20:"                   "\n\t" // Clk  Pseudocode    (T =  0)
      "st   %a[out],  %[hi]"    "\n\t" // 2    PORT = hi     (T =  2)
      "sbrc %[byte],  7"         "\n\t" // 1-2  if(b & 128)
       "mov  %[next], %[hi]"     "\n\t" // 0-1   next = hi    (T =  4)
      "dec  %[bit]"              "\n\t" // 1    bit--         (T =  5)
      "st   %a[out],  %[next]"  "\n\t" // 2    PORT = next   (T =  7)
      "mov  %[next] ,  %[lo]"    "\n\t" // 1    next = lo     (T =  8)
      "breq nextbyte20"          "\n\t" // 1-2  if(bit == 0) (from dec above)
      "rol  %[byte]"             "\n\t" // 1    b <<= 1       (T = 10)
      "rjmp .+0"                 "\n\t" // 2    nop nop       (T = 12)
      "nop"                      "\n\t" // 1    nop           (T = 13)
      "st   %a[out],  %[lo]"    "\n\t" // 2    PORT = lo     (T = 15)
      "nop"                      "\n\t" // 1    nop           (T = 16)
      "rjmp .+0"                 "\n\t" // 2    nop nop       (T = 18)
      "rjmp head20"              "\n\t" // 2    -> head20 (next bit out)
     "nextbyte20:"               "\n\t" //                    (T = 10)
      "ldi  %[bit]  ,  8"        "\n\t" // 1    bit = 8       (T = 11)
      "ld   %[byte] ,  %a[ptr]+" "\n\t" // 2    b = *ptr++    (T = 13)
      "st   %a[out], %[lo]"     "\n\t" // 2    PORT = lo     (T = 15)
      "nop"                      "\n\t" // 1    nop           (T = 16)
      "sbiw %[count], 1"         "\n\t" // 2    i--           (T = 18)
       "brne head20"             "\n"   // 2    if(i != 0) -> (next byte)
      : [out]  "+e" (out),
        [byte]  "+r" (b),
        [bit]   "+r" (bit),
        [next]  "+r" (next),
        [count] "+w" (i)
      : [ptr]    "e" (ptr),
        [hi]     "r" (hi),
        [lo]     "r" (lo));

    sei();
}
