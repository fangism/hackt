/**
	\file "util/memory/count_ptr.hh"
	Simple reference-count pointer class.  
	Do not mix with non-counted pointer types.  

	$Id: count_ptr.hh,v 1.12 2007/07/31 23:23:52 fang Exp $
 */

#ifndef	__UTIL_MEMORY_COUNT_PTR_H__
#define	__UTIL_MEMORY_COUNT_PTR_H__

#include "util/macros.h"
#include "util/memory/pointer_classes_fwd.hh"
#include "util/memory/pointer_manipulator.hh"
#if defined(BUILT_HACKT) || defined(INSTALLED_HACKT)
#include <cstddef>
#else
#include "util/size_t.h"
#endif

//=============================================================================
// debugging stuff

#ifndef	REASONABLE_REFERENCE_COUNT
#if 0
#define	REASONABLE_REFERENCE_COUNT			\
	INVARIANT(*ref_count < 10000)
#else
#define	REASONABLE_REFERENCE_COUNT
#endif
#endif

/**
	Whether or not to use a global reference count pool
	for allocating size_t's (counts).  
	We suppress ref-count-pool dependencies for the installed version
	of this header.  
 */
#if !defined(BUILT_HACKT) && !defined(INSTALLED_HACKT)
#define	USE_REF_COUNT_POOL				1
#else
#define	USE_REF_COUNT_POOL				0
#endif

/**
	Multiple levels of debugging count_ptrs:
	0 = off,
	1 = fast invariants only (non-null, comparison),
	2 = all invariants (includes slower checks), 
	3 = with null-checks before deferencing
	Recommended default level: 1 or 2
 */
#ifndef	DEBUG_COUNT_PTR
#define	DEBUG_COUNT_PTR				1
#endif

#if DEBUG_COUNT_PTR >= 3
	#define	COUNT_PTR_NEVER_NULL(x)		NEVER_NULL(x)
#else
	#define	COUNT_PTR_NEVER_NULL(x)
#endif

#if DEBUG_COUNT_PTR >= 2
	#define	COUNT_PTR_INVARIANT(x)		INVARIANT(x)
#else
	#define	COUNT_PTR_INVARIANT(x)
#endif

#if DEBUG_COUNT_PTR >= 1
	#define	COUNT_PTR_FAST_INVARIANT(x)	INVARIANT(x)
#else
	#define	COUNT_PTR_FAST_INVARIANT(x)
#endif

#define	COUNT_PTR_TEMPLATE_SIGNATURE	template <class T, class Dealloc>
#define	COUNT_PTR_CLASS			count_ptr<T,Dealloc>

#define	RAW_COUNT_PTR_TEMPLATE_SIGNATURE	COUNT_PTR_TEMPLATE_SIGNATURE
#define	RAW_COUNT_PTR_CLASS		raw_count_ptr<T,Dealloc>

namespace util {
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
RAW_COUNT_PTR_TEMPLATE_SIGNATURE
struct raw_count_ptr {
	typedef	Dealloc			deallocation_policy;
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
		COUNT_PTR_FAST_INVARIANT(ptr);
		COUNT_PTR_FAST_INVARIANT(ref_count);
	}

	template <class S>
	raw_count_ptr(S* p, size_t* c) : ptr(p), ref_count(c) {
		COUNT_PTR_FAST_INVARIANT(ptr);
		COUNT_PTR_FAST_INVARIANT(ref_count);
	}

	template <class S>
	raw_count_ptr(const raw_count_ptr<S, deallocation_policy>& r) :
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
}	// end namespace memory
}	// end namespace util

#ifndef	HAVE_COUNT_PTR_REF_COUNT_POOL_MACROS
#define	HAVE_COUNT_PTR_REF_COUNT_POOL_MACROS
#if USE_REF_COUNT_POOL
	#include "util/attributes.h"
	#define STATIC_RC_POOL_REF_INIT					\
		static util::memory::ref_count_pool_type&		\
		rc_pool_ref __ATTRIBUTE_UNUSED__ =			\
		*util::memory::get_ref_count_allocator_anchor()
	#define	NEW_SIZE_T		rc_pool_ref.allocate()
	#define	DELETE_SIZE_T(x)	rc_pool_ref.deallocate(x)
	#define	VALIDATE_SIZE_T(x)	COUNT_PTR_INVARIANT(rc_pool_ref.contains(x))
	#include "util/memory/ref_count_pool.hh"
#else	// !USE_REF_COUNT_POOL
	#define STATIC_RC_POOL_REF_INIT		// blank
	#define	NEW_SIZE_T		new size_t
	#define	DELETE_SIZE_T(x)	delete x
	#define	VALIDATE_SIZE_T(x)
#endif	// USE_REF_COUNT_POOL
#endif	// HAVE_COUNT_PTR_REF_COUNT_POOL_MACROS

namespace util {
namespace memory {
//=============================================================================
/**
	Reference counted pointer, non-const flavor.  
	How to use...
 */
COUNT_PTR_TEMPLATE_SIGNATURE
class count_ptr {
friend class pointer_manipulator;
template <class, class> friend class count_ptr;
	typedef	count_ptr<T,Dealloc>	this_type;
public:
	typedef	this_type		type;
	typedef T			element_type;
	typedef	Dealloc			deallocation_policy;
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
	typedef	this_type		nontransfer_cast_type;
	typedef	this_type		transfer_cast_type;

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
	count_ptr(T* p = NULL);

	explicit
	count_ptr(const raw_count_ptr<T, deallocation_policy>& r);

	template <class S>
	explicit
	count_ptr(const raw_count_ptr<S, deallocation_policy>& r);

	/**
		Copy constructor.  
		No argument pointer should ever be NULL?
	 */
	count_ptr(const this_type& c) :
			ptr(c.ptr), ref_count(c.ref_count) {
		if (this->ref_count) {
			(*this->ref_count)++;
			COUNT_PTR_FAST_INVARIANT(ptr);
			REASONABLE_REFERENCE_COUNT;
		} else {
			COUNT_PTR_FAST_INVARIANT(!ptr);
		}
	}

	/**
		Safe, up-cast copy constructor.  
		Still maintains reference count.  
	 */
	template <class S>
	count_ptr(const count_ptr<S, deallocation_policy>& c) :
			ptr(c.ptr), ref_count(c.ref_count) {
		if (this->ref_count) {
			(*this->ref_count)++;
			COUNT_PTR_FAST_INVARIANT(ptr);
			REASONABLE_REFERENCE_COUNT;
		} else {
			COUNT_PTR_FAST_INVARIANT(!ptr);
		}
	}

	/// Reserved, see comment in "util/memory/count_ptr.tcc"
	count_ptr(T* p, size_t* c);

protected:
	/**
		Accessor provided as a method for consistency
		with standard pointer operations.  
	 */
	const pointer&
	base_pointer(void) const { return ptr; }

	/// documented in "util/memory/count_ptr.tcc"
	pointer
	release(void);

	/// documented in "util/memory/count_ptr.tcc"
	void
	reset(T* p, size_t* c);

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
	bool
	owned(void) const {
		return (ref_count && *ref_count == 1);
	}

	/// synonym for release()
	void
	abandon(void) { release(); }

	/**
		Dereference.  
	 */
	reference
	operator * (void) const throw() {
		COUNT_PTR_NEVER_NULL(ptr); return *ptr;
	}

	/**
		Dereference to member or method.  
	 */
	pointer
	operator -> (void) const throw() {
		COUNT_PTR_NEVER_NULL(ptr); return ptr;
	}

	operator bool() const { return ptr != NULL; }

	/**
		Assignment operator.  
		Need to be careful if assigning to self!
	 */
	this_type&
	operator = (const count_ptr<T, deallocation_policy>& c) {
		reset(c.ptr, c.ref_count);
		return *this;
	}

	/// documented in "util/memory/count_ptr.tcc"
	pointer
	exclusive_release(void);

	/**
		Static cast.  
	 */
	template <class S>
	count_ptr<S, deallocation_policy>
	as_a(void) const {
		return count_ptr<S, deallocation_policy>(
			static_cast<S*>(this->ptr), this->ref_count);
	}

	/**
		Dynamic cast.  
	 */
	template <class S>
	count_ptr<S, deallocation_policy>
	is_a(void) const {
		return count_ptr<S, deallocation_policy>(
			dynamic_cast<S*>(this->ptr), 
			this->ref_count);
	}

	/**
		Dynamic cast assertion.  
	 */
	template <class S>
	void
	must_be_a(void) const {
		assert(dynamic_cast<S*>(this->ptr));
	}

	// don't rely on using std::rel_ops
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

	template <class P>
	bool
	operator < (const P& p) const {
		return pointer_manipulator::compare_pointers_less(ptr, p);
	}

	template <class P>
	bool
	operator <= (const P& p) const {
		return !pointer_manipulator::compare_pointers_less(p, ptr);
	}

	template <class P>
	bool
	operator > (const P& p) const {
		return pointer_manipulator::compare_pointers_less(ptr, p);
	}

	template <class P>
	bool
	operator >= (const P& p) const {
		return !pointer_manipulator::compare_pointers_less(p, ptr);
	}

	template <class S>
	struct rebind {
		typedef	count_ptr<S, deallocation_policy>	type;
	};

};	// end class count_ptr

//=============================================================================
// pointer_traits specializations

SPECIALIZE_ALL_POINTER_TRAITS_2(count_ptr)

//=============================================================================
}	// end namespace memory
}	// end namespace util

#if USE_REF_COUNT_POOL
#include "util/memory/ref_count_pool_anchor.hh"
#endif

//=============================================================================

#endif	//	__UTIL_MEMORY_COUNT_PTR_H__

