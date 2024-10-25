/*
 * GPIO Driver Test 1: Blink
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include "gpio.h"

#define TEST_PIN 12U
#define DELAYTIME_US 200000UL

void loop(void);
void delay_us(unsigned long time_us);

void main(void)
{
	if(!gpio_init())
	{
		printf("GPIO INIT ERROR\n");
		return;
	}

	gpio_reset_pin(TEST_PIN);
	gpio_set_pinmode(TEST_PIN, GPIO_PINMODE_OUTPUT);

	printf("Running...\n");
	loop();

	return;
}

void loop(void)
{
	while(true)
	{
		gpio_set_level(TEST_PIN, true);
		delay_us(DELAYTIME_US);
		gpio_set_level(TEST_PIN, false);
		delay_us(DELAYTIME_US);
	}

	return;
}

void delay_us(unsigned long time_us)
{
	clock_t start_time = clock();
	while(clock() < (start_time + time_us));
	return;
}

