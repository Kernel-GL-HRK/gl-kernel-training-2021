// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include "gl_gpio.h"

#include <linux/timer.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>

#define GPIO_NUMBER(port, bit) (32 * (port) + (bit))

#define from_timer(var, callback_timer, timer_fieldname) \
	container_of(callback_timer, typeof(*var), timer_fieldname)

struct gl_timer_data {
	struct timer_list timer;
	struct gl_gpio_ctx *ctx;
	u32 period_ms;
};

struct gl_gpio_ctx {
	int gpio_number;
	enum gl_gpio_dir dir;
	bool gpio_in_use;
	bool irq_registred;
	void (*user_cb)(int data, void *user_data);
	void *user_data;
	struct gl_timer_data poll;
};

static void _timer_handler(struct timer_list *_timer)
{
	struct gl_timer_data *poll = from_timer(poll, _timer, timer);
	int data = gpio_get_value(poll->ctx->gpio_number);

	poll->ctx->user_cb(data, poll->ctx->user_data);
	mod_timer(_timer, jiffies + msecs_to_jiffies(poll->period_ms));
}

static void _timer_start(struct gl_timer_data *poll)
{
	timer_setup(&poll->timer, _timer_handler, 0);
	mod_timer(&poll->timer, jiffies + msecs_to_jiffies(poll->period_ms));
}

static int _gpio_init(int gpio_number, enum gl_gpio_dir dir, bool *in_use,
	u32 debounce_ms)
{
	int ret = 0;

	ret = gpio_request(gpio_number, NULL);
	if (ret != 0)
		*in_use = true;

	switch (dir) {
	case GPIO_DIR_IN:
		if (debounce_ms > 0) {
			ret = gpio_set_debounce(gpio_number, debounce_ms);
			if (ret != 0)
				goto exit_free;
		}

		ret = gpio_direction_input(gpio_number);
		if (ret != 0)
			goto exit_free;
	break;
	case GPIO_DIR_OUT:
		ret = gpio_direction_output(gpio_number, 0);
		if (ret != 0)
			goto exit_free;
	break;
	default:
		ret = -1;
	break;
	}

	return ret;

exit_free:
	gpio_free(gpio_number);
	return ret;
}

static inline void _gpio_deinit(int gpio_number)
{
	if (gpio_number > 0)
		gpio_free(gpio_number);
}

static bool _context_irq_valid(struct gl_gpio *gpio)
{
	struct gl_gpio_ctx *ctx = (struct gl_gpio_ctx *)gpio->_ctx;

	if (ctx == NULL) {
		pr_err("GL GPIO%d_%d not inited!\n", gpio->group, gpio->pin);
		return false;
	}

	if (ctx->dir != GPIO_DIR_IN) {
		pr_err("GL GPIO must be input!\n");
		return false;
	}

	if (ctx->irq_registred == true) {
		pr_err("GL GPIO IRQ already registred!\n");
		return false;
	}

	return true;
}

int gl_gpio_init(struct gl_gpio *gpio)
{
	int ret = 0;
	int gpio_number = GPIO_NUMBER(gpio->group, gpio->pin);
	int dir = gpio->dir;
	bool in_use = false;
	struct gl_gpio_ctx *ctx;

	ret = _gpio_init(gpio_number, dir, &in_use, gpio->debounce_ms);
	if (ret != 0) {
		pr_err("GL GPIO%d_%d init failed!\n", gpio->group, gpio->pin);
		goto exit;
	}

	if (in_use == true)
		pr_warn("WARNING: GPIO%d_%d already in use!\n",
			gpio->group, gpio->pin);

	ctx = kmalloc(sizeof(struct gl_gpio_ctx), GFP_KERNEL);
	if (ctx == NULL) {
		ret = -1;
		goto exit_deinit;
	}

	ctx->user_cb = ctx->user_data = NULL;
	ctx->gpio_number = gpio_number;
	ctx->gpio_in_use = in_use;
	ctx->irq_registred = false;
	ctx->dir = dir;

	gpio->_ctx = (void *)ctx;

	return ret;

exit_deinit:
	if (ctx->gpio_in_use == false)
		_gpio_deinit(gpio_number);
exit:
	return ret;
}

int gl_gpio_write(struct gl_gpio *gpio, int value)
{
	struct gl_gpio_ctx *ctx = gpio->_ctx;

	if (ctx == NULL) {
		pr_err("GL GPIO%d_%d not inited!\n", gpio->group, gpio->pin);
		return -1;
	}

	gpio_set_value(ctx->gpio_number, !!value);
	return 0;
}

int gl_gpio_read(struct gl_gpio *gpio, int *value)
{
	struct gl_gpio_ctx *ctx = gpio->_ctx;

	if (ctx == NULL) {
		pr_err("GL GPIO%d_%d not inited!\n", gpio->group, gpio->pin);
		return -1;
	}

	*value = gpio_get_value(ctx->gpio_number);
	return 0;
}

int gl_gpio_poll(struct gl_gpio *gpio, void (*cb)(int, void *),
	void *data, u32 period_ms)
{
	struct gl_gpio_ctx *ctx = (struct gl_gpio_ctx *)gpio->_ctx;

	if (ctx == NULL) {
		pr_err("GL GPIO%d_%d not inited!\n", gpio->group, gpio->pin);
		return -1;
	}

	if (ctx->dir != GPIO_DIR_IN) {
		pr_err("GL GPIO must be input!\n");
		return -1;
	}

	if (cb == NULL) {
		pr_err("GL GPIO invalid callback!\n");
		return -1;
	}

	if (ctx->user_cb != NULL) {
		pr_err("GL GPIO already polling!\n");
		return -1;
	}

	ctx->user_cb = cb;
	ctx->user_data = data;

	ctx->poll.ctx = ctx;
	ctx->poll.period_ms = period_ms;

	_timer_start(&ctx->poll);

	return 0;
}

int gl_gpio_register_irq(struct gl_gpio *gpio, irq_handler_t hw_handler,
	u32 irq_mode)
{
	int ret;
	struct gl_gpio_ctx *ctx = (struct gl_gpio_ctx *)gpio->_ctx;

	if (_context_irq_valid(gpio) == false)
		return -1;

	if (hw_handler == NULL) {
		pr_err("GL GPIO invalid irq handler!\n");
		return -1;
	}

	ret = request_irq(gpio_to_irq(ctx->gpio_number), hw_handler,
		irq_mode, NULL, NULL);
	if (ret != 0)
		return -1;

	ctx->irq_registred = true;
	return ret;
}

int gl_gpio_register_threaded_irq(struct gl_gpio *gpio,
	irq_handler_t hw_handler, irq_handler_t sw_handler, u32 irq_mode)
{
	int ret;
	struct gl_gpio_ctx *ctx = (struct gl_gpio_ctx *)gpio->_ctx;

	if (_context_irq_valid(gpio) == false)
		return -1;

	if (hw_handler == NULL || sw_handler == NULL) {
		pr_err("GL GPIO invalid irq handler!\n");
		return -1;
	}

	ret = request_threaded_irq(gpio_to_irq(ctx->gpio_number), hw_handler,
		sw_handler, irq_mode, NULL, NULL);
	if (ret != 0)
		return -1;

	ctx->irq_registred = true;
	return ret;
}

void gl_gpio_deinit(struct gl_gpio *gpio)
{
	struct gl_gpio_ctx *ctx = gpio->_ctx;

	if (ctx != NULL) {
		/* delete timer if active */
		if (ctx->user_cb != NULL)
			del_timer(&ctx->poll.timer);

		/* deinit irq if inited */
		if (ctx->irq_registred == true)
			free_irq(gpio_to_irq(ctx->gpio_number), NULL);

		/* deinit gpio if inited*/
		if (ctx->gpio_in_use == true)
			pr_warn("WARNING: GPIO%d_%d in use, do not deinit!\n",
				gpio->group, gpio->pin);
		else
			_gpio_deinit(ctx->gpio_number);

		/* context free*/
		kfree(ctx);
		gpio->_ctx = NULL;
	}
}
