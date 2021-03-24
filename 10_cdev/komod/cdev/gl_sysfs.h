/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __GL_SYSFS_H__
#define __GL_SYSFS_H__

#include <linux/kobject.h>

#define GL_SYSFS_DIR "gl_cdev" /* folder name in /sys dir */
#define GL_SYSFS_CLEAN buffer_clean
#define GL_SYSFS_DEBUG debug

int gl_sysfs_init(size_t *read, size_t *written);

void gl_sysfs_deinit(void);


extern int gl_clean_buffer(void);

#endif /* __GL_SYSFS_H__ */
