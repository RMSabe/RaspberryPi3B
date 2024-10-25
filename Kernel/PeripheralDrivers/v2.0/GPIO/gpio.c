/*
 * Broadcom BCM2837 GPIO Driver Version 2.0
 *
 * Author: Rafael Sabe
 * Email: rafaelmsabe@gmail.com
 */

#include "gpio.h"
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define __GPIO_PROC_FILE_DIR ("/proc/gpioctrl")

#define __GPIO_PIN_MAX 53U
#define __GPIO_PINMODE_MAX 7U
#define __GPIO_PUDCTRL_MAX 2U

#define __GPIO_DATAIO_SIZE 3UL

#define __GPIO_CMD_RESET_PIN 0U
#define __GPIO_CMD_SET_LEVEL 1U
#define __GPIO_CMD_GET_LEVEL 2U
#define __GPIO_CMD_SET_PINMODE 3U
#define __GPIO_CMD_GET_PINMODE 4U
#define __GPIO_CMD_SET_PUDCTRL 5U
#define __GPIO_CMD_GET_EVENTDETECTED 6U
#define __GPIO_CMD_SET_ENABLE_REDGEDETECT 7U
#define __GPIO_CMD_GET_ENABLE_REDGEDETECT 8U
#define __GPIO_CMD_SET_ENABLE_FEDGEDETECT 9U
#define __GPIO_CMD_GET_ENABLE_FEDGEDETECT 10U
#define __GPIO_CMD_SET_ENABLE_ASYNC_REDGEDETECT 11U
#define __GPIO_CMD_GET_ENABLE_ASYNC_REDGEDETECT 12U
#define __GPIO_CMD_SET_ENABLE_ASYNC_FEDGEDETECT 13U
#define __GPIO_CMD_GET_ENABLE_ASYNC_FEDGEDETECT 14U
#define __GPIO_CMD_SET_ENABLE_HIGHDETECT 15U
#define __GPIO_CMD_GET_ENABLE_HIGHDETECT 16U
#define __GPIO_CMD_SET_ENABLE_LOWDETECT 17U
#define __GPIO_CMD_GET_ENABLE_LOWDETECT 18U

#define __GPIO_CMD_KERNEL_RESPONSE 0xff

int _gpio_proc_fd = -1;
uint8_t _gpio_data_io[__GPIO_DATAIO_SIZE];

bool gpio_is_active(void)
{
	return (_gpio_proc_fd >= 0);
}

bool gpio_init(void)
{
	if(gpio_is_active()) return true;

	_gpio_proc_fd = open(__GPIO_PROC_FILE_DIR, O_RDWR);
	return (_gpio_proc_fd >= 0);
}

void _gpio_call_kernel(void)
{
	if(_gpio_proc_fd < 0) return;

	write(_gpio_proc_fd, _gpio_data_io, __GPIO_DATAIO_SIZE);

	do{
		read(_gpio_proc_fd, _gpio_data_io, __GPIO_DATAIO_SIZE);
	}while(_gpio_data_io[0] != __GPIO_CMD_KERNEL_RESPONSE);

	return;
}

void gpio_reset_pin(uint8_t pin)
{
	if(pin > __GPIO_PIN_MAX) return;

	_gpio_data_io[0] = __GPIO_CMD_RESET_PIN;
	_gpio_data_io[1] = pin;

	_gpio_call_kernel();
	return;
}

void gpio_set_level(uint8_t pin, bool level)
{
	if(pin > __GPIO_PIN_MAX) return;

	_gpio_data_io[0] = __GPIO_CMD_SET_LEVEL;
	_gpio_data_io[1] = pin;
	_gpio_data_io[2] = (uint8_t) level;

	_gpio_call_kernel();
	return;
}

bool gpio_get_level(uint8_t pin)
{
	if(pin > __GPIO_PIN_MAX) return false;

	_gpio_data_io[0] = __GPIO_CMD_GET_LEVEL;
	_gpio_data_io[1] = pin;

	_gpio_call_kernel();
	return (bool) _gpio_data_io[2];
}

void gpio_set_pinmode(uint8_t pin, uint8_t pinmode)
{
	if(pin > __GPIO_PIN_MAX) return;
	if(pinmode > __GPIO_PINMODE_MAX) return;

	_gpio_data_io[0] = __GPIO_CMD_SET_PINMODE;
	_gpio_data_io[1] = pin;
	_gpio_data_io[2] = pinmode;

	_gpio_call_kernel();
	return;
}

uint8_t gpio_get_pinmode(uint8_t pin)
{
	if(pin > __GPIO_PIN_MAX) return 0u;

	_gpio_data_io[0] = __GPIO_CMD_GET_PINMODE;
	_gpio_data_io[1] = pin;

	_gpio_call_kernel();
	return (_gpio_data_io[2] & 0x7);
}

void gpio_set_pudctrl(uint8_t pin, uint8_t pudctrl)
{
	if(pin > __GPIO_PIN_MAX) return;
	if(pudctrl > __GPIO_PUDCTRL_MAX) return;

	_gpio_data_io[0] = __GPIO_CMD_SET_PUDCTRL;
	_gpio_data_io[1] = pin;
	_gpio_data_io[2] = pudctrl;

	_gpio_call_kernel();
	return;
}

bool gpio_event_detected(uint8_t pin)
{
	if(pin > __GPIO_PIN_MAX) return false;

	_gpio_data_io[0] = __GPIO_CMD_GET_EVENTDETECTED;
	_gpio_data_io[1] = pin;

	_gpio_call_kernel();
	return (bool) _gpio_data_io[2];
}

void gpio_enable_redge_detect(uint8_t pin, bool enable)
{
	if(pin > __GPIO_PIN_MAX) return;

	_gpio_data_io[0] = __GPIO_CMD_SET_ENABLE_REDGEDETECT;
	_gpio_data_io[1] = pin;
	_gpio_data_io[2] = (uint8_t) enable;

	_gpio_call_kernel();
	return;
}

bool gpio_redge_detect_is_enabled(uint8_t pin)
{
	if(pin > __GPIO_PIN_MAX) return false;

	_gpio_data_io[0] = __GPIO_CMD_GET_ENABLE_REDGEDETECT;
	_gpio_data_io[1] = pin;

	_gpio_call_kernel();
	return (bool) _gpio_data_io[2];
}

void gpio_enable_fedge_detect(uint8_t pin, bool enable)
{
	if(pin > __GPIO_PIN_MAX) return;

	_gpio_data_io[0] = __GPIO_CMD_SET_ENABLE_FEDGEDETECT;
	_gpio_data_io[1] = pin;
	_gpio_data_io[2] = (uint8_t) enable;

	_gpio_call_kernel();
	return;
}

bool gpio_fedge_detect_is_enabled(uint8_t pin)
{
	if(pin > __GPIO_PIN_MAX) return false;

	_gpio_data_io[0] = __GPIO_CMD_GET_ENABLE_FEDGEDETECT;
	_gpio_data_io[1] = pin;

	_gpio_call_kernel();
	return (bool) _gpio_data_io[2];
}

void gpio_enable_fast_redge_detect(uint8_t pin, bool enable)
{
	if(pin > __GPIO_PIN_MAX) return;

	_gpio_data_io[0] = __GPIO_CMD_SET_ENABLE_ASYNC_REDGEDETECT;
	_gpio_data_io[1] = pin;
	_gpio_data_io[2] = (uint8_t) enable;

	_gpio_call_kernel();
	return;
}

bool gpio_fast_redge_detect_is_enabled(uint8_t pin)
{
	if(pin > __GPIO_PIN_MAX) return false;

	_gpio_data_io[0] = __GPIO_CMD_GET_ENABLE_ASYNC_REDGEDETECT;
	_gpio_data_io[1] = pin;

	_gpio_call_kernel();
	return (bool) _gpio_data_io[2];
}

void gpio_enable_fast_fedge_detect(uint8_t pin, bool enable)
{
	if(pin > __GPIO_PIN_MAX) return;

	_gpio_data_io[0] = __GPIO_CMD_SET_ENABLE_ASYNC_FEDGEDETECT;
	_gpio_data_io[1] = pin;
	_gpio_data_io[2] = (uint8_t) enable;

	_gpio_call_kernel();
	return;
}

bool gpio_fast_fedge_detect_is_enabled(uint8_t pin)
{
	if(pin > __GPIO_PIN_MAX) return false;

	_gpio_data_io[0] = __GPIO_CMD_GET_ENABLE_ASYNC_FEDGEDETECT;
	_gpio_data_io[1] = pin;

	_gpio_call_kernel();
	return (bool) _gpio_data_io[2];
}

void gpio_enable_high_detect(uint8_t pin, bool enable)
{
	if(pin > __GPIO_PIN_MAX) return;

	_gpio_data_io[0] = __GPIO_CMD_SET_ENABLE_HIGHDETECT;
	_gpio_data_io[1] = pin;
	_gpio_data_io[2] = (uint8_t) enable;

	_gpio_call_kernel();
	return;
}

bool gpio_high_detect_is_enabled(uint8_t pin)
{
	if(pin > __GPIO_PIN_MAX) return false;

	_gpio_data_io[0] = __GPIO_CMD_GET_ENABLE_HIGHDETECT;
	_gpio_data_io[1] = pin;

	_gpio_call_kernel();
	return (bool) _gpio_data_io[2];
}

void gpio_enable_low_detect(uint8_t pin, bool enable)
{
	if(pin > __GPIO_PIN_MAX) return;

	_gpio_data_io[0] = __GPIO_CMD_SET_ENABLE_LOWDETECT;
	_gpio_data_io[1] = pin;
	_gpio_data_io[2] = (uint8_t) enable;

	_gpio_call_kernel();
	return;
}

bool gpio_low_detect_is_enabled(uint8_t pin)
{
	if(pin > __GPIO_PIN_MAX) return false;

	_gpio_data_io[0] = __GPIO_CMD_GET_ENABLE_LOWDETECT;
	_gpio_data_io[1] = pin;

	_gpio_call_kernel();
	return (bool) _gpio_data_io[2];
}

