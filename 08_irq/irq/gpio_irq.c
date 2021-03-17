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

MODULE_DESCRIPTION("Basic module demo: working with gpio");
MODULE_AUTHOR("AlexShlikhta oleksandr.shlikhta@globallogic.com");
MODULE_VERSION("0.1");
MODULE_LICENSE("Dual MIT/GPL"); // this affects the kernel behavior

static bool simulate_busy;
module_param(simulate_busy, bool, 0);
MODULE_PARM_DESC(cnt, "enabling delay");

#define LED_1_PIN 60
#define LED_1_LABEL "LED_1"
#define LED_2_PIN 44
#define LED_2_LABEL "LED_2"
#define BUTTON_PIN 7
#define BUTTON_LABEL "BUTTON"
#define BUTTON_DEBOUNCE 2000 //us
#define IRQ_TRIG (IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING)
#define TH_DELAY 3000 //ms

#define LOW false
#define HIGH true

#define MY_GPIO_INT_NAME "my_button_int"
#define MY_DEV_NAME "my_device"

static uint32_t var;

bool led_1_state;
bool blink_en;

static unsigned long flags;
static int button_irq;

static irqreturn_t button_handler(int irq, void *data)
{
	local_irq_save(flags);

	led_1_state = !led_1_state;
	gpio_set_value(LED_1_PIN, led_1_state);
	var++;

	local_irq_restore(flags);
	return IRQ_WAKE_THREAD;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

static irqreturn_t thread_fn(int irq, void *ptr)
{
	if (simulate_busy) {
		pr_info("############################\n");
		gpio_set_value(LED_2_PIN, HIGH);
		pr_info("Went to bed:)\n");
		msleep(TH_DELAY);
		gpio_set_value(LED_2_PIN, LOW);
		pr_info("Woke up:)\n");
	}

	pr_info("'var' state is %d\n", var);

	return IRQ_HANDLED;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

int gpio_in_init(uint16_t pin, const char *label, uint32_t debounce)
{
	int res;

	if (!gpio_is_valid(pin))
		return -EINVAL;

	res = gpio_request_one(pin, GPIOF_IN, label);

	if (res < 0)
		return res;

	return gpio_set_debounce(pin, debounce);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

int gpio_out_init(uint16_t pin, const char *label)
{
	if (!gpio_is_valid(pin))
		return -EINVAL;

	return gpio_request_one(pin, GPIOF_OUT_INIT_LOW, label);
}

///////////////////////////////////////////////////////////////////////////////
//###########################INIT CALLBACK#####################################
///////////////////////////////////////////////////////////////////////////////

int __init mod_init(void)
{
	int state;

	led_1_state = LOW;
	blink_en = false;

	state = gpio_out_init(LED_1_PIN, LED_1_LABEL);
	if (state < 0) {
		pr_err("gpio '%s' init failed:(\n", LED_1_LABEL);
		return state;
	}

	state = gpio_out_init(LED_2_PIN, LED_2_LABEL);
	if (state < 0) {
		pr_err("gpio '%s' init failed:(\n", LED_2_LABEL);
		return state;
	}

	state = gpio_in_init(BUTTON_PIN, BUTTON_LABEL, BUTTON_DEBOUNCE);
	if (state < 0) {
		pr_err("gpio '%s' init failed:(\n", BUTTON_LABEL);
		return state;
	}

	button_irq = gpio_to_irq(BUTTON_PIN);
	if (button_irq < 0) {
		pr_err("'gpio_to_irq' err :(\n");
		return button_irq;
	}

	state = request_threaded_irq(button_irq, button_handler, thread_fn,
				     IRQ_TRIG, MY_GPIO_INT_NAME, MY_DEV_NAME);

	if (state < 0) {
		pr_err("'request_treaded_irq' err :(\n");
		return state;
	}

	pr_info("'gpio_irq' module initialized\n");

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
//###########################EXIT CALLBACK#####################################
///////////////////////////////////////////////////////////////////////////////

void mod_cleanup(void)
{
	gpio_set_value(LED_1_PIN, LOW);
	gpio_set_value(LED_2_PIN, LOW);

	gpio_free(LED_1_PIN);
	gpio_free(LED_2_PIN);
	free_irq(button_irq, MY_DEV_NAME);
	gpio_free(BUTTON_PIN);

	pr_info("'gpio_irq' module released\n");
}

module_init(mod_init);
module_exit(mod_cleanup);
