/**
	\file "memory/thread_lock.h"
	Thread locking.  

	$Id: thread_lock.h,v 1.1.4.1 2005/01/27 23:36:16 fang Exp $
 */

#ifndef	__UTIL_MEMORY_THREAD_LOCK_H__
#define	__UTIL_MEMORY_THREAD_LOCK_H__

#include "macros.h"
#include <pthread.h>

// because FreeBSD's <pthreads.h> defines initializer as NULL
// and I haven't figured out a workaround yet.
#if defined(__FreeBSD__)
// #warn	"This allocator will not be thread-safe in FreeBSD... yet."
#define	THREADED_ALLOC		0
#else
#define	THREADED_ALLOC		1
#endif

typedef	pthread_mutex_t		mutex_type;

namespace util {
namespace memory {
//=============================================================================
// Lock types

template <bool>
class pool_thread_lock;
// may need __attribute__ ((__unused__))

/**
	Null lock, not-a-lock.  
 */
template <>
class pool_thread_lock<false> {
public:
	explicit
	pool_thread_lock(mutex_type* const) { }

	~pool_thread_lock() { }
};

template <>
class pool_thread_lock<true> {
private:
	struct acquire_message;
	struct release_message;
	mutex_type* const	the_mutex;
public:
	explicit
	pool_thread_lock(mutex_type* const);

	~pool_thread_lock();
};

//=============================================================================
}	// end namespace memory
}	// end namespace util

#endif	// __UTIL_MEMORY_THREAD_LOCK_H__

