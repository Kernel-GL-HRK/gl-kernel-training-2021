/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __GL_GPIO_H__
#define __GL_GPIO_H__

#include <linux/types.h>
#include <linux/delay.h>
#include <linux/interrupt.h>

#define GPIO_VAL_LOW 0
#define GPIO_VAL_HIGH 1

#define GPIO_IRQ_NONE		IRQF_TRIGGER_NONE
#define GPIO_IRQ_RISING_EDGE	IRQF_TRIGGER_RISING
#define GPIO_IRQ_FALLING_EDGE	IRQF_TRIGGER_FALLING
#define GPIO_IRQ_BOTH_EDGE	(IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING)
#define GPIO_IRQ_LEVEL_HIGH	IRQF_TRIGGER_HIGH
#define GPIO_IRQ_LEVEL_LOW	IRQF_TRIGGER_LOW

#define GPIO_IRQ_HANDLED	IRQ_HANDLED
#define GPIO_IRQ_WAKE_THREAD	IRQ_WAKE_THREAD

enum gl_gpio_dir {
	GPIO_DIR_IN = 0,
	GPIO_DIR_OUT
};

struct gl_gpio {
	u8 group;
	u8 pin;
	u32 debounce_ms;	// hw debounce (0 to disable)
	enum gl_gpio_dir dir;
	void *_ctx;
};

int gl_gpio_init(struct gl_gpio *gpio);

int gl_gpio_write(struct gl_gpio *gpio, int value);

int gl_gpio_read(struct gl_gpio *gpio, int *value);

int gl_gpio_poll(struct gl_gpio *gpio, void (*cb)(int, void *),
	void *data, u32 period_ms);

int gl_gpio_register_irq(struct gl_gpio *gpio,
	irq_handler_t hw_handler, u32 irq_mode);

int gl_gpio_register_threaded_irq(struct gl_gpio *gpio,
	irq_handler_t hw_handler, irq_handler_t sw_handler, u32 irq_mode);

void gl_gpio_deinit(struct gl_gpio *gpio);

#endif /* __GL_GPIO_H__ */
