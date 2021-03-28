// SPDX-License-Identifier: MIT and GPL
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <linux/jiffies.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Anton Kotsiubailo");
MODULE_DESCRIPTION("A Button/LED test driver for the BBB");
MODULE_VERSION("0.1");

#define BUTTON_DEBOUNCE		2000
#define MY_GPIO_INT_NAME "my_button_int"
#define MY_DEV_NAME	"my_device"
#define LOW		false

static unsigned int gpioLED1 = 48;
static unsigned int gpioLED2 = 49;
static unsigned int gpioButton = 117;
static bool ledOn;
static int led_gpio = -1;
static int button_gpio = -1;
static uint32_t counter;
static int button_irq;

static uint simulate_busy;
module_param(simulate_busy, uint, 0);

irqreturn_t button_threaded_irq(int data, void *arg)
{
	if (simulate_busy == 0) {
		pr_info("GL Button IRQ counter: %u\n", counter);

	} else {
		int busy = 20;
		const int busy_time = BUTTON_DEBOUNCE / busy;

		while (busy--) {
			ledOn = !ledOn;
			gpio_set_value(gpioLED1, ledOn);
			mdelay(busy_time);
		}
		pr_info("GL Button IRQ counter: %u\n", counter);
	}

	return IRQ_HANDLED;
}

irqreturn_t button_handler(int data, void *arg)
{
	ledOn = !ledOn;
	gpio_set_value(gpioLED1, ledOn);
	pr_info("GPIO_TEST: Interrupt! (button state is %d)\n",
		gpio_get_value(gpioButton));
	counter++;
	return IRQ_WAKE_THREAD;
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

	return gpio_set_debounce(gpio, BUTTON_DEBOUNCE);

err_input:
	gpio_free(gpio);
err_register:
	return rc;
}

int __init mod_init(void)
{
	pr_info("GL Simulation busy is %s\n", simulate_busy ? "ON" : "OFF");
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

	button_irq = gpio_to_irq(gpioButton);

	if (button_irq < 0) {
		pr_err("'gpio_to_irq' err :(\n");
		return button_irq;
	}

	res = request_threaded_irq(button_irq, button_handler,
					button_threaded_irq,
				     IRQF_TRIGGER_RISING, MY_GPIO_INT_NAME,
				     MY_DEV_NAME);

	if (res < 0) {
		pr_err("'request_treaded_irq' err :(\n");
		return res;
	}

	pr_info("'gpio_irq' module initialized\n");

	return 0;
}

void __exit mod_cleanup(void)
{
	gpio_set_value(gpioLED1, LOW);
	gpio_set_value(gpioLED2, LOW);

	gpio_free(gpioLED1);
	gpio_free(gpioLED2);
	gpio_free(gpioButton);

	free_irq(button_irq, MY_DEV_NAME);
	pr_info("'gpio_poll' module released\n");
}

module_init(mod_init);
module_exit(mod_cleanup);
