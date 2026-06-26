/**
 * Placeholder cat image — replace with real image data.
 *
 * To get the actual image:
 * 1) Open the Arduino SDK example:
 *    Inkplate-Arduino-library/examples/Inkplate10/Advanced/ImageFromBuffer/cat.h
 * 2) Copy the file here and make two changes:
 *    a) Remove "PROGMEM" keyword (not needed in ESP-IDF — const arrays go to flash anyway)
 *    b) Change the type "byte" to "uint8_t"
 * Result should look like:
 *    static const uint8_t cat[] = { 0x.., 0x.., ... };
 *
 * Image format: L8 grayscale, 1200x820 pixels, 1 byte per pixel (984000 bytes total)
 */

#pragma once

#define CAT_HEIGHT 820
#define CAT_WIDTH  1200

// Placeholder: all-black image. Replace with real data from the Arduino SDK.
static const uint8_t cat[CAT_WIDTH * CAT_HEIGHT] = {};
