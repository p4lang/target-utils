/*
 * porting.h
 */

#ifndef _lub_porting_h
#define _lub_porting_h

#include <target-sys/bf_sal/bf_sys_mem.h>
#include <target-sys/bf_sal/bf_sys_sem.h>
#include <target-sys/bf_sal/bf_sys_str.h>
#include <target-sys/bf_sal/bf_sys_thread.h>

#define lub_malloc bf_sys_malloc
#define lub_calloc bf_sys_calloc
#define lub_realloc bf_sys_realloc
#define lub_free bf_sys_free
#define lub_strdup bf_sys_strdup

#if !defined(lub_malloc)
#define lub_malloc malloc
#endif

#if !defined(lub_calloc)
#define lub_calloc calloc
#endif

#if !defined(lub_realloc)
#define lub_realloc realloc
#endif

#if !defined(lub_free)
#define lub_free free
#endif

#if !defined(lub_strdup)
#define lub_strdup strdup
#endif

#endif				/* _lub_porting_h */
