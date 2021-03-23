// SPDX-License-Identifier: GPL-2.0
#define pr_fmt(fmt) KBUILD_MODNAME ": %s: " fmt, __func__

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/mic_bus.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oleksiy Lyubochko <oleksiy.m.lyubochko@globallogic.com>");
MODULE_DESCRIPTION("A simple example Linux module.");
MODULE_VERSION("0.01");

#define GPIO_NUMBER(port, bit) (32 * (port) + (bit))

/* On-board LESs
 *  0: D2	GPIO1_21	heartbeat
 *  1: D3	GPIO1_22	uSD access
 *  2: D4	GPIO1_23	active
 *  3: D5	GPIO1_24	eMMC access
 *
 * Note that uSD and eMMC LEDs are not used in nfs boot mode, but they are
 * already requested by correspondent drivers.
 * So that, we don't use gpio_request()/gpio_free() here, but control these
 * LEds (dirty!).
 * Use request/free if you want to control free GPIO routed to board
 * connectors.
 */

#define LED_MMC GPIO_NUMBER(1, 24)
#define GPIO_LED GPIO_NUMBER(1, 17)
#define GPIO_BUTTON GPIO_NUMBER(3, 21)
#define TIMER_DELAY 20
#define DEBOUNCE_MASK 0x07
#define IS_BUTTON_PRESSED(state) (((state) & DEBOUNCE_MASK) == DEBOUNCE_MASK)
#define IS_BUTTON_NOTPRESSED(state) (((state) & DEBOUNCE_MASK) == 0)

static int button_state;
static struct timer_list simple_timer;
static struct hrtimer hr_timer;
static unsigned long timer_interval_ns = 1e6;

static enum hrtimer_restart hrtimer_handler(struct hrtimer *timer)
{
	gpio_set_value(GPIO_LED, 0);
	pr_info("HR Timer module calback\n");

	return HRTIMER_NORESTART;
}

void timer_handler(struct timer_list *timer)
{
	ktime_t interval;
	static uint8_t state;
	uint8_t filtered_state;
	static uint8_t old_filtered_state;

	mod_timer(timer, jiffies + (msecs_to_jiffies(TIMER_DELAY)));

	state <<= 1;
	state |= gpio_get_value(GPIO_BUTTON);

	if (IS_BUTTON_PRESSED(state) || IS_BUTTON_NOTPRESSED(state)) {
		filtered_state = (state & 1) ? 1 : 0;

		if (filtered_state != old_filtered_state) {
			pr_info("Timerfunction, button state have changed\n");
			gpio_set_value(LED_MMC, state & 1);

			gpio_set_value(GPIO_LED, 1);
			interval = ktime_set(0, timer_interval_ns);
			hrtimer_start(&hr_timer, interval, HRTIMER_MODE_REL);
			button_state = state;

			old_filtered_state = filtered_state;
		}
	}
}

static int my_gpio_led_init(int gpio)
{
	int rc;

	rc = gpio_request(gpio, "My led");
	if (rc)
		goto err_req;

	rc = gpio_direction_output(gpio, 0);
	if (rc)
		goto err_out;
	pr_info("Init My GPIO%d OK\n", gpio);

	return 0;
err_out:
	gpio_free(gpio);
err_req:
	return rc;
}

static int my_gpio_button_init(int gpio)
{
	int rc;

	rc = gpio_request(gpio, "My button");
	if (rc)
		goto err_req;

	rc = gpio_direction_input(gpio);
	if (rc)
		goto err_input;

	pr_info("Init GPIO%d OK\n", gpio);
	return 0;

err_input:
	gpio_free(gpio);
err_req:
	return rc;
}

static int __init check_button_init(void)
{
	int rc;

	rc = my_gpio_led_init(GPIO_LED);
	if (rc) {
		pr_err("Can't set GPIO%d for led\n", GPIO_LED);
		goto err_led;
	}

	rc = my_gpio_button_init(GPIO_BUTTON);
	if (rc) {
		pr_err("Can't set GPIO%d for button\n", GPIO_BUTTON);
		goto err_button;
	}

	pr_info("HR Timer module init\n");
	hrtimer_init(&hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	hr_timer.function = &hrtimer_handler;

	timer_setup(&simple_timer, timer_handler, 0);
	mod_timer(&simple_timer, jiffies + msecs_to_jiffies(TIMER_DELAY));
	pr_info("Timer inited\n");

	return 0;

err_button:
	gpio_free(GPIO_LED);
err_led:
	return rc;
}

static void __exit check_button_exit(void)
{
	int ret;

	ret = hrtimer_cancel(&hr_timer);
	pr_info("hr_timer was %s", ret ? "active -> cancelled" : "not active");

	try_to_del_timer_sync(&simple_timer);
	pr_info("Timer deleted\n");

	gpio_free(GPIO_BUTTON);
	gpio_free(GPIO_LED);
	pr_info("Deinit GPIO\n");
}

module_init(check_button_init);
module_exit(check_button_exit);

