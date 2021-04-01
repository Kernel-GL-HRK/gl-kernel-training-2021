// SPDX-License-Identifier: GPL-3.0
/**/
#define pr_fmt(fmt) KBUILD_MODNAME ": %s: " fmt, __func__
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/delay.h>

#define GPIO_NUMBER(port, bit) (32 * (port) + (bit))

/*	Board config:
 *	LED1 (D3): GPIO1_22 "uSD access"
 *	LED3 (D5): GPIO1_24 "eMMC access"
 *	BUTTON GPIO2_8
 */

#define LED_BUTTON_ON GPIO_NUMBER(1, 22)
#define LED_LONGWORK GPIO_NUMBER(1, 24)
#define BUTTON GPIO_NUMBER(2, 8)

static int led1_gpio = -1;
static int led3_gpio = -1;
static int btn_gpio = -1;
static int btn_irq;

static struct btn_irq_data_t {
	int busy_delay_sec;
	u32 btn_cnt;
} btn_irq_data;

static int simulate_busy;
module_param(simulate_busy, int, 0);

static irqreturn_t btn_irq_handler(int irq, void *data)
{
	struct btn_irq_data_t *ptr = (struct btn_irq_data_t *)data;

	gpio_set_value(LED_BUTTON_ON, !gpio_get_value(LED_BUTTON_ON));
	ptr->btn_cnt += 1;
	return IRQ_WAKE_THREAD;
}

static irqreturn_t button_threaded_irq(int irq, void *data)
{
	struct btn_irq_data_t  *ptr = (struct btn_irq_data_t *)data;

	pr_info("Button IRQ threaded counter: %u\n", ptr->btn_cnt);
	gpio_set_value(LED_LONGWORK, 1);
	msleep_interruptible(ptr->busy_delay_sec*1000);
	gpio_set_value(LED_LONGWORK, 0);
	pr_info("I`m still alive...\n");
	return IRQ_HANDLED;
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

static int btn_gpio_init(int gpio)
{
	int res;

	res = gpio_request(gpio, "Onboard user button");
	if (res != 0)
		return res;

	res = gpio_direction_input(gpio);
	if (res != 0)
		goto err_input;

	btn_irq = gpio_to_irq(gpio);
	if (res < 0)
		goto err_input;

	btn_gpio = gpio;
	pr_info("Init GPIO%d OK\n", btn_gpio);
	return 0;
err_input:
	gpio_free(gpio);
	return res;
}

static void btn_gpio_deinit(void)
{
	if (btn_gpio >= 0) {
		gpio_free(btn_gpio);
		pr_info("Deinit GPIO%d\n", btn_gpio);
	}
}

static int __init gpio_irq_init(void)
{
	int res;

	res = btn_gpio_init(BUTTON);
	if (res != 0) {
		pr_err("Can't set GPIO%d for button\n", BUTTON);
		return res;
	}

	res = led_gpio_init(LED_BUTTON_ON, &led1_gpio);
	if (res != 0) {
		pr_err("Can't set GPIO%d for output\n", LED_BUTTON_ON);
		goto err_ret;
	}

	gpio_set_value(led1_gpio, 0);

	res = led_gpio_init(LED_LONGWORK, &led3_gpio);
	if (res != 0) {
		pr_err("Can't set GPIO%d for output\n", LED_LONGWORK);
		goto err_ret;
	}
	gpio_set_value(led3_gpio, 0);

	btn_irq_data.busy_delay_sec = (simulate_busy > 0) ? simulate_busy : 0;
	res = request_threaded_irq(btn_irq, btn_irq_handler,
		button_threaded_irq, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
		"button_interrupt", &btn_irq_data);

	if (res != 0)
		goto err_ret;

	return 0;

err_ret:
	btn_gpio_deinit();
	return res;
}

static void __exit gpio_irq_exit(void)
{
	synchronize_irq(btn_irq);
	free_irq(btn_irq, &btn_irq_data);
	gpio_set_value(led3_gpio, 0);
	gpio_set_value(led1_gpio, 0);
	btn_gpio_deinit();
}

module_init(gpio_irq_init);
module_exit(gpio_irq_exit);

MODULE_AUTHOR("Malakhova.Maryna <maryna.malakhova@globallogic.com>");
MODULE_DESCRIPTION("Simple irq test");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

