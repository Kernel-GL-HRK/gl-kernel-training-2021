// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

/*
 * 08_irq task gpio_poll
 *
 * Module which uses periodic jiffies-based timer to poll the button state.
 * Module checks button state every 20 ms, it uses a simple debouncing method
 * by comparing current and previous button state.
 * When button is pressed LED1 is ON, and OFF otherwise.
 * Button state change issues a pulse on LED3 output for 1 ms.
 *
 * Author: Eduard Chaika <eduard.chaika@globallogic.com>
 *
 */
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/hrtimer.h>

#include "../gl_gpio/gl_gpio.h"

#ifndef KBUILD_MODNAME
#define KBUILD_MODNAME
#endif

#define LED_PULSE_TIME_NS 1e6
#define LED_ON(led) gl_gpio_write(&led, GPIO_VAL_HIGH)
#define LED_OFF(led) gl_gpio_write(&led, GPIO_VAL_LOW)
#define LED_PULSE() led_pulse()

enum gl_pulse_state {
	PULSE_STATE_ON = 0,
	PULSE_STATE_OFF
};

struct gl_gpio gpio_btn = {
	.group = 2,
	.pin = 8,
	.dir = GPIO_DIR_IN
};

struct gl_gpio gpio_led_btn = {
	.group = 1,
	.pin = 22,
	.dir = GPIO_DIR_OUT
};

struct gl_gpio gpio_led_pulse = {
	.group = 1,
	.pin = 24,
	.dir = GPIO_DIR_OUT
};

enum gl_pulse_state pulse_state;
static struct hrtimer pulse_timer;

enum hrtimer_restart pulse_timer_cb(struct hrtimer *_timer)
{
	enum hrtimer_restart restart = HRTIMER_RESTART;
	ktime_t interval = ktime_set(0, LED_PULSE_TIME_NS);

	hrtimer_forward(_timer, ktime_get(), interval);

	switch (pulse_state) {
	case PULSE_STATE_ON:
		LED_ON(gpio_led_pulse);
		pulse_state++;
	break;
	case PULSE_STATE_OFF:
		LED_OFF(gpio_led_pulse);
		pulse_state++;
	break;
	default:
		restart = HRTIMER_NORESTART;
	break;
	}

	return restart;
}

static void led_pulse(void)
{
	ktime_t interval = ktime_set(0, LED_PULSE_TIME_NS);

	pulse_state = 0;
	hrtimer_init(&pulse_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	pulse_timer.function = &pulse_timer_cb;
	hrtimer_start(&pulse_timer, interval, HRTIMER_MODE_REL);
}

static void gl_button_poll(int data, void *arg)
{
	static int _data = 1;

	if (_data != data) {
		_data = data;
		LED_PULSE();
	}

	if (data == 0)
		LED_ON(gpio_led_btn);
	else
		LED_OFF(gpio_led_btn);
}

static int gl_button_init(void)
{
	int ret;
	const int inverval = 20;

	ret = gl_gpio_init(&gpio_btn);
	if (ret != 0)
		return -1;

	ret = gl_gpio_poll(&gpio_btn, gl_button_poll, NULL, inverval);
	if (ret != 0) {
		gl_gpio_deinit(&gpio_btn);
		return -1;
	}

	return 0;
}

static int gl_led_init(void)
{
	int ret;

	ret = gl_gpio_init(&gpio_led_btn);
	if (ret != 0)
		return -1;

	ret = gl_gpio_init(&gpio_led_pulse);
	if (ret != 0)
		return -1;


	return 0;
}

static void gl_button_deinit(void)
{
	gl_gpio_deinit(&gpio_btn);
}

static void gl_led_deinit(void)
{
	gl_gpio_deinit(&gpio_led_btn);
	gl_gpio_deinit(&gpio_led_pulse);
}

static int __init gl_gpio_poll_init(void)
{
	pr_info("GL Kernel Training 2021\n");

	gl_button_init();
	gl_led_init();

	return 0;
}

static void __exit gl_gpio_poll_exit(void)
{
	pr_info("Exit from 08_irq module\n");
	gl_button_deinit();
	gl_led_deinit();
}

module_init(gl_gpio_poll_init);
module_exit(gl_gpio_poll_exit);

MODULE_AUTHOR("Eduard Chaika <eduard.chaika@globallogic.com>");
MODULE_DESCRIPTION("GL Linux Kernel Training 08_irq");
MODULE_LICENSE("GPL");
