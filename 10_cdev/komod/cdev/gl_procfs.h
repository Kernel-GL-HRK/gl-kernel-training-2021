/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __GL_PROCFS_H__
#define __GL_PROCFS_H__

#include <linux/proc_fs.h>
#include <linux/uaccess.h>

#define GL_PROCFS_DIR "gl_cdev" /* folder name in /proc dir */
#define GL_PROCFS_BUF_SIZE "buffer_size"
#define GL_PROCFS_BUF_VOL "buffer_volume"

int gl_procfs_init(size_t size, size_t *volume);

void gl_procfs_deinit(void);

#endif /* __GL_PROCFS_H__ */
