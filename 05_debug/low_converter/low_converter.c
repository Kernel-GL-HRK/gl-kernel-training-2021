// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/bug.h>
#include <linux/device.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oleksiy Lyubochko <oleksiy.m.lyubochko@globallogic.com>");
MODULE_DESCRIPTION("A simple example Linux module.");
MODULE_VERSION("0.01");

#define BUF_SIZE 32
#define STAT_BUF_SIZE 256
#define CLASS_NAME "low_conv"
#define UPPER_DIF_VALUE 32
#define STAT_STRING "Total calls - %d, total char processed - %d, total char converted - %d!\n"

static char buf[BUF_SIZE];
static int wr_ptr, rd_ptr, buf_len;
static int stat_calls, stat_processed, stat_converted;
static struct class *attr_class;

static void conv_to_lower(char *buf, size_t size)
{
	int i;

	for (i = 0; i < size; i++) {
		if (buf[i] >= 'A' && buf[i] <= 'Z') {
			buf[i] = buf[i] + UPPER_DIF_VALUE;
			stat_converted++;
		}
		stat_processed++;
	}
}

static size_t get_part_size(size_t size, int ptr)
{
	size_t part_size;

	/* Get length no longer than the edge of the buffer */
	if (size > BUF_SIZE - ptr)
		part_size = BUF_SIZE - ptr;
	else
		part_size = size;

	return part_size;
}


static ssize_t conv_show(struct class *class, struct class_attribute *attr,
		char *pbuf)
{
	size_t part_size, count, size = 0;

	stat_calls++;

	count = buf_len;

	while (size != count) {
		part_size = get_part_size(count - size, rd_ptr);

		memcpy(pbuf + size, buf + rd_ptr, part_size);

		rd_ptr += part_size;
		size += part_size;
		buf_len -= part_size;

		/* Check the and of the buffer for cycling */
		if (rd_ptr == BUF_SIZE)
			rd_ptr = 0;
	}

	return count;
}

static ssize_t conv_store(struct class *class, struct class_attribute *attr,
		const char *pbuf, size_t count)
{
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

		memcpy(buf + wr_ptr, pbuf + size, part_size);

		conv_to_lower(buf + wr_ptr, part_size);

		wr_ptr += part_size;
		size += part_size;
		buf_len += part_size;

		/* Check the and of the buffer for cycling */
		if (wr_ptr == BUF_SIZE)
			wr_ptr = 0;
	}

	return count;
}

static ssize_t stat_show(struct class *class, struct class_attribute *attr,
		char *st_buf)
{
	return snprintf(st_buf, STAT_BUF_SIZE, STAT_STRING, stat_calls,
			stat_processed, stat_converted);
}

CLASS_ATTR_RW(conv);
CLASS_ATTR_RO(stat);

static int __init up_conv_init(void)
{
	int ret;

	attr_class = class_create(THIS_MODULE, CLASS_NAME);
	if (attr_class == NULL) {
		pr_err("low_conv: error creating sysfs class\n");
		return -ENOMEM;
	}

	ret = class_create_file(attr_class, &class_attr_conv);
	if (ret) {
		pr_err("low_conv: error creating sysfs class attribute conv\n");
		goto clean_class;
	}

	ret = class_create_file(attr_class, &class_attr_stat);
	if (ret) {
		pr_err("low_conv: error creating sysfs class attribute stat\n");
		goto clean_conv;
	}

	pr_info("low_conv: init success\n");

	return 0;

clean_conv:
	class_remove_file(attr_class, &class_attr_conv);

clean_class:
	class_destroy(attr_class);
	return ret;
}

static void __exit up_conv_exit(void)
{
	class_remove_file(attr_class, &class_attr_stat);
	class_remove_file(attr_class, &class_attr_conv);
	class_destroy(attr_class);

	pr_info("low_conv: module release!\n");
}

module_init(up_conv_init);
module_exit(up_conv_exit);
