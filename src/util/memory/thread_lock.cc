/**
	\file "util/memory/thread_lock.cc"
	Thread lock implementation for allocators.  

	$Id: thread_lock.cc,v 1.5 2005/09/04 21:15:09 fang Exp $
 */

#ifndef	__UTIL_MEMORY_THREAD_LOCK_CC__
#define	__UTIL_MEMORY_THREAD_LOCK_CC__

#define	VERBOSE_LOCK			0

#if VERBOSE_LOCK
#include <iostream>
#endif
#include "util/macros.h"
#include "util/memory/thread_lock.hh"

namespace util {
namespace memory {
#if VERBOSE_LOCK
#include "util/using_ostream.hh"
#endif

//-----------------------------------------------------------------------------

#if VERBOSE_LOCK
struct pool_thread_lock<true>::acquire_message {
	explicit
	acquire_message(const mutex_type* const addr) {
		cerr << "getting lock at " << addr << "... ";
	}

	~acquire_message() { cerr << "got it." << endl; }
} __ATTRIBUTE_UNUSED__ ;

struct pool_thread_lock<true>::release_message {
	release_message() { cerr << "releasing lock... "; }
	~release_message() { cerr << "released." << endl; }
} __ATTRIBUTE_UNUSED__ ;
#else
struct pool_thread_lock<true>::acquire_message {
	acquire_message(mutex_type*) { }
} __ATTRIBUTE_UNUSED__ ;
struct pool_thread_lock<true>::release_message {
	release_message() { }
} __ATTRIBUTE_UNUSED__ ;
#endif

pool_thread_lock<true>::pool_thread_lock(mutex_type* const m) : the_mutex(m) {
#if THREADED_ALLOC
	INVARIANT(m);
	const acquire_message msg(the_mutex);
	// NDEBUG WILL DISABLE THIS! FIX ME!
	assert(!pthread_mutex_lock(the_mutex));
#endif
}

pool_thread_lock<true>::~pool_thread_lock() {
#if THREADED_ALLOC
	const release_message msg;
	// NDEBUG WILL DISABLE THIS! FIX ME!
	assert(!pthread_mutex_unlock(the_mutex));
#endif
}

#if 0
template struct pool_thread_lock<true>::acquire_message;
template struct pool_thread_lock<true>::release_message;
#endif

//=============================================================================
}	// end namespace memory
}	// end namespace util

#endif	//	__UTIL_MEMORY_THREAD_LOCK_CC__

