/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __GL_TIME_H__
#define __GL_TIME_H__

#include <linux/types.h>
#include <linux/ktime.h>
#include <linux/math64.h>

#define TMAX(a, b) gl_max_time(a, b)
#define TMIN(a, b) gl_min_time(a, b)
#define TSUM(a, b) gl_sum_time(a, b)
#define TDIV(a, b) gl_div_time(a, b)

#define TIME_PR_TEMPLATE "\t%5u.%03u us"
#define TIME_MEASURE_PRECISION NSEC_PER_SEC

#define TIME_MEASURE(expr) \
	do { \
		_ts = ktime_get_ns(); \
		{ \
			expr; \
		} \
		_ts = ktime_get_ns() - _ts; \
		_tm = gl_get_time(_ts); \
	} while (0)

#define TIME_MEASURED(precision) \
	( \
		({ \
			_tm.tm_nsec = div_u64(_tm.tm_nsec, \
				div_u64(NSEC_PER_SEC, precision)); \
			_tm.tm_sec = ((_tm.tm_sec > 0) ? div_u64(_tm.tm_sec, \
				div_u64(1, precision)) : 0); \
			if (_tm.tm_nsec >= 1000) { \
				u32 thousands = (u32)_tm.tm_nsec / 1000; \
				_tm.tm_nsec = (u32)_tm.tm_nsec % 1000; \
				_tm.tm_sec += thousands; \
			} \
		}), \
		_tm \
	)

struct gl_time {
	u32 tm_sec;
	u32 tm_nsec;
};

static inline struct gl_time gl_get_time(u64 timestamp)
{
	struct gl_time tm;
	u64 reminder = 0;

	/* separate seconds from nanoseconds */
	tm.tm_sec = div64_u64_rem(timestamp, NSEC_PER_SEC, &reminder);
	tm.tm_nsec = (u32)reminder;

	return tm;
}

static inline struct gl_time gl_max_time(struct gl_time a, struct gl_time b)
{
	if (a.tm_sec > b.tm_sec)
		return a;
	else if (a.tm_sec < b.tm_sec)
		return b;

	if (a.tm_nsec > b.tm_nsec)
		return a;
	else if (a.tm_nsec < b.tm_nsec)
		return b;

	return a;
}

static inline struct gl_time gl_min_time(struct gl_time a, struct gl_time b)
{
	if (b.tm_sec > a.tm_sec)
		return a;
	else if (b.tm_sec < a.tm_sec)
		return b;

	if (b.tm_nsec > a.tm_nsec)
		return a;
	else if (b.tm_nsec < a.tm_nsec)
		return b;

	return a;
}

static inline struct gl_time gl_sum_time(struct gl_time a, struct gl_time b)
{
	a.tm_sec += b.tm_sec;
	a.tm_nsec += b.tm_nsec;

	return a;
}

static inline struct gl_time gl_div_time(struct gl_time tm, u32 divider)
{
	tm.tm_nsec += (tm.tm_sec * 1000);
	tm.tm_nsec = (u32)tm.tm_nsec / divider;

	tm.tm_sec = 0;
	if (tm.tm_nsec >= 1000) {
		u32 thousands = (u32)tm.tm_nsec / 1000;

		tm.tm_nsec = (u32)tm.tm_nsec % 1000;
		tm.tm_sec += thousands;
	}

	return tm;
}

#endif /* __GL_TIME_H__ */
