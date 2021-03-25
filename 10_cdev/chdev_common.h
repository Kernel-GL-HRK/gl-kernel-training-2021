#ifndef CHDEV_COMMON_H
#define CHDEV_COMMON_H

#define BUF_SIZE 32

extern int stat_calls, stat_readed, stat_writed, buf_len, wr_ptr, rd_ptr;

#if 0
struct chdev_ctrl_t {
	int stat_calls;
	int stat_readed;
	int stat_writed;
	int buf_len;
};
#endif

#endif
