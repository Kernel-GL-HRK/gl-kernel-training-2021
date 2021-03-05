// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include "debug_comm.h"

/* 'stat' body structure */
static struct convert_stat stat[GL_FS_NUM];

/* lists for storage input strings */
static LIST_HEAD(lowercase_list);
static LIST_HEAD(uppercase_list);

static inline char gl_char_to_uppercase(char ch)
{
	return ((ch >= 'a' && ch <= 'z') ? ch - ' ' : ch);
}

static inline char gl_char_to_lowercase(char ch)
{
	return ((ch >= 'A' && ch <= 'Z') ? ch + ' ' : ch);
}

static unsigned int gl_str_to_case(gl_char_case char_case, char *str)
{
	char ch;
	unsigned int symb_converted = 0;

	while (*str) {
		if (char_case == GL_LOWERCASE)
			ch = gl_char_to_lowercase(*str);
		else
			ch = gl_char_to_uppercase(*str);

		if (ch != *str) {
			*str = ch;
			symb_converted++;
		}
		str++;
	}

	return symb_converted;
}

static struct convert_stat *gl_comm_get_stat(gl_fs_type fstype)
{
	switch (fstype) {
	case GL_PROCFS:
	case GL_SYSFS:
		return &stat[fstype];
	break;
	default:
		return NULL;
	break;
	}
}

static struct list_head *gl_comm_get_list(gl_fs_type fstype)
{
	switch (fstype) {
	case GL_PROCFS:
		return &uppercase_list;
	case GL_SYSFS:
		return &lowercase_list;
	break;
	default:
		return NULL;
	break;
	}
}

ssize_t gl_comm_main_read(gl_fs_type fstype, char *buf, size_t len)
{
	ssize_t _len;
	struct convert_node *_node;
	struct convert_stat *_stat;
	struct list_head *_list;

	/* get appropriate list */
	_list = gl_comm_get_list(fstype);
	if (!_list)
		return -EINVAL;

	/* get appropriate stat struct */
	_stat = gl_comm_get_stat(fstype);
	if (!_stat)
		return -EINVAL;

	/* update stat */
	_stat->total_calls++;

	/* empty list, do nothing */
	if (list_empty(_list))
		return 0;

	/* get last uppercase string from the list */
	_node = list_first_entry(_list, struct convert_node, head);

	/* user cannot handle our buffer size? */
	if (_node->size > len)
		return 0;

	/* fill the buffer */
	memcpy(buf, _node->data, _node->size);

	/* remove node from list and do memory free */
	_len = _node->size;
	__list_del_entry(&_node->head);
	kfree(_node->data);
	kfree(_node);

	return _len; /* return the buffer size */
}

ssize_t gl_comm_main_write(gl_fs_type fstype, const char *buf, size_t len)
{
	struct convert_node *_node;
	struct convert_stat *_stat;
	struct list_head *_list;

	/* get appropriate list */
	_list = gl_comm_get_list(fstype);
	if (!_list)
		return -EINVAL;

	/* get appropriate stat struct */
	_stat = gl_comm_get_stat(fstype);
	if (!_stat)
		return -EINVAL;

	/* update stat */
	_stat->total_calls++;

	_node = kmalloc(sizeof(struct convert_node), GFP_KERNEL);
	if (!_node)
		return -ENOMEM;

	/* allocate string buffer inluding endline character */
	_node->size = len;
	_node->data = kmalloc(_node->size, GFP_KERNEL);
	if (!_node) {
		kfree(_node);
		return -ENOMEM;
	}

	/* copy data to buffer */
	memcpy(_node->data, buf, len);

	/* make sure we have null terminated string */
	_node->data[len - 1] = '\0';

	/* convert string to uppercase and update stat */
	if (fstype == GL_PROCFS)
		_stat->converted += gl_str_to_case(GL_UPPERCASE, _node->data);
	else
		_stat->converted += gl_str_to_case(GL_LOWERCASE, _node->data);
	_stat->processed += len - 1; /* -1 for null terminator */

	/* add converted string to the list */
	list_add_tail(&_node->head, _list);

	return _node->size;
}

ssize_t gl_comm_stat_read(gl_fs_type fstype, char *buf, size_t buf_size)
{
	char *_buf = buf;
	struct convert_stat *_stat;

	_stat = gl_comm_get_stat(fstype);
	if (!_stat)
		return -EINVAL;

	if (buf_size < GL_COMM_MIN_STAT_BUFFER_LEN)
		return -ENOMEM;

	_buf += sprintf(_buf, "total_calls\t%u\n", _stat->total_calls);
	_buf += sprintf(_buf, "symb_processed\t%u\n", _stat->processed);
	_buf += sprintf(_buf, "symb_converted\t%u\n", _stat->converted);

	return _buf - buf; /* return buf length */
}
