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

// header include
#include "Tm1637IicBridgeEsp32.hpp"

Tm1637IicBridgeEsp32::Tm1637IicBridgeEsp32() {
  messageBuilder.buildMessageForCommandWriteToDisplay(command);
}
// write code here...
Tm1637IicBridgeEsp32::~Tm1637IicBridgeEsp32() {
  if (nullptr != buffer) {
    free(buffer);
  }
}

BridgeStatus<esp_err_t>
Tm1637IicBridgeEsp32::upload(SevenSegmentsRegisters *registers,
                             BridgeId recipient) {
  bool ack_mode = true;

  messageBuilder.buildMessageForAddressAndData(
      addressAndData, registers->data.digits, registers->data.size);
  if (registers->control.switchedOn) {
    messageBuilder.buildMessageForControlBrightness(
        displayControl,
        registers->control.brightness &
            Tm1637OpcodeParts::DISPLAY_AND_CONTROL_BRIGHTNESS_MASK);
  } else {
    messageBuilder.buildMessageForControlSwitchOff(
        displayControl);
  }

  // -- sequence of commands
  BridgeStatus<esp_err_t> result;
  result = send(commandData, command[0], recipient, ack_mode);
  if (!result.ok) {
    return result;
  }
  result = send(addressAndDataData, addressAndData[0], recipient, ack_mode);
  if (!result.ok) {
    return result;
  }
  result = send(displayControlData, displayControl[0], recipient, ack_mode);
  return result;
}

BridgeStatus<esp_err_t>
Tm1637IicBridgeEsp32::send(const uint8_t *const messageData,
                           uint8_t messageSize, BridgeId recipient,
                           bool ackMode) {
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write(cmd, messageData, messageSize, ackMode);
  i2c_master_stop(cmd);
  esp_err_t status = i2c_master_cmd_begin(recipient, cmd, 10);
  i2c_cmd_link_delete(cmd);
  BridgeStatus<esp_err_t> result = {.ok = (ESP_OK == status), .errorCode = status};
  return result;
}
