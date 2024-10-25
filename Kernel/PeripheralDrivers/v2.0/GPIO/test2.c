/*
 * GPIO Driver Test 2: Basic I/O
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include "gpio.h"

#define LED0_PIN 12U
#define LED1_PIN 13U
#define LED2_PIN 16U
#define LED3_PIN 17U

#define BUTTON0_PIN 5U
#define BUTTON1_PIN 6U

uint8_t b = 0u;

void loop(void);
void led_update(void);
void delay_us(unsigned long time_us);

void main(void)
{
	if(!gpio_init())
	{
		printf("GPIO INIT ERROR\n");
		return;
	}

	gpio_reset_pin(LED0_PIN);
	gpio_reset_pin(LED1_PIN);
	gpio_reset_pin(LED2_PIN);
	gpio_reset_pin(LED3_PIN);
	gpio_reset_pin(BUTTON0_PIN);
	gpio_reset_pin(BUTTON1_PIN);

	gpio_set_pinmode(LED0_PIN, GPIO_PINMODE_OUTPUT);
	gpio_set_pinmode(LED1_PIN, GPIO_PINMODE_OUTPUT);
	gpio_set_pinmode(LED2_PIN, GPIO_PINMODE_OUTPUT);
	gpio_set_pinmode(LED3_PIN, GPIO_PINMODE_OUTPUT);
	gpio_set_pinmode(BUTTON0_PIN, GPIO_PINMODE_INPUT);
	gpio_set_pinmode(BUTTON1_PIN, GPIO_PINMODE_INPUT);

	gpio_set_pudctrl(BUTTON0_PIN, GPIO_PUDCTRL_PULLUP);
	gpio_set_pudctrl(BUTTON1_PIN, GPIO_PUDCTRL_PULLUP);

	printf("Running...\n");
	loop();

	return;
}

void loop(void)
{
	while(true)
	{
		if(!gpio_get_level(BUTTON0_PIN))
		{
			b--;
			led_update();

			while(!gpio_get_level(BUTTON0_PIN)) delay_us(1024ul);
		}

		if(!gpio_get_level(BUTTON1_PIN))
		{
			b++;
			led_update();

			while(!gpio_get_level(BUTTON1_PIN)) delay_us(1024ul);
		}

		delay_us(16384ul);
	}

	return;
}

void led_update(void)
{
	gpio_set_level(LED3_PIN, (b & 0x8));
	gpio_set_level(LED2_PIN, (b & 0x4));
	gpio_set_level(LED1_PIN, (b & 0x2));
	gpio_set_level(LED0_PIN, (b & 0x1));
	return;
}

void delay_us(unsigned long time_us)
{
	clock_t start_time = clock();
	while(clock() < (start_time + time_us));
	return;
}

