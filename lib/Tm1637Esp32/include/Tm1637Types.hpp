// Copyright 2023 David SPORN
// ---
// This file is part of 'Tm1637Esp32-by-sporniket'.
// ---
// 'Tm1637Esp32-by-sporniket' is free software: you can redistribute it and/or 
// modify it under the terms of the GNU General Public License as published 
// by the Free Software Foundation, either version 3 of the License, or 
// (at your option) any later version.

// 'Tm1637Esp32-by-sporniket' is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General 
// Public License for more details.

// You should have received a copy of the GNU General Public License along 
// with 'Tm1637Esp32-by-sporniket'. If not, see <https://www.gnu.org/licenses/>. 

#ifndef TM1637TYPES_HPP
#define TM1637TYPES_HPP

// standard includes
#include <cstdint>

// esp32 includes

// project includes

// write code here
#include "SevenSegmentsTypes.hpp"

/** @brief type for storing the keyscan value. */
using Tm1637Keyscan = uint8_t;

const uint8_t TM1637_MAX_DIGITS = 6;
const SevenSegmentsBrightness TM1637_MAX_BRIGHTNESS = 7;

/** @brief Registers of the */
struct Tm1637Registers : SevenSegmentsRegisters {
  /** @brief store the keyscan read from the controller. */
  Tm1637Keyscan keyscan = 0;
};

/** @brief Parts to build full opcodes.
 *
 * One should use one `COMMAND_SETTING_xxx` value, and add parts of the
 * respective command.
 */
enum Tm1637OpcodeParts {
  // Command parts
  COMMAND_SETTING_DATA = 0x40,
  COMMAND_SETTING_DISPLAY_AND_CONTROL = 0x80,
  COMMAND_SETTING_ADDRESS = 0xc0,

  // data command parts
  // -- direction
  DATA_DIRECTION_WRITE_TO_DISPLAY = 0x0,
  DATA_DIRECTION_READ_KEY_SCAN = 0x2,
  // -- address mode
  DATA_ADDRESS_AUTOMATIC = 0x0,
  DATA_ADDRESS_FIXED = 0x4,
  // -- skip test mode, always normal

  // display and control command parts
  // -- brightness
  DISPLAY_AND_CONTROL_BRIGHTNESS_0 = 0x0,
  DISPLAY_AND_CONTROL_BRIGHTNESS_1 = 0x1,
  DISPLAY_AND_CONTROL_BRIGHTNESS_2 = 0x2,
  DISPLAY_AND_CONTROL_BRIGHTNESS_3 = 0x3,
  DISPLAY_AND_CONTROL_BRIGHTNESS_4 = 0x4,
  DISPLAY_AND_CONTROL_BRIGHTNESS_5 = 0x5,
  DISPLAY_AND_CONTROL_BRIGHTNESS_6 = 0x6,
  DISPLAY_AND_CONTROL_BRIGHTNESS_7 = 0x7,
  DISPLAY_AND_CONTROL_BRIGHTNESS_MASK = 0x7,
  // -- control : on/off
  DISPLAY_AND_CONTROL_OFF = 0x0,
  DISPLAY_AND_CONTROL_ON = 0x8,

  // Address command parts
  // -- display registers
  ADDRESS_DISPLAY_0 = 0x0,
  ADDRESS_DISPLAY_1 = 0x1,
  ADDRESS_DISPLAY_2 = 0x2,
  ADDRESS_DISPLAY_3 = 0x3,
  ADDRESS_DISPLAY_4 = 0x4,
  ADDRESS_DISPLAY_5 = 0x5,
};

#endif