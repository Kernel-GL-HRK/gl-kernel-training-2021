// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/printk.h>

#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>


MODULE_AUTHOR("Oleh Yakymenko <oleh.o.yakymenko@globallogic.com");
MODULE_DESCRIPTION("GPIO Poll");
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

#define MS_TO_NS(t) ((unsigned long)(t)*(unsigned long)NSEC_PER_MSEC)

static struct timer_list timer_button;
static struct hrtimer hrtimer_led_mmc;

const int TIMER_BUTTON_DELAY = 20;
const int HRTIMER_LED_MMC_DELAY = 1;

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

	pr_info("Init Button GPIO%d OK\n", gpio);
	return 0;

err_input:
	gpio_free(gpio);
err_register:
	return rc;
}

static void button_gpio_deinit(int gpio)
{
	gpio_free(gpio);
	pr_info("Deinit Button GPIO%d\n", gpio);
}


enum hrtimer_restart hrtimer_led_mmc_handler(struct hrtimer *a_hrtimer)
{
	gpio_set_value(LED_MMC, 0);
	return HRTIMER_NORESTART;
}

void timer_button_fun(struct timer_list *t)
{
	static u32 prev_state = 1;
	u32 current_state = 0;
	ktime_t ktime;

	current_state = gpio_get_value(BUTTON);

	if (current_state != prev_state) {

		gpio_set_value(LED_MMC, 1);

		ktime = ktime_set(0, MS_TO_NS(HRTIMER_LED_MMC_DELAY));
		hrtimer_start(&hrtimer_led_mmc, ktime, HRTIMER_MODE_REL);
	}
	prev_state = current_state;

	if (!current_state)
		gpio_set_value(LED_SD, 1);
	else
		gpio_set_value(LED_SD, 0);


	mod_timer(&timer_button,
			jiffies + msecs_to_jiffies(TIMER_BUTTON_DELAY));
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

	timer_setup(&timer_button, timer_button_fun, 0);
	mod_timer(&timer_button,
			jiffies + msecs_to_jiffies(TIMER_BUTTON_DELAY));

	hrtimer_init(&hrtimer_led_mmc, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	hrtimer_led_mmc.function = &hrtimer_led_mmc_handler;

	pr_info("Module loaded\n");

	return 0;

err_led:
	button_gpio_deinit(BUTTON);
err_button:
	return rc;
}

static void __exit gpio_poll_exit(void)
{
	hrtimer_cancel(&hrtimer_led_mmc);
	del_timer(&timer_button);

	gpio_set_value(LED_MMC, 0);
	gpio_set_value(LED_SD, 0);
	pr_info("LED at GPIO OFF\n");

	button_gpio_deinit(BUTTON);

	pr_info("Module exited\n");
}

module_init(gpio_poll_init);
module_exit(gpio_poll_exit);