/**
 * @file Inkplate.h
 * @author Fran Fodor for Soldered
 * @brief Top-level Inkplate class — selects the board implementation at
 *        compile time via Kconfig and provides the public API.
 *
 * https://github.com/SolderedElectronics/Inkplate-Esp-library
 * For more info about the product, please check:
 * https://docs.soldered.com/inkplate/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "WiFi.h"
#include "sdkconfig.h"

#if defined(CONFIG_INKPLATE_BOARD_INKPLATE6) ||                                \
    defined(CONFIG_INKPLATE_BOARD_INKPLATE6FLICK)
#include "Inkplate6.h"
#define INKPLATE_BOARD_CLASS Inkplate6
#elif CONFIG_INKPLATE_BOARD_INKPLATE6COLOR
#include "Inkplate6Color.h"
#define INKPLATE_BOARD_CLASS Inkplate6Color
#elif CONFIG_INKPLATE_BOARD_INKPLATE10
#include "Inkplate10.h"
#define INKPLATE_BOARD_CLASS Inkplate10
#elif defined(CONFIG_INKPLATE_BOARD_INKPLATE13)
#include "Inkplate13.h"
#define INKPLATE_BOARD_CLASS Inkplate13
#elif CONFIG_INKPLATE_BOARD_INKPLATE5
#include "Inkplate5.h"
#define INKPLATE_BOARD_CLASS Inkplate5
#elif CONFIG_INKPLATE_BOARD_INKPLATE4
#include "Inkplate4.h"
#define INKPLATE_BOARD_CLASS Inkplate4
#elif CONFIG_INKPLATE_BOARD_INKPLATE2
#include "Inkplate2.h"
#define INKPLATE_BOARD_CLASS Inkplate2
#else
#error                                                                         \
    "No Inkplate board selected. Choose a board in menuconfig -> Inkplate Board."
#endif

class Inkplate : public INKPLATE_BOARD_CLASS {
public:
  Inkplate();

  WiFi wifi;
};
