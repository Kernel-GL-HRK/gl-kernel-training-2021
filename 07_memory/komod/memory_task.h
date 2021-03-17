/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __MEMORY_TASK_H__
#define __MEMORY_TASK_H__

#include <linux/types.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>

#include "gl_time.h"

enum gl_allocator {
	_KMALLOC = 0,
	_KZMALLOC,
	_VMALLOC,
	_GET_FREE_PAGES,
	_ALLOCATOR_CNT
};

extern size_t __gl_free_pages_size;

static inline char *gl_get_allocator_name(enum gl_allocator allocator)
{
	switch (allocator) {
	case _KMALLOC:
		return "kmalloc";
	case _KZMALLOC:
		return "kzalloc";
	case _VMALLOC:
		return "vmalloc";
	case _GET_FREE_PAGES:
		return "get_free_pages";
	default:
		return NULL;
	}
}

static inline void *gl_alloc(enum gl_allocator allocator, size_t size)
{
	switch (allocator) {
	case _KMALLOC:
		return kmalloc(size, GFP_KERNEL);
	case _KZMALLOC:
		return kzalloc(size, GFP_KERNEL);
	case _VMALLOC:
		return vmalloc(size);
	case _GET_FREE_PAGES:
		__gl_free_pages_size = size;
		return alloc_pages_exact(size, GFP_KERNEL);
	default:
		return NULL;
	}
}

static inline void gl_dealloc(enum gl_allocator allocator, void *_ptr)
{
	switch (allocator) {
	case _KMALLOC:
		kfree(_ptr);
	break;
	case _KZMALLOC:
		kzfree(_ptr);
	break;
	case _VMALLOC:
		vfree(_ptr);
	break;
	case _GET_FREE_PAGES:
		free_pages_exact(_ptr, __gl_free_pages_size);
	default:
	break;
	}
}

#endif /* __MEMORY_TASK_H__ */
