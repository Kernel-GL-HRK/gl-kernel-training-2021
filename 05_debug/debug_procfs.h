/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __GL_DEBUG_PROCFS_H__
#define __GL_DEBUG_PROCFS_H__

#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include "debug_comm.h"

#define GL_PROCFS_DIR "gl_debug" /* folder name in /proc dir */
#define GL_PROCFS_MAIN "uppercase"
#define GL_PROCFS_STAT "stat"
#define GL_PROCFS_STAT_BUF_LEN 128 /* it's enough */
#define GL_PROCFS_READ_BUF_LEN 256

int gl_procfs_init(void);

void gl_procfs_deinit(void);

#endif /* __GL_DEBUG_PROCFS_H__ */
