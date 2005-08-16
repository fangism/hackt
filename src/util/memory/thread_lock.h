/**
	\file "util/memory/thread_lock.h"
	Thread locking.  

	$Id: thread_lock.h,v 1.3.28.1 2005/08/16 03:50:32 fang Exp $
 */

#ifndef	__UTIL_MEMORY_THREAD_LOCK_H__
#define	__UTIL_MEMORY_THREAD_LOCK_H__

#include "util/pthread.h"
#include "util/attributes.h"

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

/**
	Null lock, not-a-lock.  
 */
template <>
class pool_thread_lock<false> {
public:
	explicit
	pool_thread_lock(mutex_type* const) { }

	~pool_thread_lock() { }
} __ATTRIBUTE_UNUSED__ ;

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
} __ATTRIBUTE_UNUSED__ ;

//=============================================================================
}	// end namespace memory
}	// end namespace util

#endif	// __UTIL_MEMORY_THREAD_LOCK_H__

