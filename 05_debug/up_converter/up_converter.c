// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/bug.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oleksiy Lyubochko <oleksiy.m.lyubochko@globallogic.com>");
MODULE_DESCRIPTION("A simple example Linux module.");
MODULE_VERSION("0.01");

#define BUF_SIZE 4096
#define STAT_BUF_SIZE 256
#define CONV_DIR_NAME "up_conv"
#define CONV_NAME "converter"
#define CONV_STAT_NAME "stat"
#define UPPER_DIF_VALUE 32
#define STAT_STRING "Total calls - %d, total char processed - %d, total char converted - %d!\n"

static struct proc_dir_entry *dir, *ent_conv, *ent_stat;
static char buf[BUF_SIZE], buf_stat[STAT_BUF_SIZE];
static int wr_ptr, rd_ptr, buf_len;
static int stat_calls, stat_processed, stat_converted;

static void conv_to_upper(char *buf, size_t size)
{
	int i;

	for (i = 0; i < size; i++) {
		if (buf[i] >= 'a' && buf[i] <= 'z') {
			buf[i] = buf[i] - UPPER_DIF_VALUE;
			stat_converted++;
		}
		stat_processed++;
	}
}

static size_t get_part_size(size_t size, int ptr)
{
	size_t part_size;

	/* Get lenght no longer then the endge of the buffer */
	if (size > BUF_SIZE - ptr)
		part_size = BUF_SIZE - ptr;
	else
		part_size = size;

	return part_size;
}

static int conv_write(struct file *file, const char __user *pbuf, size_t count,
		loff_t *ppos)
{
	int ret;
	size_t part_size, size = 0;

	stat_calls++;

	if (buf_len == BUF_SIZE) {
		pr_err("up_conv: buffer is full\n");
		return -1;
	}

	if (count > BUF_SIZE - buf_len)
		count = BUF_SIZE - buf_len;

	while (size != count) {
		part_size = get_part_size(count - size, wr_ptr);

		ret = copy_from_user(buf + wr_ptr, pbuf + size, part_size);

		conv_to_upper(buf + wr_ptr, part_size);

		if (!ret) {
			wr_ptr += part_size;
			size += part_size;
			buf_len += part_size;
		}

		/* Check the and of the buffer for cicling */
		if (wr_ptr == BUF_SIZE)
			wr_ptr = 0;
	}

	return count;
}

static int conv_read(struct file *file, char __user *pbuf, size_t count,
		loff_t *ppos)
{
	int ret;
	size_t part_size, size = 0;

	stat_calls++;

	if (count > buf_len)
		count = buf_len;

	while (size != count) {
		part_size = get_part_size(count - size, rd_ptr);

		ret = copy_to_user(pbuf + size, buf + rd_ptr, part_size);

		if (!ret) {
			rd_ptr += part_size;
			size += part_size;
			buf_len -= part_size;
		}

		/* Check the and of the buffer for cicling */
		if (rd_ptr == BUF_SIZE)
			rd_ptr = 0;
	}

	return count;
}

static int stat_read(struct file *file, char __user *pbuf, size_t count,
		loff_t *ppos)
{
	int ret;

	ret = snprintf(buf_stat, STAT_BUF_SIZE, STAT_STRING, stat_calls,
			stat_processed, stat_converted);
	return simple_read_from_buffer(pbuf, count, ppos, buf_stat, ret);
}

static const struct file_operations conv_ops = {
	.owner = THIS_MODULE,
	.read = conv_read,
	.write = conv_write,
};

static const struct file_operations stat_ops = {
	.owner = THIS_MODULE,
	.read = stat_read,
};

static int __init up_conv_init(void)
{
	dir = proc_mkdir(CONV_DIR_NAME, NULL);
	if (dir == NULL) {
		pr_err("up_conv: error creating directory\n");
		return -ENOMEM;
	}

	ent_conv = proc_create(CONV_NAME, 0666, dir, &conv_ops);

	if (!ent_conv) {
		pr_err("up_conv: error creating converter\n");
		goto clean_dir;
	}

	ent_stat = proc_create(CONV_STAT_NAME, 0, dir, &stat_ops);

	if (!ent_stat) {
		pr_err("up_conv: error creating statistic\n");
		goto clean_ent_conv;
	}

	pr_info("up_conv: init success!\n");
	return 0;

clean_ent_conv:
	remove_proc_entry(CONV_NAME, dir);

clean_dir:
	remove_proc_entry(CONV_DIR_NAME, NULL);
	return -ENOMEM;
}

static void __exit up_conv_exit(void)
{
	proc_remove(dir);
	pr_info("up_conv: module release!\n");
}

module_init(up_conv_init);
module_exit(up_conv_exit);
