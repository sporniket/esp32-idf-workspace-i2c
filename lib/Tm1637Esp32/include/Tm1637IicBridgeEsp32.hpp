#ifndef TM1637IIC_BRIDGE_ESP32_HPP
#define TM1637IIC_BRIDGE_ESP32_HPP

// standard includes
#include <cstdint>

// esp32 includes
#include "driver/i2c.h"
#include "esp_err.h"

// project includes
#include "MessageBuilderForTm1637.hpp"
#include "SevenSegmentsIicBridge.hpp"

/** @brief Iic bridge for a TM1637 for the Esp32, not thread safe !!.
 */
class Tm1637IicBridgeEsp32 : SevenSegmentsIicBridge<esp_err_t> {
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
  IicStatus<esp_err_t> send(const uint8_t *const messageData, uint8_t messageSize, IicDeviceId recipient,
                            bool ackMode);

public:
  Tm1637IicBridgeEsp32();
  virtual ~Tm1637IicBridgeEsp32();
  IicStatus<esp_err_t> upload(SevenSegmentsRegisters *registers,
                              IicDeviceId recipient);
};

#endif