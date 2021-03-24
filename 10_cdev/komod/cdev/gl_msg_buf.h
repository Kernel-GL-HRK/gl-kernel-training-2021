/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __GL_MSG_BUF__
#define __GL_MSG_BUF__

#include <linux/slab.h>

#define MIN_BUFFER_SIZE 32
#define DEF_BUFFER_SIZE 1024

struct msg_buffer {
	size_t len;
	char *data;
};

extern struct msg_buffer msg_buffer;

static inline int gl_msg_buffer_init(size_t buffer_size)
{
	if (buffer_size < MIN_BUFFER_SIZE) {
		pr_err("GL buffer_size cannot be less than %d!\n",
			MIN_BUFFER_SIZE);
		return -EINVAL;
	}

	msg_buffer.data = kmalloc(buffer_size, GFP_KERNEL);
	if (msg_buffer.data == NULL)
		return -1;

	return 0;
}

static inline ssize_t gl_msg_buffer_read(char *buf, size_t len)
{
	int ret;

	if (msg_buffer.len == 0) {
		pr_warn("GL Buffer is empty!\n");
		return 0;
	}

	if (len > msg_buffer.len)
		len = msg_buffer.len;

	ret = copy_to_user(buf, msg_buffer.data, len);
	if (ret) {
		pr_err("GL Unable to copy to!\n");
		return -EFAULT;
	}

	return msg_buffer.len;
}


static inline ssize_t gl_msg_buffer_write(const char *buf, size_t len)
{
	int ret;

	if (msg_buffer.len >= DEF_BUFFER_SIZE - 1) {
		pr_warn("GL Buffer is full!\n");
		return 0;
	}

	if (msg_buffer.len + len > DEF_BUFFER_SIZE - 1)
		len = (DEF_BUFFER_SIZE - 1) - msg_buffer.len;

	ret = copy_from_user(&msg_buffer.data[msg_buffer.len], buf, len);
	if (ret != 0) {
		pr_err("GL Unable to copy from!\n");
		return -EFAULT;
	}

	msg_buffer.data[msg_buffer.len + len] = '\0';
	msg_buffer.len += len;

	return len;
}

static inline int gl_msg_buffer_clean(void)
{
	msg_buffer.len = 0;

	return 0;
}

static inline size_t *gl_get_msg_buffer_len(void)
{
	return &msg_buffer.len;
}

static inline int gl_msg_buffer_free(void)
{
	if (msg_buffer.data != NULL)
		kfree(msg_buffer.data);

	msg_buffer.len = 0;
	return 0;
}

#endif /* __GL_MSG_BUF__ */
