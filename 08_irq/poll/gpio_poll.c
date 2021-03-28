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

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Anton Kotsiubailo");
MODULE_DESCRIPTION("A Button/LED test driver for the BBB");
MODULE_VERSION("0.1");

#define NSEC_IN_USEC 1000
#define HIGH		true
#define LOW		false
#define BUTTON_PERIOD 20 //ms
#define BLINK_PERIOD 1000 //us

struct timer_list button_timer;
struct hrtimer blink_timer;
static unsigned int gpioLED1 = 11;
static unsigned int gpioLED2 = 19;
static unsigned int gpioButton = 21;
ktime_t ktime;

static int led_gpio = -1;
static int button_gpio = -1;

enum hrtimer_restart hrtimer_function(struct hrtimer *data)
{
	gpio_set_value(gpioLED2, LOW);

	return HRTIMER_NORESTART;
}

void timer_function(unsigned long data)
{
	static bool old_state;
	bool cur_state = gpio_get_value(gpioButton);
	ktime = ktime_set(0, BLINK_PERIOD * NSEC_IN_USEC);

	if (old_state == cur_state) {
		if (cur_state)
			gpio_set_value(gpioLED1, LOW);
		else
			gpio_set_value(gpioLED1, HIGH);
	} else {
		ktime = ktime_set(0, BLINK_PERIOD * NSEC_IN_USEC);
		gpio_set_value(gpioLED1, HIGH);
		hrtimer_start(&blink_timer, ktime, HRTIMER_MODE_REL);
		old_state = cur_state;
		}

	mod_timer(&button_timer, jiffies + msecs_to_jiffies(BUTTON_PERIOD));
}


static int led_gpio_init(int gpio)
{
	int rc;
	rc = gpio_direction_output(gpio, 0);
	if (rc)
		return rc;

	led_gpio = gpio;
	return 0;
}

static int button_gpio_init(int gpio)
{
	int rc;

	rc = gpio_request(gpio, "Onboard user button");
	if (rc)
		goto err_register;

	rc = gpio_direction_input(gpio);
	if (rc)
		goto err_input;

	button_gpio = gpio;
	pr_info("Init GPIO%d OK\n", button_gpio);
	return 0;

err_input:
	gpio_free(gpio);
err_register:
	return rc;
}

int __init mod_init(void)
{
	int res;

	res = led_gpio_init(gpioLED1);
	if (res != 0) {
		pr_err("LED1 was not initialised\n");
		return -EINVAL;
	}

	res = led_gpio_init(gpioLED2);
	if (res != 0) {
		pr_err("LED2 was not initialised\n");
		return -EINVAL;
	}

	res = button_gpio_init(gpioButton);
	if (res != 0) {
		pr_err("Button was not initialised\n");
		return -EINVAL;
	}
	hrtimer_init(&blink_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	blink_timer.function = &hrtimer_function;

	timer_setup(&button_timer, timer_function, 0);
	mod_timer(&button_timer, jiffies + msecs_to_jiffies(BUTTON_PERIOD));

	add_timer(&button_timer);

	return res;
}

void __exit mod_cleanup(void)
{
	gpio_set_value(gpioLED1, LOW);

	hrtimer_cancel(&blink_timer);
	del_timer(&button_timer);

	gpio_free(gpioLED1);
	gpio_free(gpioLED2);
	gpio_free(gpioButton);

	pr_info("'gpio_poll' module released\n");
}

module_init(mod_init);
module_exit(mod_cleanup);