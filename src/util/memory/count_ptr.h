/**
	\file "memory/count_ptr.h"
	Simple reference-count pointer class.  
	Do not mix with non-counted pointer types.  

	$Id: count_ptr.h,v 1.1.2.2 2005/01/24 20:51:43 fang Exp $

	TODO:
		* split into .tcc file
		* pool allocate size_t counts
 */

#ifndef	__UTIL_MEMORY_COUNT_PTR_H__
#define	__UTIL_MEMORY_COUNT_PTR_H__

#include "config.h"
#if HAVE_STDDEF_H
#include <cstddef>			// for size_t
#endif

#include "macros.h"
#include "memory/pointer_classes_fwd.h"
#include "memory/pointer_manipulator.h"

//=============================================================================
// debugging stuff

#if 0
#include <iostream>
#include "using_ostream.h"
#endif

#define	REASONABLE_REFERENCE_COUNT			\
	INVARIANT(*ref_count < 10000)

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
	Unsafe copy of a reference-count pointer, intended for
	transfer use, and places where you know what you're doing.  
	(Ok, where *I* know what I'm doing.)

	Note this never deletes anything.  
 */
template <class T>
struct raw_count_ptr {
	typedef T			element_type;
	typedef T&			reference;
	typedef T*			pointer;

// template <class> friend class count_ptr;
	pointer	ptr;
	size_t*	ref_count;

	/**
		Never transfers invalid pointers and reference counts.  
	 */
	raw_count_ptr(T* p, size_t* c) : ptr(p), ref_count(c) {
		NEVER_NULL(ptr);
		NEVER_NULL(ref_count);
	}

	template <class S>
	raw_count_ptr(S* p, size_t* c) : ptr(p), ref_count(c) {
		NEVER_NULL(ptr);
		NEVER_NULL(ref_count);
	}

	template <class S>
	raw_count_ptr(const raw_count_ptr<S>& r) :
			ptr(r.ptr), ref_count(r.ref_count) {
	}

	// plain copy-construction

	// plain destruction

	// plain assignment

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

	size_t
	refs(void) const {
		return *ref_count;
	}

};	// end struct raw_count_ptr

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

	explicit
	count_ptr(const raw_count_ptr<T>& r) :
			ptr(r.ptr), ref_count(r.ref_count) {
		if (this->ref_count) {
			(*this->ref_count)++;
			NEVER_NULL(ptr);
			REASONABLE_REFERENCE_COUNT;
		} else {
			INVARIANT(!ptr);
		}
	}

	template <class S>
	explicit
	count_ptr(const raw_count_ptr<S>& r) :
			ptr(r.ptr), ref_count(r.ref_count) {
		if (this->ref_count) {
			(*this->ref_count)++;
			NEVER_NULL(ptr);
			REASONABLE_REFERENCE_COUNT;
		} else {
			INVARIANT(!ptr);
		}
	}

	/**
		Copy constructor.  
		No argument pointer should ever be NULL?
	 */
	count_ptr(const count_ptr<T>& c) :
			ptr(c.ptr), ref_count(c.ref_count) {
		if (this->ref_count) {
			(*this->ref_count)++;
			NEVER_NULL(ptr);
			REASONABLE_REFERENCE_COUNT;
		} else {
			INVARIANT(!ptr);
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
			NEVER_NULL(ptr);
			REASONABLE_REFERENCE_COUNT;
		} else {
			INVARIANT(!ptr);
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
			NEVER_NULL(c);		// counter must be valid
			(*c)++;			// increment here
			REASONABLE_REFERENCE_COUNT;
		} else {
			INVARIANT(!c);		// counter must also be NULL
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
			INVARIANT(*this->ref_count);
			(*this->ref_count)--;
			REASONABLE_REFERENCE_COUNT;
			if (!*this->ref_count) {
				NEVER_NULL(ptr);
				delete ptr;
#if 0
				cerr << "count_ptr<>::release() @ "
					<< this << endl;
				char c; std::cin >> c;
#endif
				delete this->ref_count;
			}
			ptr = NULL;
			this->ref_count = NULL;
		} else {
			INVARIANT(!ptr);
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
			INVARIANT(*this->ref_count);
			(*this->ref_count)--;
			REASONABLE_REFERENCE_COUNT;
			if (!*this->ref_count) {
				NEVER_NULL(ptr);
				delete ptr;
#if 0
				cerr << "count_ptr<>::reset() @ "
					<< this << endl;
				char c; std::cin >> c;
#endif
				delete this->ref_count;
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
	operator * (void) const throw() { NEVER_NULL(ptr); return *ptr; }

	/**
		Dereference to member or method.  
	 */
	pointer
	operator -> (void) const throw() { NEVER_NULL(ptr); return ptr; }

	operator bool() const { return ptr != NULL; }

	/**
		Assignment operator.  
	 */
	count_ptr<T>&
	operator = (const count_ptr<T>& c) {
		reset(c.ptr, c.ref_count);
		return *this;
	}

	/**
		Returns a naked pointer only if this is the final reference
		to the object, otherwise returns NULL.  
	 */
	pointer
	exclusive_release(void) {
		if (this->owned()) {
			T* ret = ptr;
			ptr = NULL;
			delete this->ref_count;
			this->ref_count = NULL;
			return ret;
		} else
			return NULL;
	}

	/**
		Static cast.  
	 */
	template <class S>
	count_ptr<S>
	as_a(void) const {
		count_ptr<S> ret;
		ret.reset(static_cast<S*>(this->ptr), this->ref_count);
		return ret;
	}

	/**
		Dynamic cast.  
	 */
	template <class S>
	count_ptr<S>
	is_a(void) const {
		count_ptr<S> ret;
		ret.reset(dynamic_cast<S*>(this->ptr), this->ref_count);
		return ret;
	}

	/**
		Dynamic cast assertion.  
	 */
	template <class S>
	void
	must_be_a(void) const {
		assert(dynamic_cast<S*>(this->ptr));
	}

	template <class P>
	bool
	operator == (const P& p) const {
		return pointer_manipulator::compare_pointers_equal(ptr, p);
	}

	template <class P>
	bool
	operator != (const P& p) const {
		return pointer_manipulator::compare_pointers_unequal(ptr, p);
	}

};	// end class count_ptr

//=============================================================================
}	// end namespace memory
}	// end namespace util

//=============================================================================

#endif	//	__UTIL_MEMORY_COUNT_PTR_H__

