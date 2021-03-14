// SPDX-License-Identifier: GPL-2.0
#define pr_fmt(fmt) KBUILD_MODNAME ": %s: " fmt, __func__

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/mic_bus.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oleksandr Redchuk (at GL training courses)");
MODULE_DESCRIPTION("BBB Onboard IO Demo");
MODULE_VERSION("0.1");

#define GPIO_NUMBER(port, bit) (32 * (port) + (bit))

/* On-board LESs
 *  0: D2	GPIO1_21	heartbeat
 *  1: D3	GPIO1_22	uSD access
 *  2: D4	GPIO1_23	active
 *  3: D5	GPIO1_24	eMMC access
 *
 * Note that uSD and eMMC LEDs are not used in nfs boot mode, but they are
 * already requested by correspondent drivers.
 * So that, we don't use gpio_request()/gpio_free() here, but control these
 * LEds (dirty!).
 * Use request/free if you want to control free GPIO routed to board connectors.
 */


#define LED_SD  GPIO_NUMBER(1, 22)
#define LED_MMC GPIO_NUMBER(1, 24)

#define BUTTON  GPIO_NUMBER(2, 8)
#define GPIO_LED 49
#define GPIO_BUTTON 117
#define GPIO_DEBOUNCE_TIME 200
#define TIMEOUT_SEC 5
#define MSEC_IN_SEC 1000
#define WAIT_QUEUE_SLEEP

int irq;
int my_id;
static unsigned int state_counter;
static int simulate_busy;

#ifdef WAIT_QUEUE_SLEEP
static wait_queue_head_t my_queue;
static unsigned int cond;
#endif //WAIT_QUEUE_SLEEP

module_param(simulate_busy, int, 0644);
MODULE_PARM_DESC(simulate_busy, "Busy thread simulating");

static int my_gpio_led_init(int gpio)
{
	int rc;

	rc = gpio_request(gpio, "My led");
	if (rc)
		goto err_req;

	rc = gpio_direction_output(gpio, 0);
	if (rc)
		goto err_out;
	pr_info("Init My GPIO%d OK\n", gpio);

	return 0;
err_out:
	gpio_free(gpio);
err_req:
	return rc;
}

static int my_gpio_button_init(int gpio)
{
	int rc;

	rc = gpio_request(gpio, "My button");
	if (rc)
		goto err_req;

	rc = gpio_direction_input(gpio);
	if (rc)
		goto err_input;

	pr_info("Init GPIO%d OK\n", gpio);
	return 0;

err_input:
	gpio_free(gpio);
err_req:
	return rc;
}

irqreturn_t my_irq_thread(int irq, void *data)
{
	pr_info("IRQ thread, counter - %u, sim_busy - %d\n", state_counter,
			simulate_busy);

	if (simulate_busy) {
		gpio_direction_output(LED_MMC, 1);
		pr_info("Simulate is busy, wait for work will be done...\n");
#ifndef WAIT_QUEUE_SLEEP
		msleep_interruptible(TIMEOUT_SEC * MSEC_IN_SEC);
#else
		wait_event_interruptible_timeout(my_queue, cond, HZ *
				TIMEOUT_SEC);
#endif //WAIT_QUEUE_SLEEP
		gpio_direction_output(LED_MMC, 0);
		pr_info("Simulate have finished his work.\n");
		simulate_busy = 0;
	}
	return 0;
}

irqreturn_t my_irq_handler(int irq, void *data)
{
	pr_info("IRQ HW handler\n");
	state_counter++;
	gpio_direction_output(GPIO_LED, state_counter & 1);

	return IRQ_WAKE_THREAD;
}

static int __init irq_button_init(void)
{
	int rc;

#ifdef WAIT_QUEUE_SLEEP
	/* Waitqueue */
	init_waitqueue_head(&my_queue);
#endif //WAIT_QUEUE_SLEEP

	rc = my_gpio_led_init(GPIO_LED);
	if (rc) {
		pr_err("Can't set GPIO%d for led\n", GPIO_LED);
		goto err_led;
	}

	rc = my_gpio_button_init(GPIO_BUTTON);
	if (rc) {
		pr_err("Can't set GPIO%d for button\n", GPIO_BUTTON);
		goto err_button;
	}

	irq = gpio_to_irq(GPIO_BUTTON);

	gpio_set_debounce(GPIO_BUTTON, GPIO_DEBOUNCE_TIME);

	rc = request_threaded_irq(irq, my_irq_handler, my_irq_thread,
			IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
			"touch_reset_key", &my_id);

	if (rc < 0) {
		pr_err("Request irq failed - %d\n", rc);
		goto err_req_irq;
	}

	pr_info("irq - %d\n", irq);

	return 0;

err_req_irq:
	gpio_free(GPIO_BUTTON);
err_button:
	gpio_free(GPIO_LED);
err_led:
	return rc;
}

static void __exit irq_button_exit(void)
{
#ifdef WAIT_QUEUE_SLEEP
	/* Wake up thread if its busy */
	cond = 1;
	wake_up_interruptible(&my_queue);
#endif //WAIT_QUEUE_SLEEP

	disable_irq(irq);
	free_irq(irq, &my_id);
	gpio_free(GPIO_BUTTON);
	gpio_free(GPIO_LED);
	pr_info("Deinit GPIO\n");
}

module_init(irq_button_init);
module_exit(irq_button_exit);

