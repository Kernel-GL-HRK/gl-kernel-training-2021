// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/printk.h>
#include <linux/interrupt.h>
#include <linux/moduleparam.h>
#include <linux/delay.h>

MODULE_AUTHOR("Vitalii Irkha <vitalii.o.irkha@globallogic.com");
MODULE_DESCRIPTION("Interrupt handling in Linux Kernel Training");
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
#define BTN_DEBOUNCE 2000 //us
#define SLEEP_TIME 3000   //ms

static int simulate_busy;
static int btn_state_counter;
static int led_sd_state;

module_param(simulate_busy, int, 0);
MODULE_PARM_DESC(simulate_busy, "Busy in thread_fn");

static int button_irq;

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

	gpio_set_debounce(gpio, BTN_DEBOUNCE);
	pr_info("Init Button GPIO%d OK\n", gpio);
	return 0;

err_input:
	gpio_free(gpio);
err_register:
	return rc;
}

static void button_gpio_deinit(int gpio)
{
	gpio_set_debounce(gpio, 0);
	gpio_free(gpio);
	pr_info("Deinit Button GPIO%d\n", gpio);
}


static irqreturn_t button_handler(int irq, void *ptr)
{
	led_sd_state ^= 0x1;
	gpio_set_value(LED_SD, led_sd_state);
	btn_state_counter++;
	return IRQ_WAKE_THREAD;
}

static irqreturn_t thread_fn(int irq, void *ptr)
{
	pr_info("Counter value: %d\n", btn_state_counter);

	if (simulate_busy > 0) {
		pr_info("Going to sleep for %d(ms)\n", SLEEP_TIME);
		gpio_set_value(LED_MMC, 1);
		msleep(SLEEP_TIME);
		gpio_set_value(LED_MMC, 0);
		pr_info("Waking up!\n");
	}

	return IRQ_HANDLED;
}

static int __init gpio_irq_init(void)
{
	int rc = 0;

	rc = button_gpio_init(BUTTON);
	if (rc) {
		pr_err("Can't set GPIO%d for button\n", BUTTON);
		goto err_button;
	}

	rc = led_gpio_init(LED_MMC);
	if (rc) {
		pr_err("Can't set GPIO%d for output\n", LED_MMC);
		goto err_led_irq;
	}

	rc = led_gpio_init(LED_SD);
	if (rc) {
		pr_err("Can't set GPIO%d for output\n", LED_SD);
		goto err_led_irq;
	}

	button_irq = gpio_to_irq(BUTTON);
	if (button_irq < 0) {
		pr_err("Can't find GPIO%d IRQ\n", BUTTON);
		goto err_led_irq;
	}

	rc = request_threaded_irq(button_irq, button_handler, thread_fn,
				IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING,
				"button test", NULL);
	if (rc < 0) {
		pr_err("Request for IRQ %d failed\n", button_irq);
		goto err_led_irq;
	}

	return 0;

err_led_irq:
	button_gpio_deinit(BUTTON);
err_button:
	return rc;
}

static void __exit gpio_irq_exit(void)
{
	button_gpio_deinit(BUTTON);
	free_irq(button_irq, NULL);

	gpio_set_value(LED_MMC, 0);
	gpio_set_value(LED_SD, 0);
	pr_info("LED at GPIO OFF\n");

}

module_init(gpio_irq_init);
module_exit(gpio_irq_exit);
