/**
	\file "memory/count_ptr_gcc41death.h"
	Simple reference-count pointer class.  
	Do not mix with non-counted pointer types.  

	$Id: count_ptr_gcc41death.h,v 1.1.2.1 2005/04/26 00:13:11 fang Exp $

	TODO:
		* split into .tcc file
		* pool allocate size_t counts
 */

#ifndef	__UTIL_MEMORY_COUNT_PTR_GCC41DEATH_H__
#define	__UTIL_MEMORY_COUNT_PTR_GCC41DEATH_H__

// #include "macros.h"
// #include "memory/pointer_classes_fwd.h"
#include <cstddef>		// for size_t
#include "memory/pointer_traits_gcc41death.h"
// #include "memory/pointer_manipulator.h"

//=============================================================================
// debugging stuff

#define	REASONABLE_REFERENCE_COUNT

//=============================================================================

namespace util {
// forward declaration only
class persistent_object_manager;

namespace memory {
//=============================================================================
/***
	Reference-counting pointer class templates.

	Generally not intended for mixed use with pointers from
	the non-counted pointer classes in "ptrs.h":
		excl_ptr, some_ptr, never_ptr.  
***/
//=============================================================================
/**
	Reference counted pointer, non-const flavor.  
	How to use...
 */
template <class T>
class count_ptr {
friend class pointer_manipulator;
template <class> friend class count_ptr;

public:
	typedef T			element_type;
	typedef T&			reference;
	typedef T*			pointer;
	typedef	shared_owner_pointer_tag	pointer_category;
	static const bool		is_array = false;
	static const bool		is_intrusive = false;
	static const bool		is_counted = true;
	static const bool		is_shared = true;
	static const bool		always_owns = false;
	static const bool		sometimes_owns = true;
	static const bool		never_owns = false;
	typedef	count_ptr<T>		nontransfer_cast_type;
	typedef	count_ptr<T>		transfer_cast_type;

protected:
	/**
		The pointer to the referenced object.  
	 */
	T*			ptr;
	/// the reference count
	size_t*			ref_count;

public:
	/**
		Constructor of reference-counter pointer.  
		Stupid things to avoid: 
		1) tampering with (leaking out) newly allocated
			pointer before passing to this constructor.  
		\param p should be a newly allocated pointer, ONLY.  
	 */
	explicit
	count_ptr(T* p = NULL) :
	ptr(p), ref_count((p) ? new size_t(1) : NULL) { }

	/**
		Copy constructor.  
		No argument pointer should ever be NULL?
	 */
	count_ptr(const count_ptr<T>& c) :
			ptr(c.ptr), ref_count(c.ref_count) {
		if (this->ref_count) {
			(*this->ref_count)++;
			// NEVER_NULL(ptr);
			REASONABLE_REFERENCE_COUNT;
		} else {
			// INVARIANT(!ptr);
		}
	}

	/**
		Safe, up-cast copy constructor.  
		Still maintains reference count.  
	 */
	template <class S>
	count_ptr(const count_ptr<S>& c) :
			ptr(c.ptr), ref_count(c.ref_count) {
		if (this->ref_count) {
			(*this->ref_count)++;
			// NEVER_NULL(ptr);
			REASONABLE_REFERENCE_COUNT;
		} else {
			// INVARIANT(!ptr);
		}
	}

	/**
		UNSAFE: constructor that corecively sets the 
		reference pointer and the count pointer 
		and increments the pointer.  
		The manager should initially allocate a counter set to zero.  
		The manager should not own the pointer or the counter.  
		Needed for transient pointer to object reconstruction.  
		Use this only if you know what you're doing.
	 */
	count_ptr(T* p, size_t* c) : ptr(p), ref_count(c) {
		if (p) {
			// NEVER_NULL(c);		// counter must be valid
			(*c)++;			// increment here
			REASONABLE_REFERENCE_COUNT;
		} else {
			// INVARIANT(!c);		// counter must also be NULL
		}
	}

protected:
	/**
		Accessor provided as a method for consistency
		with standard pointer operations.  
	 */
	const pointer&
	base_pointer(void) const { return ptr; }

	/**
		Overkill safety checks.  
	 */
	pointer
	release(void) {
		T* ret = ptr;
		if(this->ref_count) {
			// INVARIANT(*this->ref_count);
			(*this->ref_count)--;
			REASONABLE_REFERENCE_COUNT;
			if (!*this->ref_count) {
				// NEVER_NULL(ptr);
				delete ptr;
				delete this->ref_count;
			}
			ptr = NULL;
			this->ref_count = NULL;
		} else {
			// INVARIANT(!ptr);
		}
		return ret;
	}

	/**
		\param p the new pointer, from some other count_ptr.
		\param c the corresponding reference count if p is valid, 
			else is ignored.  
	 */
	void
	reset(T* p, size_t* c) {
		if(this->ref_count) {
			// INVARIANT(*this->ref_count);
			(*this->ref_count)--;
			REASONABLE_REFERENCE_COUNT;
			if (!*this->ref_count) {
				// NEVER_NULL(ptr);
				delete ptr;
				delete this->ref_count;
			}
		} else {
			// INVARIANT(!ptr);
		}
		ptr = p;
		this->ref_count = (ptr) ? c : NULL;
		if (ptr && this->ref_count) {
			(*this->ref_count)++;
			REASONABLE_REFERENCE_COUNT;
		}
	}

public:
	/**
		Destructor, which conditionally deletes pointer if is
		last reference.  
	 */
	~count_ptr() { this->release(); }

	/**
		Reference count.  
	 */
	size_t
	refs(void) const {
		if (ref_count) return *ref_count;
		else return 0;
	}

	/**
		A shared reference is not really owned by any one
		copy of the pointer.  
		The last reference to an object is the owner.  
	 */
	bool owned(void) const {
		return (ref_count && *ref_count == 1);
	}

	/// synonym for release()
	void abandon(void) { release(); }

	/**
		Dereference.  
	 */
	reference
	operator * (void) const throw() { return *ptr; }

	/**
		Dereference to member or method.  
	 */
	pointer
	operator -> (void) const throw() { return ptr; }

	operator bool() const { return ptr != NULL; }

};	// end class count_ptr

//=============================================================================
// pointer_traits specializations

SPECIALIZE_ALL_POINTER_TRAITS(count_ptr)

//=============================================================================
}	// end namespace memory
}	// end namespace util

//=============================================================================

#endif	//	__UTIL_MEMORY_COUNT_PTR_GCC41DEATH_H__

