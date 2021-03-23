// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>


MODULE_AUTHOR("Vitalii Irkha <vitalii.o.irkha@globallogic.com");
MODULE_DESCRIPTION("Memory managenent in Linux Kernel Training");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_VERSION("0.1");


#define GPIO_NUMBER(port, bit) (32 * (port) + (bit))

/* On-board LESs
 *  0: D2	GPIO1_21	heartbeat
 *  1: D3	GPIO1_22	uSD access
 *  2: D4	GPIO1_23	active
 *  3: D5	GPIO1_24	eMMC access
 */

#define LED_SD  GPIO_NUMBER(1, 22)
#define LED_MMC GPIO_NUMBER(1, 24)

/* On-board button.
 *
 * LCD/HDMI interface must be disabled.
 */
#define BUTTON  GPIO_NUMBER(2, 8)

#define BUTTON_PERIOD 20 //ms
#define LED_MMC_PERIOD 1 //ms

static struct timer_list button_timer;
static struct hrtimer led_mmc_timer;

enum hrtimer_restart led_mmc_handler(struct hrtimer *t_ptr)
{
	gpio_set_value(LED_MMC, 0);
	return HRTIMER_NORESTART;
}

void timer_button_cb(struct timer_list *t_ptr)
{
	static int prev_state = 1;
	int cur_state = 0;
	ktime_t period = 0;

	cur_state = gpio_get_value(BUTTON);

	if (prev_state == cur_state) {
		if (cur_state)
			gpio_set_value(LED_SD, 0);
		else
			gpio_set_value(LED_SD, 1);
	} else {
		gpio_set_value(LED_MMC, 1);

		period = ktime_set(0, LED_MMC_PERIOD * NSEC_PER_MSEC);
		hrtimer_start(&led_mmc_timer, period, HRTIMER_MODE_REL);
	}

	prev_state = cur_state;
	mod_timer(&button_timer, jiffies + msecs_to_jiffies(BUTTON_PERIOD));
}

static int led_gpio_init(int gpio)
{
	int rc;

	rc = gpio_direction_output(gpio, 0);
	if (rc)
		return rc;

	pr_info("Init LED GPIO%d OK\n", gpio);
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

	pr_info("Init BUTTON GPIO%d OK\n", gpio);
	return 0;

err_input:
	gpio_free(gpio);
err_register:
	return rc;
}

static void button_gpio_deinit(int gpio)
{
	gpio_free(gpio);
	pr_info("Deinit BUTTON GPIO%d\n", gpio);
}

static int __init gpio_poll_init(void)
{
	int rc;

	rc = button_gpio_init(BUTTON);
	if (rc) {
		pr_err("Can't set GPIO%d for button\n", BUTTON);
		goto err_button;
	}

	rc = led_gpio_init(LED_MMC);
	if (rc) {
		pr_err("Can't set GPIO%d for output\n", LED_MMC);
		goto err_led;
	}


	rc = led_gpio_init(LED_SD);
	if (rc) {
		pr_err("Can't set GPIO%d for output\n", LED_SD);
		goto err_led;
	}

	timer_setup(&button_timer, timer_button_cb, 0);
	mod_timer(&button_timer, jiffies + msecs_to_jiffies(BUTTON_PERIOD));

	hrtimer_init(&led_mmc_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	led_mmc_timer.function = &led_mmc_handler;

	pr_info("gpio_poll: --- module loaded! ---\n");

	return 0;

err_led:
	button_gpio_deinit(BUTTON);
err_button:
	return rc;
}

static void __exit gpio_poll_exit(void)
{
	hrtimer_cancel(&led_mmc_timer);
	del_timer(&button_timer);

	gpio_set_value(LED_MMC, 0);
	gpio_set_value(LED_SD, 0);
	pr_info("LED at GPIO OFF\n");

	button_gpio_deinit(BUTTON);

	pr_info("gpio_poll:--- module unloaded! ---\n");
}

module_init(gpio_poll_init);
module_exit(gpio_poll_exit);

