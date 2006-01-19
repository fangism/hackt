/**
	\file "util/memory/unique_ptr.h"
	Library of pointers for explicit memory management.  
	The pointer classes contained herein are all non-counted.  
	For reference-counted classes, see "count_ptr.h". 

	Initially just copied from ptrs.h, but needs to be overhauled:
	eliminate derivation from abstract ptr class.  
	Eliminate const- duplication, should be able to
	specialize as const T if necessary?


	Be able to attach pointer to allocator? oooooo....
	Be able to pass pointers between regions?  maybe not...

	$Id: unique_ptr.h,v 1.1.2.3 2006/01/19 00:16:17 fang Exp $
 */
// all methods in this file are to be defined here, to be inlined

// to do: references! (as non-null pointers)

// major issue: pointer type conversion emulation with classes
// friends and templates:
//	pointer classes can't access each other private constructors
//	because can't template friend classes, no generalization... :(

// lock pointers? non-transferrable exclusive pointers.  

#ifndef	__UTIL_MEMORY_UNIQUE_PTR_H__
#define	__UTIL_MEMORY_UNIQUE_PTR_H__

#include "util/macros.h"
#include "util/memory/pointer_classes_fwd.h"
#include "util/memory/pointer_manipulator.h"
#include "util/memory/deallocation_policy_fwd.h"
#include "util/STL/construct_fwd.h"

//=============================================================================
// debugging stuff

/**
	Debuggling flag for null-pointer-checking the unique_ptr 
	pointer class family (including never_ptr, some_ptr, sticky_ptr).
	This may be predefined to explicitly enable checking per module.  
 */
#ifndef	DEBUG_UNIQUE_PTR
#define	DEBUG_UNIQUE_PTR		0
#endif

#if DEBUG_UNIQUE_PTR
	#define	UNIQUE_PTR_NEVER_NULL(x)		NEVER_NULL(x)
#else
	#define	UNIQUE_PTR_NEVER_NULL(x)
#endif

#ifndef	UNIQUE_PTR_TEMPLATE_SIGNATURE
#define	UNIQUE_PTR_TEMPLATE_SIGNATURE	template <class T, class Dealloc>
#endif

#ifndef	UNIQUE_PTR_TEMPLATE_CLASS
#define	UNIQUE_PTR_TEMPLATE_CLASS	unique_ptr<T, Dealloc>
#endif

//=============================================================================

namespace util {
namespace memory {
//=============================================================================
// forward declarations (repeated)

//=============================================================================
/**
	NOTE: no aliases to this pointer may exist, i.e. it is not implicitly
	convertable to other non-owning pointer classes.  
	Note that, this can only take a raw-pointer for a constructor
	argument, and no other pointer-class. 
	Consider adding a __restrict__ attribute.

	NOTE: care must be taken to ensure that references (&) don't
	outlive the internal pointer... can this be achieved with some
	metaprogramming magic?

	This pointer is the exclusive owner of the pointed memory, 
	no one else may delete it.  Others who use it must promise
	not to delete it.  Very similar to auto_ptr, with destructive
	transfer of ownership, (as if we derived from it privately).
	The only methods of releasing memory are through
	destructive transfer, or end of scope.  
	We want unique_ptr to be usable with standard containers.  
	A const unique_ptr prevents transfer of ownership.  
	\param T the element type.
	\param TP the pointer to the element type, usually T*.
		This may be overridden to do some crazy things.  
 */
UNIQUE_PTR_TEMPLATE_SIGNATURE
class unique_ptr {
friend class pointer_manipulator;
	typedef	unique_ptr<T,Dealloc>	this_type;
public:
	typedef	this_type		type;
	typedef	T			element_type;
	typedef	Dealloc			deallocation_policy;
	typedef	T&			reference;
	typedef	T*			pointer;
	typedef	exclusive_owner_pointer_tag	pointer_category;
	static const bool		is_array = false;
	static const bool		is_intrusive = false;
	static const bool		is_counted = false;
	static const bool		is_shared = true;
	static const bool		always_owns = true;
	static const bool		sometimes_owns = true;
	static const bool		never_owns = false;
private:
	pointer				ptr;

private:
	/**
		Accessor provided as a method for consistency
		with standard pointer operations.  
	 */
	const pointer&
	base_pointer(void) const { return ptr; }

public:
/**
	Relinquishes resposibility for deleting pointer, by returning a
	copy of it, and nullifying its own pointer.  
	This is allowed to be public because the uniqueness property
	makes it safe to explicitly leak this to the outside world.  
	\return a pointer that must be deleted by it consumer if not NULL.  
 */
	pointer
	release(void) throw() {
		const pointer ret = this->ptr;
		this->ptr = NULL;
		return ret;
	}

/**
	Sets the member pointer, deleting the previous value
	if appropriate.  
	Is public because should be safe to explicitly destroy 
	a unique pointer.  
	\param p non-const pointer that is exclusively owned.  
 */
	void
	reset(const pointer p = NULL) throw() {
		if (this->ptr) {
			INVARIANT(this->ptr != p);
			// violation of exclusion!
			// delete this->ptr;
			deallocation_policy()(this->ptr);
		}
		this->ptr = p;
	}

public:
/**
	Ownership query.
 */
	bool
	owned(void) const { return this->ptr != NULL; }

/**
	\param p should be a newly allocated pointer, which has not
		been tampered with or leaked out.  Vulnerability
		here is that we don't know what else has been done with p.  
		Can we make new T return an unique_ptr?  No.
 */
	explicit
	unique_ptr(pointer p) throw() : ptr(p) { }

	unique_ptr(void) throw() : ptr(NULL) { }

/**
	Destructive copy constructor, non-const source.  
	Transfers ownership.  
	To make non-transferrable, declare as const unique_ptr<>.  
	This is not redundant, need this.  
 */
	unique_ptr(this_type& e) throw() : ptr(e.release()) { }

/**
	For safe up-cast, pointer constructor.  
 */
	template <class S>
	unique_ptr(unique_ptr<S, deallocation_policy>& e) throw() :
		ptr(e.release()) { }

private:
	// intentionally inaccessible undefined copy-constructor
	explicit
	unique_ptr(const this_type& e);

public:
/**
	De-allocates memory.  
	Should be safe to delete NULL?
	non-virtual destructor => don't use inheritance with pointers 
	to pointer-classes.
 */
	~unique_ptr() { this->reset(); }

public:
	/**
		\return reference at the pointer.  
	 */
	reference
	operator * () const throw() { UNIQUE_PTR_NEVER_NULL(ptr); return *ptr; }

	/**
		Pointer member/method dereference, unchecked.  
		\returns the pointer.  
	 */
	pointer
	operator -> () const throw() { UNIQUE_PTR_NEVER_NULL(ptr); return ptr; }

	operator bool() const { return ptr != NULL; }

/**
	Will not accept const pointer of p.  
	Naked p MUST be an exclusive owner.  
	Forbidding direct interaction with naked pointer.  
	unique_ptr<T>&
	operator = (T* p) throw() {
		reset(p);
		return *this;
	}
**/
	this_type&
	operator = (this_type& e) throw() {
		reset(e.release());
		return *this;
	}

	/**
		Acceptable transfer for safe-casts.  
	 */
	template <class S>
	this_type&
	operator = (unique_ptr<S, deallocation_policy>& e) throw() {
		reset(e.release());
		return *this;
	}


	// destructive transfer to up-cast
	template <class S>
	operator unique_ptr<S, deallocation_policy> () throw() {
		return unique_ptr<S, deallocation_policy>(this->release());
	}


	/**
		Dynamic cast assertion.  
		No return value.  
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

// permissible assignments

// non-member functions

};	// end class unique_ptr

//=============================================================================
// pointer traits specializations

SPECIALIZE_ALL_POINTER_TRAITS(unique_ptr)

//=============================================================================
}	// end namespace memory
}	// end namespace util

//=============================================================================

#undef	UNIQUE_PTR_TEMPLATE_SIGNATURE
#undef	UNIQUE_PTR_TEMPLATE_CLASS

#endif	//	__UTIL_MEMORY_UNIQUE_PTR_H__

