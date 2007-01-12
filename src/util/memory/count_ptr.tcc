/**
	\file "util/memory/count_ptr.tcc"
	This file needs to be separate because sometimes the 
	type (T) of a count_ptr isn't complete until later -- 
	compiles should complain about destructor of incomplete type.  

	$Id: count_ptr.tcc,v 1.6.74.1 2007/01/12 00:40:46 fang Exp $
 */

#ifndef	__UTIL_MEMORY_COUNT_PTR_TCC__
#define	__UTIL_MEMORY_COUNT_PTR_TCC__
#include "util/memory/count_ptr.h"
#include "util/memory/deallocation_policy_fwd.h"

#ifndef	EXTERN_TEMPLATE_UTIL_MEMORY_COUNT_PTR

#ifdef	EXCLUDE_DEPENDENT_TEMPLATES_UTIL_MEMORY_COUNT_PTR
#define	EXTERN_TEMPLATE_UTIL_MEMORY_CHUNK_MAP_POOL
#endif

#if USE_REF_COUNT_POOL
// only include header, not definition!
#include "util/memory/chunk_map_pool.h"
#endif

//=============================================================================
// debugging stuff

namespace util {
namespace memory {
//=============================================================================
COUNT_PTR_TEMPLATE_SIGNATURE
// inline
COUNT_PTR_CLASS::count_ptr(T* p) : ptr(p) {
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
COUNT_PTR_TEMPLATE_SIGNATURE
// inline
COUNT_PTR_CLASS::count_ptr(T* p, size_t* c) : ptr(p), ref_count(p ? c : NULL) {
	if (p) {
		STATIC_RC_POOL_REF_INIT;
		COUNT_PTR_FAST_INVARIANT(c);	// counter must be valid
		VALIDATE_SIZE_T(c);
		(*c)++;			// increment here
		REASONABLE_REFERENCE_COUNT;
	} else {
		COUNT_PTR_FAST_INVARIANT(!ref_count);	// counter must also be NULL
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
COUNT_PTR_TEMPLATE_SIGNATURE
// inline
COUNT_PTR_CLASS::count_ptr(const raw_count_ptr<T,Dealloc>& r) :
		ptr(r.ptr), ref_count(r.ref_count) {
	if (this->ref_count) {
		STATIC_RC_POOL_REF_INIT;
		VALIDATE_SIZE_T(this->ref_count);
		(*this->ref_count)++;
		COUNT_PTR_FAST_INVARIANT(ptr);
		REASONABLE_REFERENCE_COUNT;
	} else {
		COUNT_PTR_FAST_INVARIANT(!ptr);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
COUNT_PTR_TEMPLATE_SIGNATURE
template <class S>
// inline
COUNT_PTR_CLASS::count_ptr(const raw_count_ptr<S,Dealloc>& r) :
		ptr(r.ptr), ref_count(r.ref_count) {
	if (this->ref_count) {
		STATIC_RC_POOL_REF_INIT;
		VALIDATE_SIZE_T(this->ref_count);
		(*this->ref_count)++;
		COUNT_PTR_FAST_INVARIANT(ptr);
		REASONABLE_REFERENCE_COUNT;
	} else {
		COUNT_PTR_FAST_INVARIANT(!ptr);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Overkill safety checks.  
 */
COUNT_PTR_TEMPLATE_SIGNATURE
// inline
typename COUNT_PTR_CLASS::pointer
COUNT_PTR_CLASS::release(void) {
	T* ret = ptr;
	if(this->ref_count) {
		COUNT_PTR_FAST_INVARIANT(*this->ref_count);
		(*this->ref_count)--;
		REASONABLE_REFERENCE_COUNT;
		if (!*this->ref_count) {
			STATIC_RC_POOL_REF_INIT;
			DELETE_SIZE_T(this->ref_count);
			COUNT_PTR_FAST_INVARIANT(ptr);
			// delete ptr;
			deallocation_policy()(ptr);
		}
#if 1
		ptr = NULL;
		this->ref_count = NULL;
#endif
	} else {
		COUNT_PTR_FAST_INVARIANT(!ptr);
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
COUNT_PTR_TEMPLATE_SIGNATURE
// inline
void
COUNT_PTR_CLASS::reset(T* p, size_t* c) {
	if (ptr == p) {
		// true whether or not ptr == NULL
		COUNT_PTR_FAST_INVARIANT(this->ref_count == c);
		// no need to decrement count, both source and
		// destination are alive.  
		return;
	} else {
#if 0
	if(this->ref_count) {
		COUNT_PTR_FAST_INVARIANT(*this->ref_count);
		(*this->ref_count)--;
		REASONABLE_REFERENCE_COUNT;
		if (!*this->ref_count) {
			STATIC_RC_POOL_REF_INIT;
			DELETE_SIZE_T(this->ref_count);
			COUNT_PTR_FAST_INVARIANT(ptr);
			// delete ptr;
			deallocation_policy()(ptr);
		}
	} else {
		COUNT_PTR_FAST_INVARIANT(!ptr);
	}
#else
	this->release();
#endif
	ptr = p;
	this->ref_count = (ptr) ? c : NULL;
	if (ptr && this->ref_count) {
		(*this->ref_count)++;
		REASONABLE_REFERENCE_COUNT;
	}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns a naked pointer only if this is the final reference
	to the object, otherwise returns NULL.  
 */
COUNT_PTR_TEMPLATE_SIGNATURE
typename COUNT_PTR_CLASS::pointer
COUNT_PTR_CLASS::exclusive_release(void) {
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

#endif	// EXTERN_TEMPLATE_UTIL_MEMORY_COUNT_PTR
#endif	//	__UTIL_MEMORY_COUNT_PTR_TCC__

