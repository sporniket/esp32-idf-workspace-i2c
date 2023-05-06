#ifndef TM1637TYPES_HPP
#define TM1637TYPES_HPP

// standard includes
#include <cstdint>

// esp32 includes

// project includes

// write code here
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

  // address
  ADDRESS_DISPLAY_0 = 0x0,
  ADDRESS_DISPLAY_1 = 0x1,
  ADDRESS_DISPLAY_2 = 0x2,
  ADDRESS_DISPLAY_3 = 0x3,
  ADDRESS_DISPLAY_4 = 0x4,
  ADDRESS_DISPLAY_5 = 0x5,
};

#endif