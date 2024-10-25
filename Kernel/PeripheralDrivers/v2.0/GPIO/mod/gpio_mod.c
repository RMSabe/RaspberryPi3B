/*
 * Broadcom BCM2837 GPIO Driver Version 2.0
 *
 * Author: Rafael Sabe
 * Email: rafaelmsabe@gmail.com
 */

#include "bcm2837_gpio_mmap.h"
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <asm/io.h>

#define __GPIO_PINMODE_INPUT 0U
#define __GPIO_PINMODE_OUTPUT 1U
#define __GPIO_PINMODE_ALTFUNC0 4U
#define __GPIO_PINMODE_ALTFUNC1 5U
#define __GPIO_PINMODE_ALTFUNC2 6U
#define __GPIO_PINMODE_ALTFUNC3 7U
#define __GPIO_PINMODE_ALTFUNC4 3U
#define __GPIO_PINMODE_ALTFUNC5 2U

#define __GPIO_PINMODE_MAX 7U

#define __GPIO_PUDCTRL_NOPULL 0U
#define __GPIO_PUDCTRL_PULLUP 2U
#define __GPIO_PUDCTRL_PULLDOWN 1U

#define __GPIO_PUDCTRL_MAX 2U

#define __GPIO_PIN_MAX 53U

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

static struct proc_dir_entry *_gpio_proc = NULL;
static uint32_t *_gpio_mmap = NULL;
static uint8_t _gpio_data_io[__GPIO_DATAIO_SIZE];

static ssize_t _gpio_mod_usrread(struct file *pfile, char __user *usrbuf, size_t size, loff_t *poffset64);
static ssize_t _gpio_mod_usrwrite(struct file *pfile, const char __user *usrbuf, size_t size, loff_t *poffset64);

static const struct proc_ops _gpio_proc_ops = {
	.proc_read = &_gpio_mod_usrread,
	.proc_write = &_gpio_mod_usrwrite
};

static int __init _gpio_mod_enable(void);
static void __exit _gpio_mod_disable(void);

void _gpio_set_level(uint8_t pin, uint8_t level)
{
	size_t regindex32;
	uint8_t bit_offset;

	if(pin > __GPIO_PIN_MAX) return;

	bit_offset = (pin & 0x1f);

	if(pin < 32u)
	{
		if(level) regindex32 = __GPIO_REGINDEX32_OUTPUT0_SET;
		else regindex32 = __GPIO_REGINDEX32_OUTPUT0_CLR;
	}
	else
	{
		if(level) regindex32 = __GPIO_REGINDEX32_OUTPUT1_SET;
		else regindex32 = __GPIO_REGINDEX32_OUTPUT1_CLR;
	}

	_gpio_mmap[regindex32] = (1u << bit_offset);
	return;
}

uint8_t _gpio_get_level(uint8_t pin)
{
	size_t regindex32;
	uint8_t bit_offset;

	if(pin > __GPIO_PIN_MAX) return 0u;

	bit_offset = (pin & 0x1f);

	if(pin < 32u) regindex32 = __GPIO_REGINDEX32_INPUT0;
	else regindex32 = __GPIO_REGINDEX32_INPUT1;

	if(_gpio_mmap[regindex32] & (1u << bit_offset)) return 1u;
	return 0u;
}

void _gpio_set_pinmode(uint8_t pin, uint8_t pinmode)
{
	size_t regindex32;
	uint8_t bit_offset;

	if(pin > __GPIO_PIN_MAX) return;
	if(pinmode > __GPIO_PINMODE_MAX) return;

	bit_offset = 3u*(pin%10u);

	switch(pin/10u)
	{
		case 0u:
			regindex32 = __GPIO_REGINDEX32_FSEL0;
			break;

		case 1u:
			regindex32 = __GPIO_REGINDEX32_FSEL1;
			break;

		case 2u:
			regindex32 = __GPIO_REGINDEX32_FSEL2;
			break;

		case 3u:
			regindex32 = __GPIO_REGINDEX32_FSEL3;
			break;

		case 4u:
			regindex32 = __GPIO_REGINDEX32_FSEL4;
			break;

		case 5u:
			regindex32 = __GPIO_REGINDEX32_FSEL5;
			break;
	}

	_gpio_mmap[regindex32] &= ~(0x7 << bit_offset);
	_gpio_mmap[regindex32] |= (pinmode << bit_offset);
	return;
}

uint8_t _gpio_get_pinmode(uint8_t pin)
{
	size_t regindex32;
	uint8_t bit_offset;

	if(pin > __GPIO_PIN_MAX) return 0u;

	bit_offset = 3u*(pin%10u);

	switch(pin/10u)
	{
		case 0u:
			regindex32 = __GPIO_REGINDEX32_FSEL0;
			break;

		case 1u:
			regindex32 = __GPIO_REGINDEX32_FSEL1;
			break;

		case 2u:
			regindex32 = __GPIO_REGINDEX32_FSEL2;
			break;

		case 3u:
			regindex32 = __GPIO_REGINDEX32_FSEL3;
			break;

		case 4u:
			regindex32 = __GPIO_REGINDEX32_FSEL4;
			break;

		case 5u:
			regindex32 = __GPIO_REGINDEX32_FSEL5;
			break;
	}

	return ((_gpio_mmap[regindex32] >> bit_offset) & 0x7);
}

void _gpio_set_pudctrl(uint8_t pin, uint8_t pudctrl)
{
	size_t regindex32;
	uint8_t bit_offset;

	if(pin > __GPIO_PIN_MAX) return;
	if(pudctrl > __GPIO_PUDCTRL_MAX) return;

	_gpio_mmap[__GPIO_REGINDEX32_PUDCTRL_ENABLE] = pudctrl;

	bit_offset = (pin & 0x1f);

	if(pin < 32u) regindex32 = __GPIO_REGINDEX32_PUDCTRL0;
	else regindex32 = __GPIO_REGINDEX32_PUDCTRL1;

	_gpio_mmap[regindex32] = (1u << bit_offset);
	return;
}

uint8_t _gpio_event_detected(uint8_t pin)
{
	size_t regindex32;
	uint8_t bit_offset;

	if(pin > __GPIO_PIN_MAX) return 0u;

	bit_offset = (pin & 0x1f);

	if(pin < 32u) regindex32 = __GPIO_REGINDEX32_EVENTDETECT0_STATUS;
	else regindex32 = __GPIO_REGINDEX32_EVENTDETECT1_STATUS;

	if(_gpio_mmap[regindex32] & (1u << bit_offset))
	{
		_gpio_mmap[regindex32] |= (1u << bit_offset);
		return 1u;
	}

	return 0u;
}

void _gpio_enable_redge_detect(uint8_t pin, uint8_t enable)
{
	size_t regindex32;
	uint8_t bit_offset;

	if(pin > __GPIO_PIN_MAX) return;

	bit_offset = (pin & 0x1f);

	if(pin < 32u) regindex32 = __GPIO_REGINDEX32_REDGEDETECT0_ENABLE;
	else regindex32 = __GPIO_REGINDEX32_REDGEDETECT1_ENABLE;

	if(enable) _gpio_mmap[regindex32] |= (1u << bit_offset);
	else _gpio_mmap[regindex32] &= ~(1u << bit_offset);
	return;
}

uint8_t _gpio_redge_detect_is_enabled(uint8_t pin)
{
	size_t regindex32;
	uint8_t bit_offset;

	if(pin > __GPIO_PIN_MAX) return 0u;

	bit_offset = (pin & 0x1f);

	if(pin < 32u) regindex32 = __GPIO_REGINDEX32_REDGEDETECT0_ENABLE;
	else regindex32 = __GPIO_REGINDEX32_REDGEDETECT1_ENABLE;

	if(_gpio_mmap[regindex32] & (1u << bit_offset)) return 1u;

	return 0u;
}

void _gpio_enable_fedge_detect(uint8_t pin, uint8_t enable)
{
	size_t regindex32;
	uint8_t bit_offset;

	if(pin > __GPIO_PIN_MAX) return;

	bit_offset = (pin & 0x1f);

	if(pin < 32u) regindex32 = __GPIO_REGINDEX32_FEDGEDETECT0_ENABLE;
	else regindex32 = __GPIO_REGINDEX32_FEDGEDETECT1_ENABLE;

	if(enable) _gpio_mmap[regindex32] |= (1u << bit_offset);
	else _gpio_mmap[regindex32] &= ~(1u << bit_offset);
	return;
}

uint8_t _gpio_fedge_detect_is_enabled(uint8_t pin)
{
	size_t regindex32;
	uint8_t bit_offset;

	if(pin > __GPIO_PIN_MAX) return 0u;

	bit_offset = (pin & 0x1f);

	if(pin < 32u) regindex32 = __GPIO_REGINDEX32_FEDGEDETECT0_ENABLE;
	else regindex32 = __GPIO_REGINDEX32_FEDGEDETECT1_ENABLE;

	if(_gpio_mmap[regindex32] & (1u << bit_offset)) return 1u;

	return 0u;
}

void _gpio_enable_async_redge_detect(uint8_t pin, uint8_t enable)
{
	size_t regindex32;
	uint8_t bit_offset;

	if(pin > __GPIO_PIN_MAX) return;

	bit_offset = (pin & 0x1f);

	if(pin < 32u) regindex32 = __GPIO_REGINDEX32_ASYNC_REDGEDETECT0_ENABLE;
	else regindex32 = __GPIO_REGINDEX32_ASYNC_REDGEDETECT1_ENABLE;

	if(enable) _gpio_mmap[regindex32] |= (1u << bit_offset);
	else _gpio_mmap[regindex32] &= ~(1u << bit_offset);
	return;
}

uint8_t _gpio_async_redge_detect_is_enabled(uint8_t pin)
{
	size_t regindex32;
	uint8_t bit_offset;

	if(pin > __GPIO_PIN_MAX) return 0u;

	bit_offset = (pin & 0x1f);

	if(pin < 32u) regindex32 = __GPIO_REGINDEX32_ASYNC_REDGEDETECT0_ENABLE;
	else regindex32 = __GPIO_REGINDEX32_ASYNC_REDGEDETECT1_ENABLE;

	if(_gpio_mmap[regindex32] & (1u << bit_offset)) return 1u;

	return 0u;
}

void _gpio_enable_async_fedge_detect(uint8_t pin, uint8_t enable)
{
	size_t regindex32;
	uint8_t bit_offset;

	if(pin > __GPIO_PIN_MAX) return;

	bit_offset = (pin & 0x1f);

	if(pin < 32u) regindex32 = __GPIO_REGINDEX32_ASYNC_FEDGEDETECT0_ENABLE;
	else regindex32 = __GPIO_REGINDEX32_ASYNC_FEDGEDETECT1_ENABLE;

	if(enable) _gpio_mmap[regindex32] |= (1u << bit_offset);
	else _gpio_mmap[regindex32] &= ~(1u << bit_offset);
	return;
}

uint8_t _gpio_async_fedge_detect_is_enabled(uint8_t pin)
{
	size_t regindex32;
	uint8_t bit_offset;

	if(pin > __GPIO_PIN_MAX) return 0u;

	bit_offset = (pin & 0x1f);

	if(pin < 32u) regindex32 = __GPIO_REGINDEX32_ASYNC_FEDGEDETECT0_ENABLE;
	else regindex32 = __GPIO_REGINDEX32_ASYNC_FEDGEDETECT1_ENABLE;

	if(_gpio_mmap[regindex32] & (1u << bit_offset)) return 1u;

	return 0u;
}

void _gpio_enable_high_detect(uint8_t pin, uint8_t enable)
{
	size_t regindex32;
	uint8_t bit_offset;

	if(pin > __GPIO_PIN_MAX) return;

	bit_offset = (pin & 0x1f);

	if(pin < 32u) regindex32 = __GPIO_REGINDEX32_HIGHDETECT0_ENABLE;
	else regindex32 = __GPIO_REGINDEX32_HIGHDETECT1_ENABLE;

	if(enable) _gpio_mmap[regindex32] |= (1u << bit_offset);
	else _gpio_mmap[regindex32] &= ~(1u << bit_offset);
	return;
}

uint8_t _gpio_high_detect_is_enabled(uint8_t pin)
{
	size_t regindex32;
	uint8_t bit_offset;

	if(pin > __GPIO_PIN_MAX) return 0u;

	bit_offset = (pin & 0x1f);

	if(pin < 32u) regindex32 = __GPIO_REGINDEX32_HIGHDETECT0_ENABLE;
	else regindex32 = __GPIO_REGINDEX32_HIGHDETECT1_ENABLE;

	if(_gpio_mmap[regindex32] & (1u << bit_offset)) return 1u;

	return 0u;
}

void _gpio_enable_low_detect(uint8_t pin, uint8_t enable)
{
	size_t regindex32;
	uint8_t bit_offset;

	if(pin > __GPIO_PIN_MAX) return;

	bit_offset = (pin & 0x1f);

	if(pin < 32u) regindex32 = __GPIO_REGINDEX32_LOWDETECT0_ENABLE;
	else regindex32 = __GPIO_REGINDEX32_LOWDETECT1_ENABLE;

	if(enable) _gpio_mmap[regindex32] |= (1u << bit_offset);
	else _gpio_mmap[regindex32] &= ~(1u << bit_offset);
	return;
}

uint8_t _gpio_low_detect_is_enabled(uint8_t pin)
{
	size_t regindex32;
	uint8_t bit_offset;

	if(pin > __GPIO_PIN_MAX) return 0u;

	bit_offset = (pin & 0x1f);

	if(pin < 32u) regindex32 = __GPIO_REGINDEX32_LOWDETECT0_ENABLE;
	else regindex32 = __GPIO_REGINDEX32_LOWDETECT1_ENABLE;

	if(_gpio_mmap[regindex32] & (1u << bit_offset)) return 1u;

	return 0u;
}

void _gpio_reset_pin(uint8_t pin)
{
	if(pin > __GPIO_PIN_MAX) return;

	_gpio_enable_high_detect(pin, 0u);
	_gpio_enable_low_detect(pin, 0u);
	_gpio_enable_redge_detect(pin, 0u);
	_gpio_enable_fedge_detect(pin, 0u);
	_gpio_enable_async_redge_detect(pin, 0u);
	_gpio_enable_async_fedge_detect(pin, 0u);

	_gpio_set_pudctrl(pin, __GPIO_PUDCTRL_NOPULL);
	_gpio_set_pinmode(pin, __GPIO_PINMODE_INPUT);

	_gpio_event_detected(pin);
	return;
}

static ssize_t _gpio_mod_usrread(struct file *pfile, char __user *usrbuf, size_t size, loff_t *poffset64)
{
	ssize_t n_ret = copy_to_user(usrbuf, _gpio_data_io, __GPIO_DATAIO_SIZE);
	return n_ret;
}

static ssize_t _gpio_mod_usrwrite(struct file *pfile, const char __user *usrbuf, size_t size, loff_t *poffset64)
{
	ssize_t n_ret = copy_from_user(_gpio_data_io, usrbuf, __GPIO_DATAIO_SIZE);

	switch(_gpio_data_io[0])
	{
		case __GPIO_CMD_RESET_PIN:
			_gpio_reset_pin(_gpio_data_io[1]);
			break;

		case __GPIO_CMD_SET_LEVEL:
			_gpio_set_level(_gpio_data_io[1], _gpio_data_io[2]);
			break;

		case __GPIO_CMD_GET_LEVEL:
			_gpio_data_io[2] = _gpio_get_level(_gpio_data_io[1]);
			break;

		case __GPIO_CMD_SET_PINMODE:
			_gpio_set_pinmode(_gpio_data_io[1], _gpio_data_io[2]);
			break;

		case __GPIO_CMD_GET_PINMODE:
			_gpio_data_io[2] = _gpio_get_pinmode(_gpio_data_io[1]);
			break;

		case __GPIO_CMD_SET_PUDCTRL:
			_gpio_set_pudctrl(_gpio_data_io[1], _gpio_data_io[2]);
			break;

		case __GPIO_CMD_GET_EVENTDETECTED:
			_gpio_data_io[2] = _gpio_event_detected(_gpio_data_io[1]);
			break;

		case __GPIO_CMD_SET_ENABLE_REDGEDETECT:
			_gpio_enable_redge_detect(_gpio_data_io[1], _gpio_data_io[2]);
			break;

		case __GPIO_CMD_GET_ENABLE_REDGEDETECT:
			_gpio_data_io[2] = _gpio_redge_detect_is_enabled(_gpio_data_io[1]);
			break;

		case __GPIO_CMD_SET_ENABLE_FEDGEDETECT:
			_gpio_enable_fedge_detect(_gpio_data_io[1], _gpio_data_io[2]);
			break;

		case __GPIO_CMD_GET_ENABLE_FEDGEDETECT:
			_gpio_data_io[2] = _gpio_fedge_detect_is_enabled(_gpio_data_io[1]);
			break;

		case __GPIO_CMD_SET_ENABLE_ASYNC_REDGEDETECT:
			_gpio_enable_async_redge_detect(_gpio_data_io[1], _gpio_data_io[2]);
			break;

		case __GPIO_CMD_GET_ENABLE_ASYNC_REDGEDETECT:
			_gpio_data_io[2] = _gpio_async_redge_detect_is_enabled(_gpio_data_io[1]);
			break;

		case __GPIO_CMD_SET_ENABLE_ASYNC_FEDGEDETECT:
			_gpio_enable_async_fedge_detect(_gpio_data_io[1], _gpio_data_io[2]);
			break;

		case __GPIO_CMD_GET_ENABLE_ASYNC_FEDGEDETECT:
			_gpio_data_io[2] = _gpio_async_fedge_detect_is_enabled(_gpio_data_io[1]);
			break;

		case __GPIO_CMD_SET_ENABLE_HIGHDETECT:
			_gpio_enable_high_detect(_gpio_data_io[1], _gpio_data_io[2]);
			break;

		case __GPIO_CMD_GET_ENABLE_HIGHDETECT:
			_gpio_data_io[2] = _gpio_high_detect_is_enabled(_gpio_data_io[1]);
			break;

		case __GPIO_CMD_SET_ENABLE_LOWDETECT:
			_gpio_enable_low_detect(_gpio_data_io[1], _gpio_data_io[2]);
			break;

		case __GPIO_CMD_GET_ENABLE_LOWDETECT:
			_gpio_data_io[2] = _gpio_low_detect_is_enabled(_gpio_data_io[1]);
			break;
	}

	_gpio_data_io[0] = __GPIO_CMD_KERNEL_RESPONSE;
	return n_ret;
}

static int __init _gpio_mod_enable(void)
{
	_gpio_mmap = (uint32_t*) ioremap(__GPIO_BASE_ADDR, __GPIO_MMAP_SIZE);
	if(_gpio_mmap == NULL)
	{
		printk("GPIO: Error: GPIO mapping failed");
		return -1;
	}

	_gpio_proc = proc_create("gpioctrl", 0x1b6, NULL, &_gpio_proc_ops);
	if(_gpio_proc == NULL)
	{
		iounmap(_gpio_mmap);
		_gpio_mmap = NULL;

		printk("GPIO: Error: GPIO proc file creation failed");
		return -1;
	}

	printk("GPIO: Module enabled");
	return 0;
}

static void __exit _gpio_mod_disable(void)
{
	if(_gpio_mmap != NULL)
	{
		iounmap(_gpio_mmap);
		_gpio_mmap = NULL;
	}

	if(_gpio_proc != NULL)
	{
		proc_remove(_gpio_proc);
		_gpio_proc = NULL;
	}

	printk("GPIO: Module disabled");
	return;
}

module_init(_gpio_mod_enable);
module_exit(_gpio_mod_disable);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rafael Sabe");
MODULE_DESCRIPTION("GPIO Driver (Version 2.0) for Broadcom BCM2837");

