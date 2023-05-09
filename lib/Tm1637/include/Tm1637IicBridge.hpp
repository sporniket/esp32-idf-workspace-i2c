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