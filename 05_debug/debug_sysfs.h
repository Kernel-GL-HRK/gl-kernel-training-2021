/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __GL_DEBUG_SYSFS_H__
#define __GL_DEBUG_SYSFS_H__

#include <linux/kobject.h>
#include "debug_comm.h"

#define GL_SYSFS_DIR "gl_debug" /* folder name in /proc dir */
#define GL_SYSFS_MAIN lowercase
#define GL_SYSFS_STAT stat
#define GL_SYSFS_STAT_BUF_LEN 128 /* it's enough */

int gl_sysfs_init(void);

void gl_sysfs_deinit(void);

#endif /* __GL_DEBUG_SYSFS_H__ */
