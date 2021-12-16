// SPDX-License-Identifier: GPL-3.0
/**/

#define pr_fmt(fmt) KBUILD_MODNAME ": %s: " fmt, __func__

#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/ktime.h>

#define GPIO_NUMBER(port, bit) (32 * (port) + (bit))

/*	Board config:
 *	LED1 (D3): GPIO1_22 "uSD access"
 *	LED3 (D5): GPIO1_24 "eMMC access"
 *	BUTTON GPIO2_8
 */

#define LED_BUTTON_ON GPIO_NUMBER(1, 22)
#define LED_BUTTON_CH GPIO_NUMBER(1, 24)
#define BUTTON GPIO_NUMBER(2, 8)

static int led1_gpio = -1;
static int led3_gpio = -1;
static int button_gpio = -1;
static int button_state = -1;
static int button_cnt = -1;

static ktime_t timer_period;
struct hrtimer button_timer;

static enum hrtimer_restart timer_callback(struct hrtimer *timer)
{
	int cur_button_state;

	cur_button_state = gpio_get_value(button_gpio);
	button_cnt = (cur_button_state == button_state) ? (button_cnt + 1) : 0;
	button_state = cur_button_state;
	gpio_set_value(led3_gpio, ((button_cnt == 20) ? 1 : 0));
	if (button_cnt >= 20)
		gpio_set_value(led1_gpio, !button_state);
	hrtimer_forward(timer, timer->base->get_time(), timer_period);
	return HRTIMER_RESTART;  //restart timer
}

static int led_gpio_init(int gpio, int *led_gpio)
{
	int res;

	res = gpio_direction_output(gpio, 0);
	if (res != 0)
		return res;

	*led_gpio = gpio;
	return 0;
}

static int button_gpio_init(int gpio)
{
	int res;

	res = gpio_request(gpio, "Onboard user button");
	if (res != 0)
		return res;

	res = gpio_direction_input(gpio);
	if (res != 0)
		goto err_input;

	button_gpio = gpio;
	pr_info("Init GPIO%d OK\n", button_gpio);
	button_state = gpio_get_value(button_gpio);
	button_cnt = 0;
	return 0;

err_input:
	gpio_free(gpio);
	return res;
}

static void button_gpio_deinit(void)
{
	if (button_gpio >= 0) {
		gpio_free(button_gpio);
		pr_info("Deinit GPIO%d\n", button_gpio);
	}
}

/* Module entry/exit points */
static int __init gpio_poll_init(void)
{
	int res;

	res = button_gpio_init(BUTTON);
	if (res != 0) {
		pr_err("Can't set GPIO%d for button\n", BUTTON);
		return res;
	}

	timer_period = ktime_set(0, 1000000);      /*1 msec*/
	hrtimer_init(&button_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	hrtimer_start(&button_timer, timer_period, HRTIMER_MODE_REL);
	button_timer.function = timer_callback;

	res = led_gpio_init(LED_BUTTON_ON, &led1_gpio);
	if (res != 0) {
		pr_err("Can't set GPIO%d for output\n", LED_BUTTON_ON);
		goto err_led;
	}

	gpio_set_value(led1_gpio, 0);

	res = led_gpio_init(LED_BUTTON_CH, &led3_gpio);
	if (res != 0) {
		pr_err("Can't set GPIO%d for output\n", LED_BUTTON_CH);
		goto err_led;
	}
	gpio_set_value(led3_gpio, 0);
	return 0;

err_led:
	button_gpio_deinit();
	return res;
}

static void __exit gpio_poll_exit(void)
{
	gpio_set_value(led1_gpio, 0);
	gpio_set_value(led3_gpio, 0);
	button_gpio_deinit();
	hrtimer_cancel(&button_timer);
}

module_init(gpio_poll_init);
module_exit(gpio_poll_exit);

MODULE_AUTHOR("Malakhova.Maryna <maryna.malakhova@globallogic.com>");
MODULE_DESCRIPTION("Simple timer interrupt test");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

