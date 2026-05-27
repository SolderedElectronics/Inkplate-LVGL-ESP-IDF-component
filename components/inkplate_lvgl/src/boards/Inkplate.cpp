/**
 * @file Inkplate.cpp
 * @author Fran Fodor for Soldered
 * @brief Top-level Inkplate class implementation.
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

#include "Inkplate.h"

Inkplate::Inkplate(lv_display_render_mode_t mode) : INKPLATE_BOARD_CLASS(mode) {
  clearDisplay();
#if defined(CONFIG_INKPLATE_BOARD_INKPLATE2)
  setRotation(3);
#elif defined(CONFIG_INKPLATE_BOARD_INKPLATE6COLOR)
  setRotation(2);
#elif defined(CONFIG_INKPLATE_BOARD_INKPLATE4)
  setRotation(1);
#elif defined(CONFIG_INKPLATE_BOARD_INKPLATE13)
  setRotation(3);
#endif
}
