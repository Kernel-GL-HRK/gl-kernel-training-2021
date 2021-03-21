// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>


MODULE_AUTHOR("Denys Andreichuk <Denys.Andreichuk@globallogic.com");
MODULE_DESCRIPTION("Module to test gpio functionality");
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

/* On-board button. */
#define BUTTON  GPIO_NUMBER(2, 8)

static int led_gpio = -1;
static int button_gpio = -1;

static struct timer_list timer_20ms;
static struct hrtimer timer_1ms;

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

static void button_gpio_deinit(void)
{
	if (button_gpio >= 0) {
		gpio_free(button_gpio);
		pr_info("Deinit GPIO%d\n", button_gpio);
	}
}


void process_button(struct timer_list *t)
{
	static u32 prev_state = 1;
	u32 current_state = 0;
	ktime_t ktime;

	current_state = gpio_get_value(BUTTON);

	if (current_state != prev_state) {

		gpio_set_value(LED_MMC, 1);

		ktime = ktime_set(0, 1000000);
		hrtimer_start(&timer_1ms, ktime, HRTIMER_MODE_REL);

		prev_state = current_state;
	}

	gpio_set_value(LED_SD, !current_state);

	mod_timer(&timer_20ms, jiffies + msecs_to_jiffies(20));
}


enum hrtimer_restart timer_1ms_callback(struct hrtimer *timer)
{
	gpio_set_value(LED_MMC, 0);
	return HRTIMER_NORESTART;
}


/* Module entry/exit points */
static int __init gpio_poll_init(void)
{
	int rc;
	int gpio;

	rc = button_gpio_init(BUTTON);
	if (rc) {
		pr_err("Can't set GPIO%d for button\n", BUTTON);
		goto err_button;
	}

	rc = led_gpio_init(LED_SD);
	if (rc) {
		pr_err("Can't set GPIO%d for LED_SD\n", gpio);
		goto err_led;
	}

	rc = led_gpio_init(LED_MMC);
	if (rc) {
		pr_err("Can't set GPIO%d for LED_MMC\n", gpio);
		goto err_led;
	}

	timer_setup(&timer_20ms, process_button, 0);
	mod_timer(&timer_20ms, jiffies + msecs_to_jiffies(20));

	hrtimer_init(&timer_1ms, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	timer_1ms.function = &timer_1ms_callback;

	pr_info("gpio_poll_module says: Hi!\n");

	return 0;

err_led:
	button_gpio_deinit();
err_button:
	return rc;
}

static void __exit gpio_poll_exit(void)
{
	del_timer(&timer_20ms);
	hrtimer_cancel(&timer_1ms);

	button_gpio_deinit();

	gpio_set_value(LED_SD, 0);
	gpio_set_value(LED_MMC, 0);

	pr_info("gpio_poll_module says: Bye!\n");
}

module_init(gpio_poll_init);
module_exit(gpio_poll_exit);

