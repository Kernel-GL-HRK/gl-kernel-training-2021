/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __GL_DEBUG_COMM_H__
#define __GL_DEBUG_COMM_H__

#include <linux/list.h>
#include <linux/slab.h>

#define GL_COMM_MIN_STAT_BUFFER_LEN 128
#define GL_COMM_MAX_LIST_PAYLOAD_SIZE 256

enum gl_fs_type {
	GL_PROCFS = 0,
	GL_SYSFS,
	GL_FS_NUM
};

enum gl_char_case {
	GL_LOWERCASE = 0,
	GL_UPPERCASE,
};

struct convert_node {
	struct list_head head;
	char *data;
	ssize_t size; /* size of the buffer, not the string */
};

struct convert_stat {
	unsigned int total_calls;
	unsigned int processed;
	unsigned int converted;
};

ssize_t gl_comm_main_read(gl_fs_type fstype, char *buf, size_t len);

ssize_t gl_comm_main_write(gl_fs_type fstype, const char *buf, size_t len);

ssize_t gl_comm_stat_read(gl_fs_type fstype, char *buf, size_t buf_size);

#endif /* __GL_DEBUG_COMM_H__ */
