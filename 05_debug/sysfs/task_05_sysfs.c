// SPDX-License-Identifier: MIT and GPL
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/string.h>

MODULE_DESCRIPTION("Basic module demo: sysfs");
MODULE_AUTHOR("AlexShlikhta");
MODULE_VERSION("0.1");
MODULE_LICENSE("Dual MIT/GPL"); // this affects the kernel behavior

#define CLASS_ATTR(_name, _mode, _show, _store) \
struct class_attribute class_attr_##_name = __ATTR(_name, _mode, _show, _store)

static struct class *mod_class;

static uint32_t total_calls;
static uint32_t total_characters_processed;
static uint32_t characters_converted;
static char *text_buf = "Bufer is empty:(";

#define LOW_UP_SHIFT	('a' - 'A')

enum low_up_t {
	LOW_TO_UP = 0,
	UP_TO_LOW
};

/**
 * @brief      Change case of one character from UPPER to lower.
 * If symbol is in the range from 'A' to 'Z' it's case will be changet,
 * in other way this func return the symbol
 *
 * @param[in]  in    Symbol from ASCII table
 *
 * @return     Symbol with changed or not case.
 */
char up2low_char(char in)
{
	if (in < 'A' || 'Z' < in)
		return in;

	return in + LOW_UP_SHIFT;
}

/**
 * @brief      Change case of one character from lower to UPPER.
 * If symbol is in the range from 'a' to 'z' it's case will be changet,
 * in other way this func return the symbol
 *
 * @param[in]  in    Symbol from ASCII table
 *
 * @return     Symbol with changed or not case.
 */
char low2up_char(char in)
{
	if (in < 'a' || 'z' < in)
		return in;

	return in - LOW_UP_SHIFT;
}

/**
 * @brief      Change case of string from UPPER case to lower, or backward.
 * The result depends on 'shift_dir' flag which is enum and have only 2 states.
 *
 * @param[in]  in         Input string with different cases
 * @param[in]  shift_dir  The shift dir
 *
 * @return     Whole string in the same case.
 */
char *change_case(const char *in, enum low_up_t shift_dir)
{
	uint16_t L = strlen(in);
	uint16_t i;

	char *res = kmalloc(sizeof(*res) * L, GFP_KERNEL);

	if (res == NULL) {
		pr_err("in func 'change_case' couldn't allocate enough memory:(\n");
		goto ret;
	}

	characters_converted = 0;

	for (i = 0; i < L; i++) {
		if (shift_dir == UP_TO_LOW)
			res[i] = up2low_char(in[i]);
		else
			res[i] = low2up_char(in[i]);

		if (res[i] != in[i])
			characters_converted++;
	}

	res[L] = '\0';

	ret:
	return res;
}


///////////////////////////////////////////////////////////////////////////////

/* sysfs show() method. Calls the show() method corresponding to the individual sysfs file */
static ssize_t text_buf_show(struct class *class, struct class_attribute *attr, char *buf)
{
	strcpy(buf, text_buf);
	strcat(buf, "\n");

	return strlen(buf);
}

///////////////////////////////////////////////////////////////////////////////

/* sysfs store() method. Calls the store() method corresponding to the individual sysfs file */
static ssize_t text_buf_store(struct class *class, struct class_attribute *attr, const char *buf, size_t count)
{
	text_buf = kstrdup(buf, GFP_KERNEL);
	text_buf = change_case(text_buf, LOW_TO_UP);

	total_characters_processed += characters_converted;

	pr_info("Word '%s' has converted to UPPER case successfuly\n", buf);

	total_calls++;

	return count;
}

///////////////////////////////////////////////////////////////////////////////

/* sysfs show() method. Calls the show() method corresponding to the individual sysfs file */
static ssize_t total_calls_show(struct class *class, struct class_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", total_calls);;
}

///////////////////////////////////////////////////////////////////////////////

/* sysfs show() method. Calls the show() method corresponding to the individual sysfs file */
static ssize_t total_characters_processed_show(struct class *class, struct class_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", total_characters_processed);
}

///////////////////////////////////////////////////////////////////////////////

/* sysfs show() method. Calls the show() method corresponding to the individual sysfs file */
static ssize_t characters_converted_show(struct class *class, struct class_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", characters_converted);
}

CLASS_ATTR(text, 0644, &text_buf_show, &text_buf_store);
CLASS_ATTR(total_calls, 0444, &total_calls_show, NULL);
CLASS_ATTR(total_characters_processed, 0444, &total_characters_processed_show, NULL);
CLASS_ATTR(characters_converted, 0444, &characters_converted_show, NULL);

///////////////////////////////////////////////////////////////////////////////
//###########################INIT CALLBACK#####################################
///////////////////////////////////////////////////////////////////////////////

int __init mod_init(void)
{
	int res;

	mod_class = class_create(THIS_MODULE, "task_05");
	if (IS_ERR(mod_class)) {
		pr_err("bad class create\n");
		res = -ENOMEM;
		goto err;
	}

	res = class_create_file(mod_class, &class_attr_text);
	if (IS_ERR_VALUE(res)) {
		pr_err("bad file 'text' create\n");
		goto err;
	}

	res = class_create_file(mod_class, &class_attr_total_calls);
	if (IS_ERR_VALUE(res)) {
		pr_err("bad file 'total_calls' create\n");
		goto err;
	}

	res = class_create_file(mod_class, &class_attr_total_characters_processed);
	if (IS_ERR_VALUE(res)) {
		pr_err("bad file 'total_characters_processed' create\n");
		goto err;
	}

	res = class_create_file(mod_class, &class_attr_characters_converted);
	if (IS_ERR_VALUE(res)) {
		pr_err("bad file 'characters_converted' create\n");
		goto err;
	}

	pr_info("'task_05' module initialized\n");

	err:
	return res;
}

///////////////////////////////////////////////////////////////////////////////
//###########################EXIT CALLBACK#####################################
///////////////////////////////////////////////////////////////////////////////

void mod_cleanup(void)
{
	class_remove_file(mod_class, &class_attr_characters_converted);
	class_remove_file(mod_class, &class_attr_total_characters_processed);
	class_remove_file(mod_class, &class_attr_total_calls);
	class_remove_file(mod_class, &class_attr_text);
	class_destroy(mod_class);

	pr_info("Goodbye:)\n");
}

module_init(mod_init);
module_exit(mod_cleanup);
