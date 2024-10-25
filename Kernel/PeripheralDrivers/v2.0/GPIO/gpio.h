/*
 * Broadcom BCM2837 GPIO Driver Version 2.0
 *
 * Author: Rafael Sabe
 * Email: rafaelmsabe@gmail.com
 */

#ifndef GPIO_H
#define GPIO_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#define GPIO_PINMODE_INPUT 0U
#define GPIO_PINMODE_OUTPUT 1U
#define GPIO_PINMODE_ALTFUNC0 4U
#define GPIO_PINMODE_ALTFUNC1 5U
#define GPIO_PINMODE_ALTFUNC2 6U
#define GPIO_PINMODE_ALTFUNC3 7U
#define GPIO_PINMODE_ALTFUNC4 3U
#define GPIO_PINMODE_ALTFUNC5 2U

#define GPIO_PUDCTRL_NOPULL 0U
#define GPIO_PUDCTRL_PULLUP 2U
#define GPIO_PUDCTRL_PULLDOWN 1U

//Returns true if gpio_init() has already been succesfully called, false else
bool gpio_is_active(void);

//Must be called before calling any other function
//Initializes the GPIO Interface
//Returns true if successful or already initialized, false else
bool gpio_init(void);

void gpio_reset_pin(uint8_t pin);

//Set/Get the digital level on a GPIO pin (pin must be configured as output to set a level)
void gpio_set_level(uint8_t pin, bool level);
bool gpio_get_level(uint8_t pin);

//Set/Get the pinmode (input, output, or alternative functions (Read datasheet for further description))
void gpio_set_pinmode(uint8_t pin, uint8_t pinmode);
uint8_t gpio_get_pinmode(uint8_t pin);

//Set the Pull Up/Down control
void gpio_set_pudctrl(uint8_t pin, uint8_t pudctrl);

//Check if an event has occurred on a given pin
//Returns true if event occurred, false else
//The pin must have event detection enabled (rising edge detect, low detect, etc...)
bool gpio_event_detected(uint8_t pin);

//Enable rising edge event detection on a specific pin
void gpio_enable_redge_detect(uint8_t pin, bool enable);
bool gpio_redge_detect_is_enabled(uint8_t pin);

//Enable falling edge event detection on a specific pin
void gpio_enable_fedge_detect(uint8_t pin, bool enable);
bool gpio_fedge_detect_is_enabled(uint8_t pin);

//Enable fast (asynchronous) rising edge event detection on a specific pin
void gpio_enable_fast_redge_detect(uint8_t pin, bool enable);
bool gpio_fast_redge_detect_is_enabled(uint8_t pin);

//Enable fast (asynchronous) falling edge event detection on a specific pin
void gpio_enable_fast_fedge_detect(uint8_t pin, bool enable);
bool gpio_fast_fedge_detect_is_enabled(uint8_t pin);

//Enable high level event detection on a specific pin
void gpio_enable_high_detect(uint8_t pin, bool enable);
bool gpio_high_detect_is_enabled(uint8_t pin);

//Enable low level event detection on a specific pin
void gpio_enable_low_detect(uint8_t pin, bool enable);
bool gpio_low_detect_is_enabled(uint8_t pin);

#endif //GPIO_H

