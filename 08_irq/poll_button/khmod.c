// SPDX-License-Identifier: MIT and GPL

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/timer.h>

MODULE_DESCRIPTION("Button poll module");
MODULE_AUTHOR("vadym.kharchuk@globallogic.com");
MODULE_VERSION("0.1");
MODULE_LICENSE("Dual MIT/GPL");

#define POLL_TIME_MS	20
#define BLINK_TIME_MS	1

#define GPIO_NUMBER(port, bit) (32 * (port) + (bit))

#define LED_SD  GPIO_NUMBER(1, 22)
#define LED_MMC GPIO_NUMBER(1, 24)

/* On-board button.
 *
 * LCD/HDMI interface must be disabled.
 */
#define BUTTON  GPIO_NUMBER(2, 8)

static int button_gpio = -1;

struct timer_list tim_l;
struct hrtimer tim_h;

enum hrtimer_restart hr_callback(struct hrtimer *timer)
{
	gpio_set_value(LED_MMC, 0);
	return HRTIMER_NORESTART;
}

static void button_poll(struct timer_list *data)
{
	static bool old_button_state;
	bool button_state;
	ktime_t kt;

	button_state = gpio_get_value(BUTTON);

	if (old_button_state != button_state) {
		gpio_set_value(LED_MMC, 1);
		kt = ms_to_ktime(BLINK_TIME_MS);
		hrtimer_start(&tim_h, kt, HRTIMER_MODE_REL);
	} else {
		gpio_set_value(LED_SD, button_state ? 0 : 1);
	}

	old_button_state = button_state;
	mod_timer(&tim_l, jiffies + msecs_to_jiffies(POLL_TIME_MS));
}

static int led_gpio_init(int gpio)
{
	int rc = 0;

	rc = gpio_direction_output(gpio, 0);

	return rc;
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

static void button_gpio_deinit(void)
{
	if (button_gpio >= 0) {
		gpio_free(button_gpio);
		pr_info("Deinit GPIO%d\n", button_gpio);
	}
}

/* Module entry/exit points */
static int __init onboard_io_init(void)
{
	int rc;

	rc = button_gpio_init(BUTTON);
	if (rc) {
		pr_err("Can't set GPIO%d for button\n", BUTTON);
		goto err_button;
	}

	rc = led_gpio_init(LED_SD);
	if (rc) {
		pr_err("Can't set GPIO%d for output\n", LED_SD);
		goto err_led;
	}

	rc = led_gpio_init(LED_MMC);
	if (rc) {
		pr_err("Can't set GPIO%d for output\n", LED_MMC);
		goto err_led;
	}

	timer_setup(&tim_l, button_poll, 0);
	add_timer(&tim_l);

	hrtimer_init(&tim_h, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	tim_h.function = &hr_callback;

	return 0;

err_led:
	button_gpio_deinit();
err_button:
	return rc;
}

static void __exit onboard_io_exit(void)
{
	int ret;

	ret = hrtimer_cancel(&tim_h);
	pr_info("hr_timer was %s", ret ? "active -> cancelled" : "not active");

	ret = try_to_del_timer_sync(&tim_l);
	pr_info("timer was %s", ret ? "not active" : "active -> cancelled");

	gpio_set_value(LED_MMC, 0);
	gpio_set_value(LED_SD, 0);

	button_gpio_deinit();
}

module_init(onboard_io_init);
module_exit(onboard_io_exit);

