# Tm1637 driver for Esp32/IDF
C++ library for PlatformIO, an implementation to interface with the 7-segment controller TM1637, using an I2C link of the ESP32.

[![PlatformIO Registry](https://badges.registry.platformio.org/packages/sporniket/library/Tm1637Esp32-by-sporniket.svg)](https://registry.platformio.org/libraries/sporniket/Tm1637Esp32-by-sporniket)

## LICENSE

GPL v3

## What's new

### v0.0.1

Initial publishing, extracted from working code.


## Typical application

_See [the showcase/maintenance project](https://github.com/sporniket/esp32-idf-workspace-i2c)_

```cpp
#include "esp_log.h"
#include "driver/i2c.h"

// sdk config
#include "sdkconfig.h"

// project includes
#include "Tm1637Esp32.hpp"

extern "C" {
void app_main(void);
}

Tm1637Registers displayRegisters ;
Tm1637IicBridgeEsp32 iicBridge = Tm1637IicBridgeEsp32() ;
uint8_t demo_pool[] = {
  /* ASCII  48 0   */ 0x3f,
  /* ASCII  49 1   */ 0x06,
  /* ASCII  50 2   */ 0x5b,
  /* ASCII  51 3   */ 0x4f,
  /* ASCII  52 4   */ 0x66,
  /* ASCII  53 5   */ 0x6d,
  /* ASCII  54 6   */ 0x7d,
  /* ASCII  55 7   */ 0x07,
  /* ASCII  56 8   */ 0x7f,
  /* ASCII  57 9   */ 0x6f
} ;
uint8_t demo_length = 10 ;
uint8_t demo_cursor = 0 ;

void app_main(void) {
  // setup
  // -- i2c
  int i2c_master_port = 0;
  i2c_config_t conf = {
    .mode = I2C_MODE_MASTER,
    .sda_io_num = CONFIG_PIN_IIC_1_SDA,         // select SDA GPIO specific to your project
    .scl_io_num = CONFIG_PIN_IIC_1_SCL,         // select SCL GPIO specific to your project
    .sda_pullup_en = GPIO_PULLUP_DISABLE,
    .scl_pullup_en = GPIO_PULLUP_DISABLE,
    .master = {.clk_speed = CONFIG_IIC_1_CLK_FREQ_HZ},  // select frequency specific to your project
    .clk_flags = 0,                          // optional; you can use I2C_SCLK_SRC_FLAG_* flags to choose i2c source clock here
  };

  i2c_param_config(i2c_master_port, &conf);

  ESP_ERROR_CHECK(i2c_set_data_mode(i2c_port, I2C_DATA_MODE_LSB_FIRST, I2C_DATA_MODE_LSB_FIRST));

  ESP_ERROR_CHECK(i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0));

  displayRegisters.control.switchedOn = true ;
  displayRegisters.control.brightness = 7;
  displayRegisters.data.size = 4 ;

  // execute
  // -- should be in a loop or event handler.
  displayRegisters.data.digits[0] = demo_pool[demo_cursor];
  displayRegisters.data.digits[1] = demo_pool[(demo_cursor+1)%demo_length];
  displayRegisters.data.digits[2] = demo_pool[(demo_cursor+2)%demo_length];
  displayRegisters.data.digits[3] = demo_pool[(demo_cursor+3)%demo_length];

  iicBridge.upload(&displayRegisters, i2c_port);
  // done
}
```
