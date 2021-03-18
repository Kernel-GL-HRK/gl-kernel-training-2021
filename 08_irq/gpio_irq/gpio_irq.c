// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/delay.h>


MODULE_AUTHOR("Denys Andreichuk <Denys.Andreichuk@globallogic.com");
MODULE_DESCRIPTION("Module to test irq");
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

struct cookie_t {
	bool state;
	u32 counter;
};

static struct cookie_t cookie;

static int button_irq;

static int simulate_busy_ms;
module_param(simulate_busy_ms, int, 0);

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


static irqreturn_t button_handler(int irq, void *ptr)
{
	struct cookie_t *cookie = (struct cookie_t *)ptr;

	cookie->state ^= 1;
	gpio_set_value(LED_SD, cookie->state);
	cookie->counter++;

	return IRQ_WAKE_THREAD;
}

static irqreturn_t thread_fn(int irq, void *ptr)
{
	struct cookie_t *cookie = (struct cookie_t *)ptr;

	pr_info("Counter: %i\n", cookie->counter);

	if (simulate_busy_ms > 0) {
		pr_info("Start waiting for %i ms\n", simulate_busy_ms);
		gpio_set_value(LED_MMC, 1);

		msleep(simulate_busy_ms);

		gpio_set_value(LED_MMC, 0);
		pr_info("Finish waiting\n");
	}

	return IRQ_HANDLED;
}


/* Module entry/exit points */
static int __init gpio_irq_init(void)
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

	button_irq = gpio_to_irq(BUTTON);
	if (button_irq < 0) {
		pr_err("Can't find GPIO%d IRQ\n", BUTTON);
		goto err_led;
	}

	rc = request_threaded_irq(button_irq, button_handler, thread_fn,
				IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING,
				"boot button irq test", &cookie);
	if (rc < 0) {
		pr_err("Request for IRQ %d failed\n", button_irq);
		goto err_led;
	}

	pr_info("gpio_irq_module says: Hi!\n");

	return 0;

err_led:
	button_gpio_deinit();
err_button:
	return rc;
}

static void __exit gpio_irq_exit(void)
{
	free_irq(button_irq, &cookie);

	button_gpio_deinit();

	gpio_set_value(LED_SD, 0);
	gpio_set_value(LED_MMC, 0);

	pr_info("gpio_irq_module says: Bye!\n");
}

module_init(gpio_irq_init);
module_exit(gpio_irq_exit);

