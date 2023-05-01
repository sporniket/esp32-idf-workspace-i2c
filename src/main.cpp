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

#include "driver/gpio.h"
#include "driver/gptimer.h"

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

} // app_main
