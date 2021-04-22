// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/completion.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/kthread.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oleksiy Lyubochko <oleksiy.m.lyubochko@globallogic.com>");
MODULE_DESCRIPTION("A simple example Linux module.");
MODULE_VERSION("0.01");

#define MODULE_NAME "concurency"
//#define TESTING_TIMEOUT

struct thread_ctrl {
	struct task_struct *thrd;
	struct completion comp;
	int counter;
	int id;
};

struct mutex mutex;
static int common_counter;
static int threads_done_counter;

static int threads_amount;
struct thread_ctrl *threads;

module_param(threads_amount, int, 0644);
MODULE_PARM_DESC(threads_amount, "An integer");

int my_threadfn(void *data)
{
	int ret = 0, mut_ret = 0;
	struct thread_ctrl *tr_ctrl = (struct thread_ctrl *)data;

	do {
		set_current_state(TASK_INTERRUPTIBLE);

		if (mutex_lock_interruptible(&mutex))
			break;

		/* If the first thread after next step, need to increment
		 * steps
		 */
		if (!threads_done_counter)
			common_counter++;

		/* If this thread is the last on current step */
		if (++threads_done_counter == threads_amount)
			threads_done_counter = 0;

		tr_ctrl->counter++;

		pr_info("ID - %d, counter - %d\n", tr_ctrl->id,
				tr_ctrl->counter);

		/* If this thread already printed its counter */
		while (tr_ctrl->counter == common_counter &&
				threads_done_counter && !ret) {
			mutex_unlock(&mutex);

			/* Timeout while other threads are finishing */
			schedule_timeout(HZ / 1000);

			mut_ret = mutex_lock_interruptible(&mutex);
			if (mut_ret)
				break;

			ret = kthread_should_stop();
		}

		if (mut_ret)
			break;

#ifdef TESTING_TIMEOUT
		/* Testing timeout */
		schedule_timeout(HZ * 2);
#endif

		mutex_unlock(&mutex);

		ret = kthread_should_stop();
	} while (!ret && tr_ctrl->counter < threads_amount);

	set_current_state(TASK_RUNNING);
	pr_info("Thread ID - %d is done\n", tr_ctrl->id);
	complete_and_exit(&tr_ctrl->comp, 0);

	return 0;
}

static void stop_threads(struct thread_ctrl *threads, int max_id)
{
	int i;

	for (i = 0; i < max_id; i++) {
		if (!threads[i].comp.done) {
			pr_info("Thread ID %d is stopping from exit module\n",
					threads[i].id);
			kthread_stop(threads[i].thrd);
			wake_up_process(threads[i].thrd);
			wait_for_completion(&(threads[i].comp));
		}
	}
}

static int __init concurency_init(void)
{
	int ret, i;

	pr_info("Threads amount - %d!\n", threads_amount);

	threads = kmalloc_array(threads_amount, sizeof(struct thread_ctrl),
			GFP_KERNEL);

	if (!threads) {
		pr_err("%s error", __func__);
		return -ENOMEM;
	}

	mutex_init(&mutex);

	for (i = 0; i < threads_amount; i++) {
		threads[i].thrd = kthread_create(my_threadfn, &threads[i],
				"Test string\n");
		if (IS_ERR(threads[i].thrd)) {
			stop_threads(threads, i);
			mutex_destroy(&mutex);
			ret = PTR_ERR(threads[i].thrd);
			kfree(threads);
			pr_err("Error create thread - %d!\n", ret);
			return ret;
		}

		threads[i].counter = 0;
		threads[i].id = i + 1;
		init_completion(&(threads[i].comp));
		wake_up_process(threads[i].thrd);
	}

	pr_info(MODULE_NAME ": init success\n");

	return 0;
}


static void __exit concurency_exit(void)
{
	stop_threads(threads, threads_amount);
	mutex_destroy(&mutex);
	kfree(threads);
	pr_info(MODULE_NAME ": module release!\n");
}

module_init(concurency_init);
module_exit(concurency_exit);
