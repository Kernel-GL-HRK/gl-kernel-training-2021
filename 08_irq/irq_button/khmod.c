// SPDX-License-Identifier: MIT and GPL

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <linux/kthread.h>
#include <linux/delay.h>

MODULE_DESCRIPTION("Button irq module");
MODULE_AUTHOR("vadym.kharchuk@globallogic.com");
MODULE_VERSION("0.1");
MODULE_LICENSE("Dual MIT/GPL");

#define BLINK_TIME_MS	3000
#define DEBOUNCE_MS	5

#define GPIO_NUMBER(port, bit) (32 * (port) + (bit))

#define LED_SD  GPIO_NUMBER(1, 22)
#define LED_MMC GPIO_NUMBER(1, 24)

/* On-board button.
 *
 * LCD/HDMI interface must be disabled.
 */
#define BUTTON  GPIO_NUMBER(2, 8)

static bool simulate_busy;
module_param(simulate_busy, bool, 0);
MODULE_PARM_DESC(simulate_busy, "Is sleep should be used");

struct irq_data_s {
	bool button_state;
	unsigned int counter;
	int button_irq;
};

static int button_gpio = -1;

struct irq_data_s irq_data;

static irqreturn_t button_handler(int irq, void *ptr)
{
	struct irq_data_s *data = (struct irq_data_s *)ptr;

	data->button_state = !data->button_state;
	gpio_set_value(LED_SD, data->button_state);
	data->counter++;

	return IRQ_WAKE_THREAD;
}

static irqreturn_t button_fn(int irq, void *ptr)
{
	struct irq_data_s *data = (struct irq_data_s *)ptr;

	pr_info("IRQ count - %d\n", data->counter);

	if (simulate_busy) {
		gpio_set_value(LED_MMC, 1);
		msleep(BLINK_TIME_MS);
		gpio_set_value(LED_MMC, 0);
	}

	return IRQ_HANDLED;
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

	gpio_set_debounce(gpio, DEBOUNCE_MS);

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
		gpio_set_debounce(BUTTON, 0);
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

	irq_data.button_irq = gpio_to_irq(BUTTON);
	if (irq_data.button_irq < 0) {
		pr_err("Can't find GPIO%d IRQ\n", BUTTON);
		goto err_out;
	}

	rc = request_threaded_irq(irq_data.button_irq,
			button_handler, button_fn,
			IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING,
			"button_irq", &irq_data);
	if (rc) {
		pr_err("Err while request threaded irq\n");
		goto err_out;
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

	return 0;
err_led:
	free_irq(irq_data.button_irq, &irq_data);
err_out:
	button_gpio_deinit();
err_button:
	return rc;
}

static void __exit onboard_io_exit(void)
{
	gpio_set_value(LED_MMC, 0);
	gpio_set_value(LED_SD, 0);

	free_irq(irq_data.button_irq, &irq_data);
	button_gpio_deinit();
}

module_init(onboard_io_init);
module_exit(onboard_io_exit);

