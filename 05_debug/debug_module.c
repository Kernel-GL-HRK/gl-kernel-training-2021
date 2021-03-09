// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include <linux/ctype.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>

#include <linux/sysfs.h>
#include <linux/proc_fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#include <linux/types.h>
#include <linux/list.h>
#include <linux/slab.h>

MODULE_AUTHOR("Andrii Synenko");
MODULE_DESCRIPTION("List data module for Linux Kernel ProCamp");
MODULE_LICENSE("Dual BSD/GPL");

#define PROCFS_BUFF_SIZE 255
#define PROCFS_DIR_NAME	"debug_module"
#define PROCFS_ENT_NAME	"upper"
#define SYSFS_DIR_NAME	PROCFS_DIR_NAME
#define SYSFS_ENT_NAME	"lower"
#define STAT_ENT_NAME	"stat"

struct convertor_struct {
	char *str_buff;

	int converted_last;
	int converted_total;
	int processed_last;
	int processed_total;

};

static struct convertor_struct sysfs_conv;
static struct convertor_struct procfs_conv;

static int convert_case(bool to_lover, struct convertor_struct *conv,
						size_t str_size)
{
	size_t indx = 0;

	conv->converted_last = 0;

	if (conv == NULL || conv->str_buff == NULL) {
		pr_err("%s: convertor or buffer is NULL\n", __func__);
		return -EINVAL;
	}

	while (conv->str_buff[indx] != '\0' &&
		indx < str_size) {
		char ch = to_lover ? tolower(conv->str_buff[indx])
				   : toupper(conv->str_buff[indx]);

		if (ch != conv->str_buff[indx]) {
			conv->converted_last++;
			conv->str_buff[indx] = ch;
		}
		indx++;
	}
	conv->processed_last  = indx;

	conv->converted_total += conv->converted_last;
	conv->processed_total += conv->processed_last;

	return 0;
}

static int print_stat(char *buf, struct convertor_struct *conv)
{
	if (conv == NULL || buf == NULL) {
		pr_err("%s: convertor or buffer is NULL\n", __func__);
		return -EINVAL;
	}

	return sprintf(buf, "last processed/converted : %d/%d char(s)\n"
			"total processed/converted : %d/%d char(s)\n",
			conv->processed_last,
			conv->converted_last,
			conv->processed_total,
			conv->converted_total);
}


static ssize_t stat_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf)
{
	int res;
	ssize_t cursor = sprintf(buf, "lowercase convertion stat:\n");

	res = print_stat(&buf[cursor], &sysfs_conv);
	if (res < 0)
		return res;

	cursor += res;

	return cursor;
}

static ssize_t sysfs_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf)
{
	if (sysfs_conv.str_buff == NULL) {
		pr_err("%s: no buffer to output\n", __func__);
		return 0;
	}
	return sprintf(buf, "%s", sysfs_conv.str_buff);
}

static ssize_t sysfs_store(struct kobject *kobj, struct kobj_attribute *attr,
		const char *buf, size_t count)
{
	int res;

	kzfree(sysfs_conv.str_buff);

	sysfs_conv.str_buff = kstrdup(buf, GFP_KERNEL);

	res = convert_case(true, &sysfs_conv, count);
	if (res < 0)
		return res;

	return count;
}


static struct kobject *sysfs_kobject;
struct kobj_attribute kobj_attr_sysfs = {
	.attr = { .name = SYSFS_ENT_NAME, .mode = 0666 },
	.show	= sysfs_show,
	.store	= sysfs_store
};
struct kobj_attribute kobj_attr_stat = {
	.attr = { .name = STAT_ENT_NAME, .mode = 0444 },
	.show	= stat_show
};

int init_sysfs(void)
{
	int ret;

	sysfs_kobject = kobject_create_and_add(SYSFS_DIR_NAME, kernel_kobj);
	if (sysfs_kobject == NULL) {
		pr_err("%s: error creating kobject\n", __func__);
		return -ENOMEM;
	}

	ret = sysfs_create_file(sysfs_kobject, &kobj_attr_sysfs.attr);
	if (ret)
		goto failed_sysfs_init;

	ret = sysfs_create_file(sysfs_kobject, &kobj_attr_stat.attr);
	if (ret)
		goto failed_sysfs_init;


	memset(&sysfs_conv, 0, sizeof(sysfs_conv));

	pr_info("%s: success\n", __func__);
	return 0;

failed_sysfs_init:
	pr_err("%s: error creating sysfs class attribute\n", __func__);
	kobject_del(sysfs_kobject);
	return ret;
}

static ssize_t stat_read(struct file *file,
		char __user *pbuf, size_t count, loff_t *ppos)
{
	int res;
	char stat_buff[PROCFS_BUFF_SIZE];
	size_t to_copy = sprintf(stat_buff, "uppercase convertion stat:\n");

	res = print_stat(&stat_buff[to_copy], &procfs_conv);
	if (res < 0)
		return res;

	return simple_read_from_buffer(pbuf, count, ppos,
					stat_buff, strlen(stat_buff));
}

static ssize_t procfs_read(struct file *file, char __user *pbuf,
		size_t count, loff_t *ppos)
{
	if (procfs_conv.str_buff == NULL) {
		pr_err("%s: no buffer to output\n", __func__);
		return 0;
	}

	return simple_read_from_buffer(pbuf, count, ppos,
			procfs_conv.str_buff, strlen(procfs_conv.str_buff));

}

static ssize_t procfs_write(struct file *file, const char __user *pbuf,
		size_t count, loff_t *ppos)
{
	int res;
	ssize_t not_copied, copied;
	static char input_buf[PROCFS_BUFF_SIZE];

	kzfree(procfs_conv.str_buff);

	not_copied = copy_from_user(input_buf, pbuf, count);
	copied = count - not_copied;

	procfs_conv.str_buff = kstrdup(input_buf, GFP_KERNEL);

	res = convert_case(false, &procfs_conv, copied);
	if (res < 0)
		return res;

	return copied;
}

static struct proc_dir_entry *proc_dir;
static struct proc_dir_entry *ent_uppercase;
static const struct file_operations procfs_ops = {
	.owner = THIS_MODULE,
	.read = procfs_read,
	.write = procfs_write,
};
static const struct file_operations stat_ops = {
	.owner = THIS_MODULE,
	.read = stat_read
};

int init_procfs(void)
{
	proc_dir = proc_mkdir(PROCFS_DIR_NAME, NULL);
	if (proc_dir == NULL) {
		pr_err("%s: error creating procfs directory\n", __func__);
		return -ENOMEM;
	}

	ent_uppercase = proc_create(PROCFS_ENT_NAME, 0666,
					proc_dir, &procfs_ops);
	if (ent_uppercase == NULL)
		goto failed_procfs_init;

	ent_uppercase = proc_create(STAT_ENT_NAME, 0444, proc_dir, &stat_ops);
	if (ent_uppercase == NULL)
		goto failed_procfs_init;

	memset(&procfs_conv, 0, sizeof(procfs_conv));

	pr_info("%s: success\n", __func__);
	return 0;

failed_procfs_init:
	pr_err("%s: error creating procfs entry\n", __func__);
	proc_remove(proc_dir);
	return -ENOMEM;
}

static int debug_module_init(void)
{
	int ret;

	pr_info("%s: module starting\n",  __func__);

	ret = init_procfs();
	if (ret < 0)
		return ret;

	ret = init_sysfs();
	if (ret < 0)
		proc_remove(proc_dir);

	pr_info("%s: success\n", __func__);

	return ret;
}

static void debug_module_exit(void)
{
	pr_info("%s: module exit\n",  __func__);

	kzfree(procfs_conv.str_buff);
	kzfree(sysfs_conv.str_buff);

	kobject_del(sysfs_kobject);

	proc_remove(proc_dir);

	pr_info("%s: success\n", __func__);
}

module_init(debug_module_init);
module_exit(debug_module_exit);
