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
// with 'Tm1637Esp32-by-sporniket'. If not, see
// <https://www.gnu.org/licenses/>. 

#ifndef TM1637IIC_BRIDGE_ESP32_HPP
#define TM1637IIC_BRIDGE_ESP32_HPP

// standard includes
#include <cstdint>

// esp32 includes
#include "driver/i2c.h"
#include "esp_err.h"

// project includes
#include "MessageBuilderForTm1637.hpp"
#include "Tm1637IicBridge.hpp"

/** @brief Iic bridge for a TM1637 for the Esp32, not thread safe !!.
 */
class Tm1637IicBridgeEsp32 : Tm1637IicBridge<esp_err_t> {
private:
  /** @brief working buffer */
  uint8_t *const buffer = (uint8_t *)malloc(32);
  /** @brief message builder */
  MessageBuilderForTm1637 messageBuilder = MessageBuilderForTm1637();

  // subpointers to the working buffer.
  /** @brief Start of the command message to refresh the display */
  uint8_t *const command = buffer;
  /** @brief Start of the data of the command message to refresh the display */
  uint8_t *const commandData = command + 1;

  /** @brief Start of the control message to enable and setup the brightness */
  uint8_t *const displayControl = commandData + 2;
  /** @brief Start of the data of the control message to enable and setup the
   * brightness */
  uint8_t *const displayControlData = displayControl + 1;

  /** @brief Start of the address and data message to send.*/
  uint8_t *const addressAndData = displayControlData + 2;
  /** @brief Start of data of the address and data message to send.*/
  uint8_t *const addressAndDataData = addressAndData + 1;

  /** @brief Utility to send a write only message through I2C.*/
  BridgeStatus<esp_err_t> send(const uint8_t *const messageData,
                            uint8_t messageSize, BridgeId recipient,
                            bool ackMode);

public:
  Tm1637IicBridgeEsp32();
  virtual ~Tm1637IicBridgeEsp32();
  BridgeStatus<esp_err_t> upload(SevenSegmentsRegisters *registers,
                              BridgeId recipient);
  BridgeStatus<esp_err_t> scanKey(Tm1637Registers *registers,
                                BridgeId recipient){
      // NOT IMPLEMENTED YET
      BridgeStatus<esp_err_t> result = {
        .ok = false,
        .errorCode = ESP_ERR_NOT_SUPPORTED
      } ;
      return result ;
  };
};

#endif