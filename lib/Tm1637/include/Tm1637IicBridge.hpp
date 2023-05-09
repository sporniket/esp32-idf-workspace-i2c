#ifndef TM1637IIC_BRIDGE_HPP
#define TM1637IIC_BRIDGE_HPP

// standard includes
#include <cstdint>

// esp32 includes

// project includes
#include "SevenSegmentsIicBridge.hpp"
#include "Tm1637Types.hpp"

/** @brief Model of a module controlled by a TM1637, and addressable through an I2C link.
 * 
 * Compared to a basic 7-segment display module, a TM1637 is able to manage a keyboard of up to 16 keys.
 */
template <typename ReturnCode>
class Tm1637IicBridge : SevenSegmentsIicBridge<ReturnCode> {
private:
public:
  virtual ~Tm1637IicBridge() {}
  virtual IicStatus<ReturnCode> scanKey(Tm1637Registers *registers,
                                        IicDeviceId recipient) = 0 ;
};

#endif