// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>

//#include <linux/gpio/driver.h>

#include <linux/delay.h>
#include <linux/moduleparam.h>
#include <linux/interrupt.h>
#include <linux/irqflags.h>

MODULE_AUTHOR("Andrii Synenko");
MODULE_DESCRIPTION("List data module for Linux Kernel ProCamp");
MODULE_LICENSE("Dual BSD/GPL");

#define GPIO_NUMBER(port, bit) (32 * (port) + (bit))

static u32 state_led_pin;
module_param(state_led_pin, uint, 0);
MODULE_PARM_DESC(state_led_pin, "LED pin ID");

static u32 user_btn_pin;
module_param(user_btn_pin, uint, 0);
MODULE_PARM_DESC(user_btn_pin, "User button pin ID");

static u32 busy_led_pin;
module_param(busy_led_pin, uint, 0);
MODULE_PARM_DESC(busy_led_pin, "Busy LED pin ID");

static u32 simulate_busy;
module_param(simulate_busy, uint, 0);
MODULE_PARM_DESC(simulate_busy, "Sinulation delay in ms");

struct gpio_conf {
	u32 pin_id;
	bool output;

	int irq_id;
	irq_handler_t irq_handler;
	irq_handler_t irq_thread;
};

static struct gpio_conf user_btn  = {66, false, -1, NULL, NULL};
static struct gpio_conf state_led = {67, true, -1, NULL, NULL};
static struct gpio_conf busy_led  = {68, true, -1, NULL, NULL};
static u32 hit_counter;

static int gpio_init(struct gpio_conf *pin)
{
	int res = 0;

	res = gpio_request(pin->pin_id, "Onboard user button");
	if (res < 0) {
		pr_err("%s: failed to request gpio pin: %d\n",
		       __func__, pin->pin_id);
		return res;
	}

	res = (pin->output == true) ? gpio_direction_output(pin->pin_id, 0)
				: gpio_direction_input(pin->pin_id);
	if (res < 0)
		goto failed_gpio_init;

	if (pin->output == false)
		gpio_set_debounce(pin->pin_id, 2000);

	if (pin->irq_handler == NULL && pin->irq_thread == NULL)
		return 0;

	pin->irq_id = gpio_to_irq(pin->pin_id);
	if (pin->irq_id < 0) {
		res = pin->irq_id;
		goto failed_gpio_init;
	}

	res = request_threaded_irq(pin->irq_id, pin->irq_handler,
				pin->irq_thread,
				IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
				"user button hit", NULL);

	if (res >= 0)
		return res;

failed_gpio_init:
	pr_err("%s: failed to init gpio pin %d, error %d\n",
	       __func__, pin->pin_id, res);

	gpio_free(pin->pin_id);
	return res;
}


static void gpio_deinit(struct gpio_conf *pin)
{
	if (pin->output == false)
		gpio_set_debounce(pin->pin_id, 0);

	if (pin->irq_id >= 0)
		free_irq(pin->irq_id, NULL);

	gpio_free(pin->pin_id);
}

static irqreturn_t btn_handler(int irq, void *prt)
{
	if (gpio_get_value(state_led.pin_id))
		gpio_set_value(state_led.pin_id, 0);
	else
		gpio_set_value(state_led.pin_id, 1);

	hit_counter++;

	return IRQ_WAKE_THREAD;
}

static irqreturn_t btn_thread(int irq, void *prt)
{
	gpio_set_value(busy_led.pin_id, 1);
	msleep(simulate_busy);
	gpio_set_value(busy_led.pin_id, 0);

	pr_info("%s: button was hit %d time(s)\n", __func__, hit_counter);

	return IRQ_HANDLED;
}


static int gpio_irq_init(void)
{
	int res = 0;

	if (state_led_pin)
		state_led.pin_id = state_led_pin;
	if (busy_led_pin)
		user_btn.pin_id = busy_led_pin;
	if (user_btn_pin)
		user_btn.pin_id = user_btn_pin;

	user_btn.irq_handler = &btn_handler;
	user_btn.irq_thread = &btn_thread;

	pr_info("%s: module starting\n",  __func__);

	res = gpio_init(&state_led);
	if (res < 0)
		return res;

	res = gpio_init(&busy_led);
	if (res < 0)
		goto exit_deinit_state_led;

	res = gpio_init(&user_btn);
	if (res < 0)
		goto exit_deinit_all;

	return res;

exit_deinit_all:
	gpio_deinit(&busy_led);

exit_deinit_state_led:
	gpio_deinit(&state_led);
	return res;

}

static void gpio_irq_exit(void)
{
	pr_info("%s: module exit\n",  __func__);

	gpio_deinit(&user_btn);
	gpio_deinit(&busy_led);
	gpio_deinit(&state_led);
}

module_init(gpio_irq_init);
module_exit(gpio_irq_exit);
