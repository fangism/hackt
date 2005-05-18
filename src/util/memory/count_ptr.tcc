/**
	\file "util/memory/count_ptr.tcc"
	This file needs to be separate because sometimes the 
	type (T) of a count_ptr isn't complete until later -- 
	compiles should complain about destructor of incomplete type.  

	$Id: count_ptr.tcc,v 1.1.2.2 2005/05/18 03:58:07 fang Exp $
 */

#ifndef	__UTIL_MEMORY_COUNT_PTR_TCC__
#define	__UTIL_MEMORY_COUNT_PTR_TCC__

#include "util/memory/count_ptr.h"

#if USE_REF_COUNT_POOL
#include "util/memory/chunk_map_pool.tcc"
#endif

//=============================================================================
// debugging stuff

namespace util {
namespace memory {
//=============================================================================
template <class T>
// inline
count_ptr<T>::count_ptr(T* p) : ptr(p) {
	if (ptr) {
		STATIC_RC_POOL_REF_INIT;
		ref_count = NEW_SIZE_T;
		*ref_count = 1;
	} else {
		ref_count = NULL;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	UNSAFE: constructor that corecively sets the 
	reference pointer and the count pointer 
	and increments the pointer.  
	The manager should initially allocate a counter set to zero.  
	The manager should not own the pointer or the counter.  
	Needed for transient pointer to object reconstruction.  
	Use this only if you know what you're doing.

	NOTE: if using REF_COUNT_POOL, then the pointer
	passed in MUST BE allocated by the count pool!!!

	If p is NULL, then count pointer is dropped.  
 */
template <class T>
// inline
count_ptr<T>::count_ptr(T* p, size_t* c) : ptr(p), ref_count(p ? c : NULL) {
	if (p) {
		STATIC_RC_POOL_REF_INIT;
		NEVER_NULL(c);		// counter must be valid
		VALIDATE_SIZE_T(c);
		(*c)++;			// increment here
		REASONABLE_REFERENCE_COUNT;
	} else {
		INVARIANT(!ref_count);	// counter must also be NULL
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
// inline
count_ptr<T>::count_ptr(const raw_count_ptr<T>& r) :
		ptr(r.ptr), ref_count(r.ref_count) {
	if (this->ref_count) {
		STATIC_RC_POOL_REF_INIT;
		VALIDATE_SIZE_T(this->ref_count);
		(*this->ref_count)++;
		NEVER_NULL(ptr);
		REASONABLE_REFERENCE_COUNT;
	} else {
		INVARIANT(!ptr);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
template <class S>
// inline
count_ptr<T>::count_ptr(const raw_count_ptr<S>& r) :
		ptr(r.ptr), ref_count(r.ref_count) {
	if (this->ref_count) {
		STATIC_RC_POOL_REF_INIT;
		VALIDATE_SIZE_T(this->ref_count);
		(*this->ref_count)++;
		NEVER_NULL(ptr);
		REASONABLE_REFERENCE_COUNT;
	} else {
		INVARIANT(!ptr);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Overkill safety checks.  
 */
template <class T>
// inline
typename count_ptr<T>::pointer
count_ptr<T>::release(void) {
	T* ret = ptr;
	if(this->ref_count) {
		INVARIANT(*this->ref_count);
		(*this->ref_count)--;
		REASONABLE_REFERENCE_COUNT;
		if (!*this->ref_count) {
			STATIC_RC_POOL_REF_INIT;
			DELETE_SIZE_T(this->ref_count);
			NEVER_NULL(ptr);
			delete ptr;
		}
		ptr = NULL;
		this->ref_count = NULL;
	} else {
		INVARIANT(!ptr);
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: need to check if assigning to self!
	NOTE: when using a pool of reference counts, it is critical 
		to delete the count FIRST before ptr because
		the reference-count-pool itself is reference-counted!!!
	TODO: write this more optimally
	\param p the new pointer, from some other count_ptr.
	\param c the corresponding reference count if p is valid, 
		else is ignored.  
 */
template <class T>
// inline
void
count_ptr<T>::reset(T* p, size_t* c) {
	if (ptr == p) {
		// true whether or not ptr == NULL
		INVARIANT(this->ref_count == c);
		// no need to decrement count, both source and
		// destination are alive.  
		return;
	}
	if(this->ref_count) {
		INVARIANT(*this->ref_count);
		(*this->ref_count)--;
		REASONABLE_REFERENCE_COUNT;
		if (!*this->ref_count) {
			STATIC_RC_POOL_REF_INIT;
			DELETE_SIZE_T(this->ref_count);
			NEVER_NULL(ptr);
			delete ptr;
		}
	} else {
		INVARIANT(!ptr);
	}
	ptr = p;
	this->ref_count = (ptr) ? c : NULL;
	if (ptr && this->ref_count) {
		(*this->ref_count)++;
		REASONABLE_REFERENCE_COUNT;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns a naked pointer only if this is the final reference
	to the object, otherwise returns NULL.  
 */
template <class T>
typename count_ptr<T>::pointer
count_ptr<T>::exclusive_release(void) {
	if (this->owned()) {
		STATIC_RC_POOL_REF_INIT;
		T* ret = ptr;
		ptr = NULL;
		DELETE_SIZE_T(this->ref_count);
		this->ref_count = NULL;
		return ret;
	} else
		return NULL;
}

//=============================================================================
}	// end namespace memory
}	// end namespace util

#endif	//	__UTIL_MEMORY_COUNT_PTR_TCC__

