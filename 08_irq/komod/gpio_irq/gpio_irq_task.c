// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

/*
 * 08_irq task gpio_poll
 *
 * Module which uses interrupts to detect button state changes.
 * It uses a threaded interrupt with both edge trigger enabled.
 * The hardware interrupt toggles LED1 and increments 32-bit counter.
 * The software interrupt reads 32-bit counter and prints its state.
 * Module supports 'simulate_busy' feature in software interrupt
 * which can be enabled via module-param simulate_busy=1,
 * LED3 will indicate busy state
 *
 * Author: Eduard Chaika <eduard.chaika@globallogic.com>
 *
 */
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>

#include "../gl_gpio/gl_gpio.h"

#ifndef KBUILD_MODNAME
#define KBUILD_MODNAME
#endif

#define BUSY_TIME_MS 1000
#define LED1_TOGGLE() toggle_led(&gpio_led1, &led1_state)
#define LED3_TOGGLE() toggle_led(&gpio_led3, &led3_state)

struct gl_gpio gpio_btn = {
	.group = 2,
	.pin = 8,
	.debounce_ms = 1,
	.dir = GPIO_DIR_IN
};

struct gl_gpio gpio_led1 = {
	.group = 1,
	.pin = 22,
	.dir = GPIO_DIR_OUT
};

struct gl_gpio gpio_led3 = {
	.group = 1,
	.pin = 24,
	.dir = GPIO_DIR_OUT
};

static int led1_state, led3_state;
static u32 button_irq_counter;
static uint simulate_busy;
module_param(simulate_busy, uint, 0);

static inline void toggle_led(struct gl_gpio *gpio, int *led_state)
{
	gl_gpio_write(gpio, !(*led_state));
	*led_state = !(*led_state);
}

irqreturn_t button_irq(int data, void *arg)
{
	LED1_TOGGLE();
	button_irq_counter++;
	return GPIO_IRQ_WAKE_THREAD;
}

irqreturn_t button_threaded_irq(int data, void *arg)
{
	if (simulate_busy == 0) {
		pr_info("GL Button IRQ counter: %u\n", button_irq_counter);

	} else {
		int busy = 20;
		const int busy_time = BUSY_TIME_MS / busy;

		while (busy--) {
			LED3_TOGGLE();
			mdelay(busy_time);
		}
		pr_info("GL Button IRQ counter: %u\n", button_irq_counter);
	}

	return GPIO_IRQ_HANDLED;
}

static int gl_button_init(void)
{
	int ret = 0;

	ret = gl_gpio_init(&gpio_btn);
	if (ret != 0)
		return ret;

	ret = gl_gpio_register_threaded_irq(&gpio_btn, button_irq,
		button_threaded_irq, GPIO_IRQ_BOTH_EDGE);
	if (ret != 0)
		goto exit_free;

	return ret;

exit_free:
	gl_gpio_deinit(&gpio_btn);
	return ret;
}

static int gl_led_init(void)
{
	int ret;

	ret = gl_gpio_init(&gpio_led1);
	if (ret != 0)
		goto exit_free;

	ret = gl_gpio_init(&gpio_led3);
	if (ret != 0)
		goto exit_free;

	return 0;

exit_free:
	gl_gpio_deinit(&gpio_led1);
	gl_gpio_deinit(&gpio_led3);
	return ret;
}

static int __init gl_gpio_irq_init(void)
{
	int ret;

	pr_info("GL Kernel Training 2021\n");
	pr_info("GL Simulate busy is %s\n", simulate_busy ? "ON" : "OFF");

	ret = gl_button_init();
	if (ret != 0)
		return -1;

	ret = gl_led_init();
	if (ret != 0)
		return -1;

	return 0;
}

static void __exit gl_gpio_irq_exit(void)
{
	pr_info("Exit from 08_irq module\n");
	gl_gpio_deinit(&gpio_btn);
	gl_gpio_deinit(&gpio_led1);
	gl_gpio_deinit(&gpio_led3);
}

module_init(gl_gpio_irq_init);
module_exit(gl_gpio_irq_exit);

MODULE_AUTHOR("Eduard Chaika <eduard.chaika@globallogic.com>");
MODULE_DESCRIPTION("GL Linux Kernel Training 08_irq");
MODULE_LICENSE("GPL");
