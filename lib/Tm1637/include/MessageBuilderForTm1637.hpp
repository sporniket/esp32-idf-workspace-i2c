// Copyright 2023 David SPORN
// ---
// This file is part of 'Tm1637-by-sporniket'.
// ---
// 'Tm1637-by-sporniket' is free software: you can redistribute it and/or 
// modify it under the terms of the GNU General Public License as published 
// by the Free Software Foundation, either version 3 of the License, or 
// (at your option) any later version.

// 'Tm1637-by-sporniket' is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General 
// Public License for more details.

// You should have received a copy of the GNU General Public License along 
// with 'Tm1637-by-sporniket'. If not, see <https://www.gnu.org/licenses/>. 

#ifndef MESSAGE_BUILDER_FOR_TM1637_HPP
#define MESSAGE_BUILDER_FOR_TM1637_HPP

// standard includes
#include <cstdint>

// esp32 includes

// project includes
#include "Tm1637Types.hpp"

/** @brief What the class is for.
 */
class MessageBuilderForTm1637 {
private:
  const uint8_t opcodeWriteToDisplay =
      (uint8_t)(Tm1637OpcodeParts::COMMAND_SETTING_DATA +
                Tm1637OpcodeParts::DATA_DIRECTION_WRITE_TO_DISPLAY +
                Tm1637OpcodeParts::DATA_ADDRESS_AUTOMATIC);
  const uint8_t opcodeReadKeyscan =
      (uint8_t)(Tm1637OpcodeParts::COMMAND_SETTING_DATA +
                Tm1637OpcodeParts::DATA_DIRECTION_READ_KEY_SCAN +
                Tm1637OpcodeParts::DATA_ADDRESS_AUTOMATIC);
  const uint8_t addressBaseDisplay =
      (uint8_t)(Tm1637OpcodeParts::COMMAND_SETTING_ADDRESS +
                Tm1637OpcodeParts::ADDRESS_DISPLAY_0);

  void buildSingleByteMessage(uint8_t *buffer, uint8_t value) {
    buffer[0] = 1;
    buffer[1] = value;
  }

public:
  virtual ~MessageBuilderForTm1637();
  void buildMessageForCommandWriteToDisplay(uint8_t *buffer) {
    buildSingleByteMessage(buffer, opcodeWriteToDisplay);
  }
  void buildMessageForCommandReadKeyscan(uint8_t *buffer) {
    buildSingleByteMessage(buffer, opcodeReadKeyscan);
  }
  void buildMessageForControlSwitchOff(uint8_t *buffer) {
    buildSingleByteMessage(
        buffer,
        (uint8_t)(Tm1637OpcodeParts::COMMAND_SETTING_DISPLAY_AND_CONTROL +
                  Tm1637OpcodeParts::DISPLAY_AND_CONTROL_OFF));
  }
  void buildMessageForControlBrightness(uint8_t *buffer, uint8_t brightness) {
    buildSingleByteMessage(
        buffer,
        (uint8_t)(Tm1637OpcodeParts::COMMAND_SETTING_DISPLAY_AND_CONTROL +
                  Tm1637OpcodeParts::DISPLAY_AND_CONTROL_ON +
                  (brightness &
                   Tm1637OpcodeParts::DISPLAY_AND_CONTROL_BRIGHTNESS_MASK)));
  }
  void buildMessageForAddressAndData(uint8_t *buffer, uint8_t *data, uint8_t dataLength = 6, uint8_t firstDigit = 0) {
    uint8_t dataLengthClipped = dataLength <= 6 ? dataLength : 6 ;
    uint8_t maxFirstDigit = 6 - dataLength;
    uint8_t firstDigitClipped = firstDigit <= maxFirstDigit ? firstDigit : maxFirstDigit;
    buffer[0] = 1 + dataLengthClipped ;
    buffer[1] = addressBaseDisplay+firstDigitClipped ;
    uint8_t* src = data ;
    uint8_t* dst = buffer + 2 ;
    for (uint8_t i = 0 ; i < dataLengthClipped ; i++) {
        *dst = *src;
        ++dst;
        ++src;
    }
  }
};

#endif