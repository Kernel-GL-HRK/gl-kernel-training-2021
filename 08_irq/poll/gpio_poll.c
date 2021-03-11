// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>

#include <linux/timer.h>
#include <linux/moduleparam.h>
#include <linux/interrupt.h>
#include <linux/irqflags.h>

MODULE_AUTHOR("Andrii Synenko");
MODULE_DESCRIPTION("List data module for Linux Kernel ProCamp");
MODULE_LICENSE("Dual BSD/GPL");

static u32 state_led_pin;
module_param(state_led_pin, uint, 0);
MODULE_PARM_DESC(state_led_pin, "LED pin ID");

static u32 user_btn_pin;
module_param(user_btn_pin, uint, 0);
MODULE_PARM_DESC(user_btn_pin, "User button pin ID");

struct gpio_conf {
	u32 pin_id;
	bool output;

	int irq_id;
	irq_handler_t irq_handler;
	irq_handler_t irq_thread;
};

static struct gpio_conf user_btn  = {66, false, -1, NULL, NULL};
static struct gpio_conf state_led = {67, true, -1, NULL, NULL};

static struct timer_list g_timer;

static int gpio_init(struct gpio_conf *pin)
{
	int res = 0;

	res = gpio_request(pin->pin_id, "Onboard user button");
	if (res < 0) {
		pr_err("%s: failed to request gpio pin: %d",
		       __func__, pin->pin_id);
		return res;
	}

	res = (pin->output == true) ? gpio_direction_output(pin->pin_id, 0)
				: gpio_direction_input(pin->pin_id);
	if (res < 0)
		goto failed_gpio_init;

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
	pr_err("%s: failed to init gpio pin %d, error %d",
	       __func__, pin->pin_id, res);

	gpio_free(pin->pin_id);
	return res;
}


static void gpio_deinit(struct gpio_conf *pin)
{
	if (pin->irq_id >= 0)
		free_irq(pin->irq_id, NULL);

	gpio_free(pin->pin_id);
}

void gpio_debounce(struct timer_list *tm)
{
	static int prew_state;
	int curr_state = gpio_get_value(user_btn.pin_id);

	if (prew_state == curr_state)
		gpio_set_value(state_led.pin_id, 1 - curr_state);

	prew_state = curr_state;

	mod_timer(&g_timer, jiffies + msecs_to_jiffies(20));
}

static int gpio_poll_init(void)
{
	int res = 0;

	if (state_led_pin)
		state_led.pin_id = state_led_pin;
	if (user_btn_pin)
		user_btn.pin_id = user_btn_pin;

	pr_info("%s: module starting\n",  __func__);

	timer_setup(&g_timer, &gpio_debounce, 0);
	res = mod_timer(&g_timer, jiffies + msecs_to_jiffies(20));
	if (res < 0)
		return res;

	res = gpio_init(&state_led);
	if (res < 0)
		return res;

	res = gpio_init(&user_btn);
	if (res < 0) {
		del_timer(&g_timer);
		gpio_deinit(&state_led);
		return res;
	}
	return 0;
}

static void gpio_poll_exit(void)
{
	pr_info("%s: module exit\n",  __func__);

	del_timer(&g_timer);
	gpio_deinit(&state_led);
	gpio_deinit(&user_btn);
}

module_init(gpio_poll_init);
module_exit(gpio_poll_exit);
