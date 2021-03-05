// SPDX-License-Identifier: MIT and GPL
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

MODULE_DESCRIPTION("Basic module demo: procfs");
MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("AlexShlikhta <oleksandr.shlikhta@globallogic.com>");
MODULE_VERSION("0.1");


#define MODULE_TAG                              "example_module "
#define PROC_DIRECTORY                          "task_05"
#define TEXT_BUF_FILENAME                       "text"
#define TOTAL_CALLS_FILENAME                    "total_calls"
#define TOTAL_CHARACTERS_PROCESSED_FILENAME     "total_characters_processed"
#define CHARACTERS_CONVERTED_FILENAME           "characters_converted"


static char *text_buf;
static uint32_t total_calls;
static uint32_t total_characters_processed;
static uint32_t characters_converted;

static struct proc_dir_entry *mod_dir;
static struct proc_dir_entry *text_buf_file;
static struct proc_dir_entry *total_calls_file;
static struct proc_dir_entry *total_characters_processed_file;
static struct proc_dir_entry *characters_converted_file;

static ssize_t text_buf_read(struct file *file_p, char __user *buffer, size_t length, loff_t *offset);
static int text_buf_write(struct file *file_p, const char __user *buffer, size_t length, loff_t *offset);

static struct proc_ops text_buf_fops = {
	.proc_read  = text_buf_read,
	.proc_write = text_buf_write,
};


static int total_calls_read(struct file *file_p, char __user *buffer, size_t length, loff_t *offset);

static struct proc_ops total_calls_fops = {
	.proc_read  = total_calls_read,
};


static int total_characters_processed_read(struct file *file_p, char __user *buffer, size_t length, loff_t *offset);

static struct proc_ops total_characters_processed_fops = {
	.proc_read  = total_characters_processed_read,
};


static int characters_converted_read(struct file *file_p, char __user *buffer, size_t length, loff_t *offset);

static struct proc_ops characters_converted_fops = {
	.proc_read  = characters_converted_read,
};


#define LOW_UP_SHIFT    ('a' - 'A')

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
	char *res = kzalloc(sizeof(*res) * L, GFP_KERNEL);

	characters_converted = 0;

	for (i = 0; i < L; i++) {
		if (shift_dir == UP_TO_LOW)
			res[i] = up2low_char(in[i]);
		else
			res[i] = low2up_char(in[i]);

		if (res[i] != in[i])
			characters_converted++;
	}

	return res;
}


static int _create_proc(void)
{
	mod_dir = proc_mkdir(PROC_DIRECTORY, NULL);
	if (mod_dir == NULL)
		return -EFAULT;

	text_buf_file = proc_create(TEXT_BUF_FILENAME, 0664, mod_dir, &text_buf_fops);
	if (text_buf_file == NULL)
		return -EFAULT;

	total_calls_file = proc_create(TOTAL_CALLS_FILENAME, 0444, mod_dir, &total_calls_fops);
	if (total_calls_file == NULL)
		return -EFAULT;

	total_characters_processed_file = proc_create(TOTAL_CHARACTERS_PROCESSED_FILENAME, 0444, mod_dir, &total_characters_processed_fops);
	if (total_characters_processed_file == NULL)
		return -EFAULT;

	characters_converted_file = proc_create(CHARACTERS_CONVERTED_FILENAME, 0444, mod_dir, &characters_converted_fops);
	if (characters_converted_file == NULL)
		return -EFAULT;

	return 0;
}


static void _cleanup_proc(void)
{
	if (characters_converted_file) {
		remove_proc_entry(CHARACTERS_CONVERTED_FILENAME, mod_dir);
		characters_converted_file = NULL;
	}

	if (total_characters_processed_file) {
		remove_proc_entry(TOTAL_CHARACTERS_PROCESSED_FILENAME, mod_dir);
		total_characters_processed_file = NULL;
	}

	if (total_calls_file) {
		remove_proc_entry(TOTAL_CALLS_FILENAME, mod_dir);
		total_calls_file = NULL;
	}

	if (text_buf_file) {
		remove_proc_entry(TEXT_BUF_FILENAME, mod_dir);
		text_buf_file = NULL;
	}

	if (mod_dir) {
		remove_proc_entry(PROC_DIRECTORY, NULL);
		mod_dir = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////

static ssize_t text_buf_read(struct file *file_p, char __user *buffer, size_t length, loff_t *offset)
{
	return simple_read_from_buffer(buffer, length, offset, text_buf, strlen(text_buf));
}

///////////////////////////////////////////////////////////////////////////////

static int text_buf_write(struct file *file_p, const char __user *buffer, size_t length, loff_t *offset)
{
	size_t left;

	text_buf = kzalloc(sizeof(*text_buf) * 100, GFP_KERNEL);
	left = copy_from_user(text_buf, buffer, 100);

	if (left)
		pr_err("failed to write %u from %u chars\n", left, length);
	else
		pr_info("Word '%s' has converted to UPPER case successfuly\n", text_buf);

	text_buf = change_case(text_buf, LOW_TO_UP);

	total_characters_processed += characters_converted;
	total_calls++;

	return length;
}

///////////////////////////////////////////////////////////////////////////////

static int total_calls_read(struct file *file_p, char __user *buffer, size_t length, loff_t *offset)
{
	char tmp[16];

	sprintf(tmp, "%d\n", total_calls);

	return simple_read_from_buffer(buffer, length, offset, tmp, strlen(tmp));
}

///////////////////////////////////////////////////////////////////////////////

static int total_characters_processed_read(struct file *file_p, char __user *buffer, size_t length, loff_t *offset)
{
	char tmp[16];

	sprintf(tmp, "%d\n", total_characters_processed);

	return simple_read_from_buffer(buffer, length, offset, tmp, strlen(tmp));
}

///////////////////////////////////////////////////////////////////////////////

static int characters_converted_read(struct file *file_p, char __user *buffer, size_t length, loff_t *offset)
{
	char tmp[16];

	sprintf(tmp, "%d\n", characters_converted);

	return simple_read_from_buffer(buffer, length, offset, tmp, strlen(tmp));
}

///////////////////////////////////////////////////////////////////////////////
//###########################INIT CALLBACK#####################################
///////////////////////////////////////////////////////////////////////////////

static int __init mod_init(void)
{
	int err;

	text_buf = kstrdup("Buffer is empty:(\n", GFP_KERNEL);

	err = _create_proc();
	if (err)
		goto error;

	pr_info("'task_05' module initialized\n");
	return 0;

error:
	pr_err("'task_05' module failed to load\n");
	_cleanup_proc();
	return err;
}

///////////////////////////////////////////////////////////////////////////////
//###########################EXIT CALLBACK#####################################
///////////////////////////////////////////////////////////////////////////////

static void __exit mod_exit(void)
{
	_cleanup_proc();

	if (text_buf != NULL)
		kfree(text_buf);

	pr_info("Goodbye:)\n");
}


module_init(mod_init);
module_exit(mod_exit);
