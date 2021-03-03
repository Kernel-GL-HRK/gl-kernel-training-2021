// SPDX-License-Identifier: MIT and GPL

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/ctype.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

#define DIR_NAME	"kharchuk"
#define ENT1_NAME	"rw"
#define ENT2_NAME	"stats"

#define REPORT_BUFF_SIZE	255

MODULE_DESCRIPTION("Sysfs lowercase converter");
MODULE_AUTHOR("vadym.kharchuk@globallogic.com");
MODULE_VERSION("0.1");
MODULE_LICENSE("Dual MIT/GPL");

static struct proc_dir_entry *dir;
static struct proc_dir_entry *ent1, *ent2;

static char message[PAGE_SIZE];

struct stats {
	int total_calls;
	int char_processed;
	int char_converted;
};

struct stats toupper_stats;
ssize_t msg_size;

static void string_toupper(char *msg)
{
	int i;

	for (i = 0; msg[i]; i++) {
		if (islower(msg[i])) {
			msg[i] = toupper(msg[i]);
			toupper_stats.char_converted++;
		}
	}

	toupper_stats.total_calls++;
	toupper_stats.char_processed += strlen(msg);
}

static ssize_t main_write(struct file *file, const char __user *pbuf,
			size_t count, loff_t *ppos)
{
	ssize_t not_copied;

	not_copied = copy_from_user(message, pbuf, count);
	msg_size = count - not_copied;
	string_toupper(message);

	return msg_size;
}

static ssize_t main_read(struct file *file, char __user *pbuf,
			size_t count, loff_t *ppos)
{
	return simple_read_from_buffer(pbuf, count, ppos,
					message, strlen(message));
}

static ssize_t stats_read(struct file *file, char __user *pbuf,
			size_t count, loff_t *ppos)
{
	char report[REPORT_BUFF_SIZE];

	snprintf(report, sizeof(report),
		"Total calls - %d, characters processed - %d characters converted - %d\n",
		toupper_stats.total_calls,
		toupper_stats.char_processed,
		toupper_stats.char_converted);

	return simple_read_from_buffer(pbuf, count, ppos,
					report, strlen(report));
}

const static struct file_operations main_ops = {
	.owner = THIS_MODULE,
	.read = main_read,
	.write = main_write
};

const static struct file_operations stats_ops = {
	.owner = THIS_MODULE,
	.read = stats_read,
};

static int __init firstmod_init(void)
{
	dir = proc_mkdir(DIR_NAME, NULL);
	if (dir == NULL) {
		pr_err("%s: error creating procfs directory\n",
			THIS_MODULE->name);
		return -ENOMEM;
	}

	ent1 = proc_create(ENT1_NAME, 0666, dir, &main_ops);
	if (ent1 == NULL) {
		pr_err("%s: error creating procfs entry\n",
			THIS_MODULE->name);
		remove_proc_entry(DIR_NAME, dir);
		return -ENOMEM;
	}

	ent2 = proc_create(ENT2_NAME, 0444, dir, &stats_ops);
	if (ent2 == NULL) {
		pr_err("%s: error creating procfs entry\n",
			THIS_MODULE->name);
		remove_proc_entry(ENT1_NAME, NULL);
		remove_proc_entry(DIR_NAME, NULL);
		return -ENOMEM;
	}

	pr_info("%s: loaded!\n", THIS_MODULE->name);

	return 0;
}

static void __exit firstmod_exit(void)
{
	proc_remove(dir);
	pr_info("%s: removed!\n", THIS_MODULE->name);
}

module_init(firstmod_init);
module_exit(firstmod_exit);

