/**
 **************************************************
 *
 * @file        ditherAlgorithm.h
 * @brief       Floyd-Steinberg dithering for grayscale EPD panels.
 *              Used by Inkplate boards that operate in 1-bit (black & white)
 *              or 3-bit (8-level grayscale) display modes.
 *
 * @copyright   GNU Lesser General Public License v3.0
 * @authors     Soldered
 ***************************************************/

#pragma once

#if !defined(CONFIG_INKPLATE_BOARD_INKPLATE6COLOR) && !defined(CONFIG_INKPLATE_BOARD_INKPLATE2) && \
    !defined(CONFIG_INKPLATE_BOARD_INKPLATE13)

#include <stdint.h>
#include <string.h>

class BoardBase;

typedef struct {
    uint8_t b, g, r;
} RGBTRIPLE;

class DitherAlgorithm {
  public:
    void begin(BoardBase *inkplatePtr);
    void ditherFramebuffer(uint8_t *frameBuffer, int width, int height, uint8_t mode);

  private:
    BoardBase *_inkplate;
};

#endif
