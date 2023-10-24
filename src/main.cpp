// Copyright 2021,2022,2023 David SPORN
// ---
// This file is part of 'ESP32/IDF workspace for IIC'.
// ---
// 'ESP32/IDF workspace for IIC' is free software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// 'ESP32/IDF workspace for IIC' is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
// Public License for more details.

// You should have received a copy of the GNU General Public License along
// with 'ESP32/IDF workspace for IIC'. If not, see <https://www.gnu.org/licenses/>. 

#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/gptimer.h"
#include "driver/i2c.h"

// sdk config
#include "sdkconfig.h"

// project includes
#include "DigitalInputOutputEsp32.hpp"
#include "FeedbackLed.hpp"
#include "GeneralPurposeInputOutput.hpp"
#include "InputButton.hpp"
#include "Task.h"
// -- WIP includes
#include "SevenSegmentsFont.hpp"
#include "Tm1637Esp32.hpp"

//====================================================================
// GPIO pins affectation from configuration
#define PIN_BUTTON_MENU gpio_num_t(CONFIG_PIN_BUTTON_MENU)
#define PIN_STATUS_MAIN gpio_num_t(CONFIG_PIN_STATUS_MAIN)

static const char *TAG_MAIN = "main";

/**
 * Main file for running the BLE samples.
 */
extern "C" {
void app_main(void);
}

//=====================================================================
// Demo : ascii
SevenSegmentFont demo_font = SevenSegmentsFontUsAscii ;
uint8_t demo_start = 32 ;
uint8_t demo_length = 96 ;
uint8_t demo_cursor = 0 ;
bool demo_is_ready = false ;
Tm1637Registers displayRegisters ;

// Demo : ascii
//=====================================================================

// Utils -- i2c data mode -- log tx data mode
void logTxDataMode(i2c_trans_mode_t mode) {
  if (I2C_DATA_MODE_MSB_FIRST == mode) {
    ESP_LOGI(TAG_MAIN, "tx mode = I2C_DATA_MODE_MSB_FIRST");
  } else if (I2C_DATA_MODE_LSB_FIRST == mode) {
    ESP_LOGI(TAG_MAIN, "tx mode = I2C_DATA_MODE_LSB_FIRST");
  } else if (I2C_DATA_MODE_MAX == mode) {
    ESP_LOGI(TAG_MAIN, "tx mode = I2C_DATA_MODE_MAX");
  } else {
    ESP_LOGI(TAG_MAIN, "tx mode has another value");
  }
}

// Utils -- i2c data mode -- log tx data mode
void checkTxDataMode(int i2c_port) {
  i2c_trans_mode_t txmode, rxmode ;

  i2c_port_t port = static_cast<i2c_port_t>(i2c_port) ;

  i2c_get_data_mode(port, &txmode, &rxmode) ;
  logTxDataMode(txmode) ;
}

// Utils -- i2c data mode -- set
void trySetDataMode(int i2c_port) {
  checkTxDataMode(i2c_port);

  ESP_ERROR_CHECK(i2c_set_data_mode((i2c_port_t)i2c_port, I2C_DATA_MODE_LSB_FIRST, I2C_DATA_MODE_LSB_FIRST));

  checkTxDataMode(i2c_port);
}

// Sample task : display updater
class DisplayUpdaterTask : public Task {
private:
  Tm1637IicBridgeEsp32 iicBridge = Tm1637IicBridgeEsp32() ;

public:
  virtual ~DisplayUpdaterTask() {}

  void run(void *data) {
    const TickType_t SLEEP_TIME = 333 / portTICK_PERIOD_MS; // 3.33 Hz

    //i2c port to use
    int i2c_port = 0;

    while (true) {
      if (demo_is_ready) {
        // sample i2c sequence

        // rework here
        displayRegisters.data.digits[0] = demo_font.glyphData[demo_start + demo_cursor];
        displayRegisters.data.digits[1] = demo_font.glyphData[demo_start + (demo_cursor+1)%demo_length];
        displayRegisters.data.digits[2] = demo_font.glyphData[demo_start + (demo_cursor+2)%demo_length];
        displayRegisters.data.digits[3] = demo_font.glyphData[demo_start + (demo_cursor+3)%demo_length];

        iicBridge.upload(&displayRegisters, i2c_port);

        demo_cursor = (demo_cursor+1)%demo_length ;
      }
      vTaskDelay(SLEEP_TIME);
    }
  }
};

// Sample task : led updater
class LedUpdaterTask : public Task {
private:
  GeneralPurposeInputOutput *gpio;
  FeedbackLed *led;

public:
  LedUpdaterTask *withGpio(GeneralPurposeInputOutput *gpio) {
    this->gpio = gpio;
    return this;
  }
  LedUpdaterTask *withLed(FeedbackLed *led) {
    this->led = led;
    return this;
  }
  void run(void *data) {
    const TickType_t SLEEP_TIME = 100 / portTICK_PERIOD_MS; // 10 Hz
    while (true) {
      gpio->getDigital()->write(CONFIG_PIN_STATUS_MAIN, led->next());
      vTaskDelay(SLEEP_TIME);
    }
  }
};

// Sample task : button watcher
class ButtonWatcherTask : public Task, public InputButtonListener {
private:
  GeneralPurposeInputOutput *gpio;
  InputButton *button;
  FeedbackLed *led;

public:
  ButtonWatcherTask *withGpio(GeneralPurposeInputOutput *gpio) {
    this->gpio = gpio;
    return this;
  }
  ButtonWatcherTask *withButton(InputButton *button) {
    this->button = button;
    button->withListener(this); // so that onInputButtonEvent(...) is called.
    return this;
  }
  ButtonWatcherTask *withLed(FeedbackLed *led) {
    this->led = led;
    return this;
  }
  void run(void *data) {
    const TickType_t SLEEP_TIME = 20 / portTICK_PERIOD_MS; // 50 Hz
    while (true) {
      button->update(gpio->getDigital()->read(CONFIG_PIN_BUTTON_MENU));
      vTaskDelay(SLEEP_TIME);
    }
  }
  void onInputButtonEvent(InputButtonEvent *event) {
    if (!isStarted())
      return; // no connection
    if (STATE_CHANGE == event->type) {
      switch (event->source->getId()) {
      case CONFIG_PIN_BUTTON_MENU:
        if (event->source->isHigh()) {
          led->setFeedbackSequenceAndLoop(BLINK_THRICE);
        } else {
          led->setFeedbackSequenceAndLoop(BLINK_ONCE);

        }
        break;
      }
    }
  }
};

// global instances
GeneralPurposeInputOutput *gpio;
Task *ledUpdater;
Task *buttonWatcher;
Task *displayUpdater;
InputButton *button;
FeedbackLed *mainLed;

void app_main(void) {
  // setup
  // -- I/O peripherals
  gpio = (new GeneralPurposeInputOutput())
             ->withDigital(new DigitalInputOutputEsp32());
  gpio->getDigital()->setup(CONFIG_PIN_STATUS_MAIN, WRITE);
  gpio->getDigital()->setup(CONFIG_PIN_BUTTON_MENU, READ);

  // -- animated led
  mainLed = new FeedbackLed();
  mainLed->setFeedbackSequenceAndLoop(BLINK_ONCE);

  // -- action button
  button = new InputButton(CONFIG_PIN_BUTTON_MENU);
  button->withDebouncer(DEBOUNCER_TYPICAL);

  // Tasks
  ledUpdater = (new LedUpdaterTask()) //
                    ->withGpio(gpio)   //
                    ->withLed(mainLed);
  ledUpdater->start();

  buttonWatcher = (new ButtonWatcherTask()) //
                      ->withGpio(gpio)      //
                      ->withLed(mainLed)    //
                      ->withButton(button);
  buttonWatcher->start();

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

  i2c_port_t port = static_cast<i2c_port_t>(i2c_master_port) ;

  i2c_param_config(port, &conf);

  trySetDataMode(i2c_master_port) ;

  ESP_ERROR_CHECK(i2c_driver_install(port, conf.mode, 0, 0, 0));

  demo_is_ready = true ;

  displayRegisters.control.switchedOn = true ;
  displayRegisters.control.brightness = 7;
  displayRegisters.data.size = 4 ;

  displayUpdater = new DisplayUpdaterTask() ;
  displayUpdater->start() ;

} // app_main
