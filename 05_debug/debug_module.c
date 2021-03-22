// SPDX-License-Identifier: GPL-3.0
/**/

#define pr_fmt(fmt) "%s: " fmt,  KBUILD_MODNAME
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/ctype.h>

#define LEN_STR 160

static char buf_str[LEN_STR + 1];
static int showcalls;
static int storecalls;
static int converted;
static int processed;

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
	pr_debug("loading started\n");

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
	kobject_put(lowercase_kobj);
	pr_info("module exited\n");
}

module_init(caseconvertor_init);
module_exit(caseconvertor_exit);

MODULE_AUTHOR("Malakhova.Maryna <maryna.malakhova@globallogic.com>");
MODULE_DESCRIPTION("Case convertor module");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
