// SPDX-License-Identifier: MIT and GPL
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <linux/jiffies.h>
#include <linux/hrtimer.h>

MODULE_DESCRIPTION("Basic module demo: working with gpio");
MODULE_AUTHOR("AlexShlikhta oleksandr.shlikhta@globallogic.com");
MODULE_VERSION("0.1");
MODULE_LICENSE("Dual MIT/GPL"); // this affects the kernel behavior

#define LLED_PIN 60
#define LLED_LABEL "LLED"
#define SLED_PIN 44
#define SLED_LABEL "SLED"
#define BUTTON_PIN 7
#define BUTTON_LABEL "BUTTON"

#define LOW false
#define HIGH true

#define NSEC_IN_USEC 1000

#define BUTTON_PERIOD 20 //ms
#define BLINK_PERIOD 1000 //us

struct timer_list button_timer;
struct hrtimer blink_timer;
static bool led_state;
static bool blink_en;
static ktime_t period;

enum hrtimer_restart blink_foo(struct hrtimer *data)
{
	gpio_set_value(SLED_PIN, LOW);

	return HRTIMER_NORESTART;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void button_foo(struct timer_list *data)
{
	static bool old_state;
	bool cur_state = gpio_get_value(BUTTON_PIN);

	if (old_state == cur_state) {
		if (cur_state)
			gpio_set_value(LLED_PIN, LOW);
		else
			gpio_set_value(LLED_PIN, HIGH);
	} else {
		period = ktime_set(0, BLINK_PERIOD * NSEC_IN_USEC);
		gpio_set_value(SLED_PIN, HIGH);
		hrtimer_start(&blink_timer, period, HRTIMER_MODE_REL);
		old_state = cur_state;
	}

	mod_timer(&button_timer, jiffies + msecs_to_jiffies(BUTTON_PERIOD));
}

///////////////////////////////////////////////////////////////////////////////////////////////////

enum dir_t { INPUT = 0, OUTPUT };

int gpio_init(uint16_t pin, const char *label, enum dir_t dir)
{
	int res;

	if (!gpio_is_valid(pin))
		return -EINVAL;

	if (dir == INPUT)
		res = gpio_request_one(pin, GPIOF_IN, label);
	else
		res = gpio_request_one(pin, GPIOF_OUT_INIT_LOW, label);

	if (res < 0)
		return res;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
//###########################INIT CALLBACK#####################################
///////////////////////////////////////////////////////////////////////////////

int __init mod_init(void)
{
	int state = 0;

	led_state = LOW;
	blink_en = false;
	period = 0;

	state = gpio_init(LLED_PIN, LLED_LABEL, OUTPUT);
	if (state < 0) {
		pr_err("gpio '%s' init failed:(\n", LLED_LABEL);
		return state;
	}

	state = gpio_init(SLED_PIN, SLED_LABEL, OUTPUT);
	if (state < 0) {
		pr_err("gpio '%s' init failed:(\n", SLED_LABEL);
		return state;
	}

	state = gpio_init(BUTTON_PIN, BUTTON_LABEL, INPUT);
	if (state < 0) {
		pr_err("gpio '%s' init failed:(\n", BUTTON_LABEL);
		return state;
	}

	timer_setup(&button_timer, button_foo, 0);
	mod_timer(&button_timer, jiffies + msecs_to_jiffies(BUTTON_PERIOD));

	hrtimer_init(&blink_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	blink_timer.function = &blink_foo;

	pr_info("'gpio_poll' module initialized\n");

	return state;
}

///////////////////////////////////////////////////////////////////////////////
//###########################EXIT CALLBACK#####################################
///////////////////////////////////////////////////////////////////////////////

void mod_cleanup(void)
{
	gpio_set_value(LLED_PIN, LOW);

	hrtimer_cancel(&blink_timer);
	del_timer(&button_timer);

	gpio_free(LLED_PIN);
	gpio_free(SLED_PIN);
	gpio_free(BUTTON_PIN);

	pr_info("'gpio_poll' module released\n");
}

module_init(mod_init);
module_exit(mod_cleanup);
