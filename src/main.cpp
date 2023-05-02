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

          // sample i2c sequence
          int i2c_master_port = 0;

          // -- sample command
          uint8_t comm1 = 0x02 ; //reversed bits of 0x40
          uint8_t comm2 = 0x03 ; //reversed bits of 0xC0
          uint8_t comm3 = 0xf1 ; //reversed bits of 0x8f
          bool ack_mode = true ;
          uint8_t demo1[] = {
            comm1
          } ;
          uint8_t demo2[] = {
            comm2,0xfc,0xfc,0xfc,0xfc
          } ;
          uint8_t demo3[] = {
            comm3
          } ;
          i2c_cmd_handle_t cmd ;
          cmd = i2c_cmd_link_create();
          i2c_master_start(cmd) ;
          i2c_master_write(cmd,demo1,1,ack_mode);
          i2c_master_stop(cmd);
          ESP_ERROR_CHECK(i2c_master_cmd_begin(i2c_master_port, cmd,10));
          i2c_cmd_link_delete(cmd);
          ESP_LOGI(TAG_MAIN, "done i2c command 1");

          cmd = i2c_cmd_link_create();
          i2c_master_start(cmd) ;
          i2c_master_write(cmd,demo2,7,ack_mode);
          i2c_master_stop(cmd);
          ESP_ERROR_CHECK(i2c_master_cmd_begin(i2c_master_port, cmd,10));
          i2c_cmd_link_delete(cmd);
          ESP_LOGI(TAG_MAIN, "done i2c command 2");

          cmd = i2c_cmd_link_create();
          i2c_master_start(cmd) ;
          i2c_master_write(cmd,demo3,1,ack_mode);
          i2c_master_stop(cmd);
          ESP_ERROR_CHECK(i2c_master_cmd_begin(i2c_master_port, cmd,10));
          i2c_cmd_link_delete(cmd);
          ESP_LOGI(TAG_MAIN, "done i2c command 3");


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
    .master = {.clk_speed = 420000/*CONFIG_IIC_1_CLK_FREQ_HZ*/},  // select frequency specific to your project
    .clk_flags = 0,                          // optional; you can use I2C_SCLK_SRC_FLAG_* flags to choose i2c source clock here
  };

  i2c_trans_mode_t txmode, rxmode ;
  i2c_get_data_mode(i2c_master_port, &txmode, &rxmode) ;
  if (I2C_DATA_MODE_MSB_FIRST == txmode) {
    ESP_LOGI(TAG_MAIN, "tx mode = I2C_DATA_MODE_MSB_FIRST");
  } else if (I2C_DATA_MODE_LSB_FIRST == txmode) {
    ESP_LOGI(TAG_MAIN, "tx mode = I2C_DATA_MODE_LSB_FIRST");
  } else if (I2C_DATA_MODE_MAX == txmode) {
    ESP_LOGI(TAG_MAIN, "tx mode = I2C_DATA_MODE_MAX");
  } else {
    ESP_LOGI(TAG_MAIN, "tx mode has another value");
  }

  ESP_ERROR_CHECK(i2c_set_data_mode(i2c_master_port, I2C_DATA_MODE_MSB_FIRST, I2C_DATA_MODE_MSB_FIRST));

  i2c_get_data_mode(i2c_master_port, &txmode, &rxmode) ;
  if (I2C_DATA_MODE_MSB_FIRST == txmode) {
    ESP_LOGI(TAG_MAIN, "tx mode = I2C_DATA_MODE_MSB_FIRST");
  } else if (I2C_DATA_MODE_LSB_FIRST == txmode) {
    ESP_LOGI(TAG_MAIN, "tx mode = I2C_DATA_MODE_LSB_FIRST");
  } else if (I2C_DATA_MODE_MAX == txmode) {
    ESP_LOGI(TAG_MAIN, "tx mode = I2C_DATA_MODE_MAX");
  } else {
    ESP_LOGI(TAG_MAIN, "tx mode has another value");
  }

  i2c_param_config(i2c_master_port, &conf);

  ESP_ERROR_CHECK(i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0));


} // app_main
