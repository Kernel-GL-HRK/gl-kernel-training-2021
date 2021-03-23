// SPDX-License-Identifier: GPL-3.0
/**/

#define pr_fmt(fmt) "%s: " fmt,  KBUILD_MODNAME
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/ctype.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#define DIR_NAME	"UPPERCASE_CONV"
#define UPPERSTR_NAME	"uppercase"
#define STAT_NAME	"stat"
#define LEN_STR 160

static char buf_str[LEN_STR + 1];
static int showcalls;
static int storecalls;
static int converted;
static int processed;

static char buf_msg[LEN_STR + 1];

static ssize_t uppercase_write(struct file *file,
	const char __user *pbuf, size_t count, loff_t *ppos)
{
	ssize_t ret;
	int i = 0;

	storecalls += 1;
	processed += count;
	memset(buf_msg, '\0', LEN_STR+1);
	ret = simple_write_to_buffer(buf_msg, LEN_STR, ppos, pbuf, count);
	for (i = 0; i < LEN_STR+1; i++)
		if (islower(buf_msg[i])) {
			converted += 1;
			buf_msg[i] = toupper(buf_msg[i]);
			}
	return ret;

}

static ssize_t uppercase_read(struct file *file,
	char __user *pbuf, size_t count, loff_t *ppos)
{
	ssize_t ret;

	showcalls += 1;
	ret = simple_read_from_buffer(pbuf, count, ppos,
		buf_msg, strlen(buf_msg));
	return ret;
}

static ssize_t stat_read(struct file *file, char __user *pbuf,
	size_t count, loff_t *ppos)
{
	char *buf;
	size_t len = 0;
	ssize_t ret;

	if (*ppos != 0)
		return 0;
	if (count < sizeof(buf))
		return -ENOSPC;
	buf = kzalloc(PAGE_SIZE, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;
	len += snprintf(buf + len, PAGE_SIZE - len,
		"TOTAL CALLS: \t\t %d\n", showcalls+storecalls);
	len += snprintf(buf + len, PAGE_SIZE - len,
		"PROCESSED CHARS: \t %d\n", processed);
	len += snprintf(buf + len, PAGE_SIZE - len,
		"CONVERTED CHARS: \t %d", converted);
	len += snprintf(buf + len, PAGE_SIZE - len, "\n");
	ret = simple_read_from_buffer(pbuf, len, ppos, buf, PAGE_SIZE);
	kfree(buf);
	return ret;
}

static const struct file_operations uppercase_ops = {
	.owner = THIS_MODULE,
	.read = uppercase_read,
	.write = uppercase_write,
};

static const struct file_operations stat_ops = {
	.owner = THIS_MODULE,
	.read = stat_read,
};

static struct proc_dir_entry *dir;
static struct proc_dir_entry *proc_str, *proc_stat;

static ssize_t lowercase_show(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
	showcalls += 1;
	return scnprintf(buf, PAGE_SIZE, "STRING \t\t %s", buf_str);
}

static ssize_t lowercase_store(struct kobject *kobj,
	struct kobj_attribute *attr, const char *buf, size_t count)
{
	int i = 0;

	storecalls += 1;
	processed += count;
	memset(buf_str, '\0', LEN_STR+1);
	strncpy(buf_str, buf, count);
	for (i = 0; i < LEN_STR+1; i++)
		if (isupper(buf_str[i])) {
			converted += 1;
			buf_str[i] = tolower(buf_str[i]);
			}
	pr_info("write %s", buf_str);
	return count;
}

static ssize_t stat_show(struct kobject *kobj, struct kobj_attribute *attr,
		      char *buf)
{
	int offset = 0;
	int res = 0;

	res = scnprintf(buf+offset, PAGE_SIZE-offset,
		"TOTAL CALLS: \t\t %d\n", showcalls+storecalls);
	offset += res;
	res = scnprintf(buf+offset, PAGE_SIZE-offset,
		"PROCESSED CHARS: \t %d\n", processed);
	offset += res;

	res = scnprintf(buf+offset, PAGE_SIZE-offset,
		"CONVERTED CHARS: \t %d\n", converted);
	offset += res;
	return offset;
}

static struct kobj_attribute lowercase_attribute =
	__ATTR(lowercase, 0664, lowercase_show, lowercase_store);

static struct kobj_attribute stat_attribute =
	__ATTR(stat, 0444, stat_show, NULL);

static struct attribute *attrs[] = {
	&lowercase_attribute.attr,
	&stat_attribute.attr,
	NULL,
};

static struct attribute_group attr_group = {
	.attrs = attrs,
};

static struct kobject *lowercase_kobj;

static int __init caseconvertor_init(void)
{
	int retval;

	memset(buf_str, '\0', LEN_STR+1);
	memset(buf_msg, '\0', LEN_STR+1);

	pr_debug("loading started\n");

	dir = proc_mkdir(DIR_NAME, NULL);
	if (dir == NULL) {
		pr_err("error creating procfs directory\n");
		return -ENOMEM;
	}

	proc_str = proc_create(UPPERSTR_NAME, 0664, dir, &uppercase_ops);
	if (proc_str == NULL) {
		pr_err("error creating procfs uppercase str\n");
		remove_proc_entry(DIR_NAME, NULL);
		return -ENOMEM;
	}

	proc_stat = proc_create(STAT_NAME, 0444, dir, &stat_ops);
	if (proc_stat == NULL) {
		pr_err("error creating procfs stat\n");
		remove_proc_entry(STAT_NAME, dir);
		remove_proc_entry(DIR_NAME, NULL);
		return -ENOMEM;
	}

	pr_info("module loaded\n");

	lowercase_kobj = kobject_create_and_add("LOWERCASE_CONV", kernel_kobj);
	if (!lowercase_kobj)
		return -ENOMEM;
	retval = sysfs_create_group(lowercase_kobj, &attr_group);
	if (retval)
		kobject_put(lowercase_kobj);

	return retval;
}

static void __exit caseconvertor_exit(void)
{
	proc_remove(dir);
	kobject_put(lowercase_kobj);
	pr_info("module exited\n");
}

module_init(caseconvertor_init);
module_exit(caseconvertor_exit);

MODULE_AUTHOR("Malakhova.Maryna <maryna.malakhova@globallogic.com>");
MODULE_DESCRIPTION("Case convertor module");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
