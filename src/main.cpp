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
#include "MessageBuilderForTm1637.hpp"

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
uint8_t demo_pool[] = {
  /* ASCII  32 spa */ 0x00,
  /* ASCII  33 !   */ 0x6b,
  /* ASCII  34 "   */ 0x22,
  /* ASCII  35 #   */ 0x00,
  /* ASCII  36 $   */ 0x00,
  /* ASCII  37 %   */ 0x00,
  /* ASCII  38 &   */ 0x00,
  /* ASCII  39 '   */ 0x02,
  /* ASCII  40 (   */ 0x39,
  /* ASCII  41 )   */ 0x0f,
  /* ASCII  42 *   */ 0x00,
  /* ASCII  43 +   */ 0x00,
  /* ASCII  44 ,   */ 0x0c,
  /* ASCII  45 -   */ 0x40,
  /* ASCII  46 .   */ 0x00,
  /* ASCII  47 /   */ 0x52,
  /* ASCII  48 0   */ 0x3f,
  /* ASCII  49 1   */ 0x06,
  /* ASCII  50 2   */ 0x5b,
  /* ASCII  51 3   */ 0x4f,
  /* ASCII  52 4   */ 0x66,
  /* ASCII  53 5   */ 0x6d,
  /* ASCII  54 6   */ 0x7d,
  /* ASCII  55 7   */ 0x07,
  /* ASCII  56 8   */ 0x7f,
  /* ASCII  57 9   */ 0x6f,
  /* ASCII  58 :   */ 0x48,
  /* ASCII  59 ;   */ 0x4c,
  /* ASCII  60 <   */ 0x61,
  /* ASCII  61 =   */ 0x48,
  /* ASCII  62 >   */ 0x43,
  /* ASCII  63 ?   */ 0x4b,
  /* ASCII  64 @   */ 0x7b,
  /* ASCII  65 A   */ 0x77,
  /* ASCII  66 B   */ 0x7c,
  /* ASCII  67 C   */ 0x39,
  /* ASCII  68 D   */ 0x5e,
  /* ASCII  69 E   */ 0x79,
  /* ASCII  70 F   */ 0x71,
  /* ASCII  71 G   */ 0x3d,
  /* ASCII  72 H   */ 0x74,
  /* ASCII  73 I   */ 0x30,
  /* ASCII  74 J   */ 0x1e,
  /* ASCII  75 K   */ 0x76,
  /* ASCII  76 L   */ 0x38,
  /* ASCII  77 M   */ 0x37,
  /* ASCII  78 N   */ 0x54,
  /* ASCII  79 O   */ 0x5c,
  /* ASCII  80 P   */ 0x73,
  /* ASCII  81 Q   */ 0x67,
  /* ASCII  82 R   */ 0x50,
  /* ASCII  83 S   */ 0x6d,
  /* ASCII  84 T   */ 0x78,
  /* ASCII  85 U   */ 0x1c,
  /* ASCII  86 V   */ 0x1c,
  /* ASCII  87 W   */ 0x3e,
  /* ASCII  88 X   */ 0x76,
  /* ASCII  89 Y   */ 0x72,
  /* ASCII  90 Z   */ 0x5b,
  /* ASCII  91 [   */ 0x39,
  /* ASCII  92 \   */ 0x64,
  /* ASCII  93 ]   */ 0x0f,
  /* ASCII  94 ^   */ 0x23,
  /* ASCII  95 _   */ 0x08,
  /* ASCII  96 `   */ 0x20,
  /* ASCII  97 a   */ 0x77,
  /* ASCII  98 b   */ 0x7c,
  /* ASCII  99 c   */ 0x39,
  /* ASCII 100 d   */ 0x5e,
  /* ASCII 101 e   */ 0x79,
  /* ASCII 102 f   */ 0x71,
  /* ASCII 103 g   */ 0x3d,
  /* ASCII 104 h   */ 0x74,
  /* ASCII 105 i   */ 0x30,
  /* ASCII 106 j   */ 0x1e,
  /* ASCII 107 k   */ 0x76,
  /* ASCII 108 l   */ 0x38,
  /* ASCII 109 m   */ 0x37,
  /* ASCII 110 n   */ 0x54,
  /* ASCII 111 o   */ 0x5c,
  /* ASCII 112 p   */ 0x73,
  /* ASCII 113 q   */ 0x67,
  /* ASCII 114 r   */ 0x50,
  /* ASCII 115 s   */ 0x6d,
  /* ASCII 116 t   */ 0x78,
  /* ASCII 117 u   */ 0x1c,
  /* ASCII 118 v   */ 0x1c,
  /* ASCII 119 w   */ 0x3e,
  /* ASCII 120 x   */ 0x76,
  /* ASCII 121 y   */ 0x72,
  /* ASCII 122 z   */ 0x5b,
  /* ASCII 123 {   */ 0x46,
  /* ASCII 124 |   */ 0x30,
  /* ASCII 125 }   */ 0x70,
  /* ASCII 126 ~   */ 0x00,
  /* ASCII 127 DEL */ 0x00
} ;
uint8_t demo_length = 96 ;
uint8_t demo_cursor = 0 ;
bool demo_is_ready = false ;

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

  i2c_get_data_mode(i2c_port, &txmode, &rxmode) ;
  logTxDataMode(txmode) ;
}

// Utils -- i2c data mode -- set
void trySetDataMode(int i2c_port) {
  checkTxDataMode(i2c_port);

  ESP_ERROR_CHECK(i2c_set_data_mode(i2c_port, I2C_DATA_MODE_LSB_FIRST, I2C_DATA_MODE_LSB_FIRST));

  checkTxDataMode(i2c_port);
}

// Sample task : display updater
class DisplayUpdaterTask : public Task {
private:
  uint8_t * const buffer = (uint8_t *)malloc(32);
  MessageBuilderForTm1637 messageBuilder = MessageBuilderForTm1637();

public:
  virtual ~DisplayUpdaterTask() {
    if (nullptr != buffer) {
      free(buffer);
    }
  }
  void run(void *data) {
    const TickType_t SLEEP_TIME = 333 / portTICK_PERIOD_MS; // 3.33 Hz
    //prepare constant message
    uint8_t *const command = buffer ;
    messageBuilder.buildMessageForCommandWriteToDisplay(command) ;
    uint8_t *const commandData = command + 1;
    uint8_t *const displayControl = command + command[0] ;
    messageBuilder.buildMessageForControlBrightness(displayControl, Tm1637OpcodeParts::DISPLAY_AND_CONTROL_BRIGHTNESS_7) ;
    uint8_t *const displayControlData = displayControl + 1;
    uint8_t *const addressAndData = displayControl + displayControl[0] ;
    uint8_t *const addressAndDataData = addressAndData + 1 ;
    uint8_t displayData[4] ;

    //i2c port to use
    int i2c_master_port = 0;
    bool ack_mode = true ;

    while (true) {
      if (demo_is_ready) {
        // sample i2c sequence

        // rework here
        displayData[0] = demo_pool[demo_cursor];
        displayData[1] = demo_pool[(demo_cursor+1)%demo_length];
        displayData[2] = demo_pool[(demo_cursor+2)%demo_length];
        displayData[3] = demo_pool[(demo_cursor+3)%demo_length];
        messageBuilder.buildMessageForAddressAndData(addressAndData, displayData, 4);

        // -- sequence of commands
        i2c_cmd_handle_t cmd ;
        cmd = i2c_cmd_link_create();
        i2c_master_start(cmd) ;
        i2c_master_write(cmd,commandData,command[0],ack_mode);
        i2c_master_stop(cmd);
        ESP_ERROR_CHECK(i2c_master_cmd_begin(i2c_master_port, cmd,10));
        i2c_cmd_link_delete(cmd);
        ESP_LOGI(TAG_MAIN, "done i2c command 1");

        cmd = i2c_cmd_link_create();
        i2c_master_start(cmd) ;
        i2c_master_write(cmd,addressAndDataData,addressAndData[0],ack_mode);
        i2c_master_stop(cmd);
        ESP_ERROR_CHECK(i2c_master_cmd_begin(i2c_master_port, cmd,10));
        i2c_cmd_link_delete(cmd);
        ESP_LOGI(TAG_MAIN, "done i2c command 2");

        cmd = i2c_cmd_link_create();
        i2c_master_start(cmd) ;
        i2c_master_write(cmd,displayControlData,displayControl[0],ack_mode);
        i2c_master_stop(cmd);
        ESP_ERROR_CHECK(i2c_master_cmd_begin(i2c_master_port, cmd,10));
        i2c_cmd_link_delete(cmd);
        ESP_LOGI(TAG_MAIN, "done i2c command 3");

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
    .master = {.clk_speed = 250000/*CONFIG_IIC_1_CLK_FREQ_HZ*/},  // select frequency specific to your project
    .clk_flags = 0,                          // optional; you can use I2C_SCLK_SRC_FLAG_* flags to choose i2c source clock here
  };

  i2c_param_config(i2c_master_port, &conf);

  trySetDataMode(i2c_master_port) ;

  ESP_ERROR_CHECK(i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0));

  demo_is_ready = true ;

  displayUpdater = new DisplayUpdaterTask() ;
  displayUpdater->start() ;

} // app_main
