/**
	\file "memory/thread_lock.cc"
	Thread lock implementation for allocators.  

	$Id: thread_lock.cc,v 1.1.4.1 2005/01/27 23:36:16 fang Exp $
 */

#ifndef	__UTIL_MEMORY_THREAD_LOCK_CC__
#define	__UTIL_MEMORY_THREAD_LOCK_CC__

#define	VERBOSE_LOCK			0

#if VERBOSE_LOCK
#include <iostream>
#endif
#include "memory/thread_lock.h"

namespace util {
namespace memory {
#if VERBOSE_LOCK
#include "using_ostream.h"
#endif

//-----------------------------------------------------------------------------

#if VERBOSE_LOCK
struct pool_thread_lock<true>::acquire_message {
	explicit
	acquire_message(const mutex_type* const addr) {
		cerr << "getting lock at " << addr << "... ";
	}

	~acquire_message() { cerr << "got it." << endl; }
};

struct pool_thread_lock<true>::release_message {
	release_message() { cerr << "releasing lock... "; }
	~release_message() { cerr << "released." << endl; }
}
#else
struct pool_thread_lock<true>::acquire_message {
	acquire_message(mutex_type*) { }
};
struct pool_thread_lock<true>::release_message { };
#endif

pool_thread_lock<true>::pool_thread_lock(mutex_type* const m) :
		the_mutex(m) {
#if THREADED_ALLOC
	INVARIANT(m);
	acquire_message msg(the_mutex);
	assert(!pthread_mutex_lock(the_mutex));
#endif
}

pool_thread_lock<true>::~pool_thread_lock() {
#if THREADED_ALLOC
	release_message msg;
	assert(!pthread_mutex_unlock(the_mutex));
#endif
}

//=============================================================================
}	// end namespace memory
}	// end namespace util

#endif	//	__UTIL_MEMORY_THREAD_LOCK_CC__

