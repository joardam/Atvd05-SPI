#ifndef __FONTS_H
#define __FONTS_H

#include <stdint.h>

/* Font definitions for 8x8 LED Matrix (MAX7219) */

// Character 'T'
static const uint8_t font_T[8] = {
    0xFF, // 11111111
    0x18, // 00011000
    0x18, // 00011000
    0x18, // 00011000
    0x18, // 00011000
    0x18, // 00011000
    0x18, // 00011000
    0x18  // 00011000
};

// Character 'V'
static const uint8_t font_V[8] = {
    0x81, // 10000001
    0x81, // 10000001
    0x42, // 01000010
    0x42, // 01000010
    0x24, // 00100100
    0x24, // 00100100
    0x18, // 00011000
    0x18  // 00011000
};

// Character 'L'
static const uint8_t font_L[8] = {
    0xC0, // 11000000
    0xC0, // 11000000
    0xC0, // 11000000
    0xC0, // 11000000
    0xC0, // 11000000
    0xC0, // 11000000
    0xFF, // 11111111
    0xFF  // 11111111
};

// Character '+'
static const uint8_t font_plus[8] = {
    0x00, // 00000000
    0x18, // 00011000
    0x18, // 00011000
    0xFF, // 11111111
    0xFF, // 11111111
    0x18, // 00011000
    0x18, // 00011000
    0x00  // 00000000
};

// Character '-'
static const uint8_t font_minus[8] = {
    0x00, // 00000000
    0x00, // 00000000
    0x00, // 00000000
    0xFF, // 11111111
    0xFF, // 11111111
    0x00, // 00000000
    0x00, // 00000000
    0x00  // 00000000
};

#endif /* __FONTS_H */
