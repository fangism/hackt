/**
	\file "util/memory/excl_ptr.h"
	Library of pointers for explicit memory management.  
	The pointer classes contained herein are all non-counted.  
	For reference-counted classes, see "count_ptr.h". 

	Initially just copied from ptrs.h, but needs to be overhauled:
	eliminate derivation from abstract ptr class.  
	Eliminate const- duplication, should be able to
	specialize as const T if necessary?


	Be able to attach pointer to allocator? oooooo....
	Be able to pass pointers between regions?  maybe not...

	$Id: excl_ptr.h,v 1.14.56.1 2008/03/01 08:34:27 fang Exp $
 */
// all methods in this file are to be defined here, to be inlined

// to do: references! (as non-null pointers)

// major issue: pointer type conversion emulation with classes
// friends and templates:
//	pointer classes can't access each other private constructors
//	because can't template friend classes, no generalization... :(

// lock pointers? non-transferrable exclusive pointers.  

#ifndef	__UTIL_MEMORY_EXCL_PTR_H__
#define	__UTIL_MEMORY_EXCL_PTR_H__

#include "util/macros.h"
#include "util/memory/pointer_classes_fwd.h"
#include "util/memory/pointer_manipulator.h"
#include "util/memory/deallocation_policy_fwd.h"
#include "util/STL/construct_fwd.h"

//=============================================================================
// debugging stuff

/**
	Debuggling flag for null-pointer-checking the excl_ptr 
	pointer class family (including never_ptr, some_ptr, sticky_ptr).
	This may be predefined to explicitly enable checking per module.  
 */
#ifndef	DEBUG_EXCL_PTR
#define	DEBUG_EXCL_PTR		0
#endif

#if DEBUG_EXCL_PTR
	#define	EXCL_PTR_NEVER_NULL(x)		NEVER_NULL(x)
#else
	#define	EXCL_PTR_NEVER_NULL(x)
#endif

#ifndef	EXCL_PTR_TEMPLATE_SIGNATURE
#define	EXCL_PTR_TEMPLATE_SIGNATURE	template <class T, class Dealloc>
#endif

#ifndef	EXCL_PTR_TEMPLATE_CLASS
#define	EXCL_PTR_TEMPLATE_CLASS		excl_ptr<T,Dealloc>
#endif

//=============================================================================

namespace util {
namespace memory {
//=============================================================================
// forward declarations (repeated)
template <class> class never_ptr;
template <class, class> class some_ptr;

//=============================================================================
/**
	Helper class for type-casting.  
	Is private to the outside world, only usable by template friends.  
 */
template <class S, class Dealloc>
class excl_ptr_ref {
template <class, class> friend class excl_ptr;
template <class, class> friend class some_ptr;
template <class> friend class never_ptr;
private:
	S*	ptr;

	explicit
	excl_ptr_ref(S* p) throw() : ptr(p) { }
};	// end struct excl_ptr_ref

//=============================================================================
/**
	This pointer is the exclusive owner of the pointed memory, 
	no one else may delete it.  Others who use it must promise
	not to delete it.  Very similar to auto_ptr, with destructive
	transfer of ownership, (as if we derived from it privately).
	The only methods of releasing memory are through
	destructive transfer, or end of scope.  
	We want excl_ptr to be usable with standard containers.  
	A const excl_ptr prevents transfer of ownership.  
	Later: excl_const_ptr.  
	\param T the element type.
	\param TP the pointer to the element type, usually T*.
		This may be overridden to do some crazy things.  
 */
// EXCL_PTR_TEMPLATE_SIGNATURE
template <class T, class Dealloc>
class excl_ptr {
friend class pointer_manipulator;
template <class, class> friend class excl_ptr;
template <class, class> friend class sticky_ptr;
template <class> friend class never_ptr;
template <class, class> friend class some_ptr;
	typedef	excl_ptr<T, Dealloc>	this_type;
public:
	typedef	this_type		type;
	typedef	T			element_type;
	typedef	Dealloc			deallocation_policy;
	typedef	T&			reference;
	/**
		The underlying pointer member type.  
	 */
	typedef	T*			pointer;
	/**
		This is always a native pointer type, 
		regardless of what TP is.  
	 */
	typedef	T*			raw_pointer;
	typedef	exclusive_owner_pointer_tag	pointer_category;
	static const bool		is_array = false;
	static const bool		is_intrusive = false;
	static const bool		is_counted = false;
	static const bool		is_shared = true;
	static const bool		always_owns = true;
	static const bool		sometimes_owns = true;
	static const bool		never_owns = false;
	// really wish for template typedefs!!!
	typedef	never_ptr<T>		nontransfer_cast_type;
	typedef	this_type		transfer_cast_type;
private:
	pointer				ptr;

private:
	/**
		Accessor provided as a method for consistency
		with standard pointer operations.  
	 */
	const pointer&
	base_pointer(void) const { return ptr; }
/**
	Relinquishes resposibility for deleting pointer, by returning a
	copy of it, and nullifying its own pointer.  
	Must remain private, hence not publicly accessible.  
	\return a pointer that must be deleted by it consumer if not NULL.  
 */
	pointer
	release(void) throw() {
		const raw_pointer ret = this->ptr;
		this->ptr = NULL;
		return ret;
	}

/**
	Sets the member pointer, deleting the previous value
	if appropriate.  
	Must be private, not publicly available.  
	\param p non-const pointer that is exclusively owned.  
		Regardless of TP, this is always a raw pointer.  
 */
	void
	reset(const raw_pointer p = NULL) throw() {
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
		Can we make new T return an excl_ptr?  No.
		Regardless of TP, this is always T*.
 */
	explicit
	excl_ptr(raw_pointer p) throw() : ptr(p) { }

	excl_ptr(void) throw() : ptr(NULL) { }

/**
	Destructive copy constructor, non-const source.  
	Transfers ownership.  
	To make non-transferrable, declare as const excl_ptr<>.  
	This is not redundant, need this.  
 */
	excl_ptr(this_type& e) throw() : ptr(e.release()) { }

/**
	For safe up-cast, pointer constructor.  
 */
	template <class S>
	excl_ptr(excl_ptr<S, deallocation_policy>& e) throw() :
		ptr(e.release()) { }

/**
	Transfer from some_ptr, changes ownership, while leaving
	the argument's pointer intact.  
 */
	explicit
	excl_ptr(some_ptr<T, deallocation_policy>& s) throw();

#if 0
/**
	Bogus copy constructor, const source.  
	Doesn't actually copy source.  
	Need this to make some standard containers happy.  
	Copy constructor of containers will result in NULL members.  
	Preserves exclusion.  
	Consider: creating a container of never_ptrs from a container
		of excl_ptrs... possible to write such a template?
 */
	explicit
	excl_ptr(const excl_ptr<T>& e) throw() : ptr(NULL) {
		// what if e was already NULL?
		// INVARIANT(!e);
		// issue a warning?
	}
#endif

private:
	// intentionally inaccessible undefined copy-constructor
	explicit
	excl_ptr(const this_type& e);

public:
/**
	De-allocates memory.  
	Should be safe to delete NULL?
	non-virtual destructor => don't use inheritance with pointers 
	to pointer-classes.
 */
	// virtual
	~excl_ptr() { this->reset(); }

	/**
		\return a deep copy of this, using operator new to allocate.
	 */
	this_type
	deep_copy(void) const {
		return this_type(new T(*ptr));
	}
public:
	/**
		\return reference at the pointer.  
	 */
	reference
	operator * () const throw() { EXCL_PTR_NEVER_NULL(ptr); return *ptr; }

	/**
		Pointer member/method dereference, unchecked.  
		\returns the pointer.  
	 */
	pointer
	operator -> () const throw() { EXCL_PTR_NEVER_NULL(ptr); return ptr; }

	operator bool() const { return ptr != NULL; }

	/**
		Swap pointers and ownership.  
	 */
	void
	swap(this_type& e) {
		// std::swap or XOR trick, not crucial
		pointer p = this->ptr;
		this->ptr = e.ptr;
		e.ptr = p;
	}

/**
	Will not accept const pointer of p.  
	Naked p MUST be an exclusive owner.  
	Forbidding direct interaction with naked pointer.  
	this_type&
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
	operator = (excl_ptr<S, deallocation_policy>& e) throw() {
		reset(e.release());
		return *this;
	}


	// copied from auto_ptr_ref
	excl_ptr(excl_ptr_ref<T, deallocation_policy> r) throw() :
		ptr(r.ptr) { }

#if 1
	// not in book...
	template <class S>
	excl_ptr(excl_ptr_ref<S, deallocation_policy> r) throw() :
		ptr(r.ptr) { }
#endif

	this_type&
	operator = (excl_ptr_ref<T, deallocation_policy> r) throw() {
		reset(r.ptr);
		r.ptr = NULL;
		return *this;
	}

	/**
		Acquiring ownership from a some_ptr, destructive transfer.
		Transfer is conditional on the ownership of the source.  
	 */
	this_type&
	operator = (some_ptr<T, deallocation_policy>& r) throw() {
		if (r.owned())
			this->ptr = r.release();
		return *this;
	}

#if 0
	/**
		Does nothing!  can't acquire ownership from a const source.  
	 */
	this_type&
	operator = (const some_ptr<T>& r) throw() { }
#endif

#if 0
// might be problem
	template <class S>
	operator excl_ptr<S>& () throw() {
		static_cast<S*>(this->ptr);	// safety check
		return reinterpret_cast<excl_ptr<S>&>(*this);
	}
#endif

	// destructive transfer to up-cast
	template <class S>
	operator excl_ptr<S, deallocation_policy> () throw() {
		return excl_ptr<S, deallocation_policy>(this->release());
	}

	// safe type-casting
	template <class S>
	operator excl_ptr_ref<S, deallocation_policy>() throw() {
		return excl_ptr_ref<S, deallocation_policy>(this->release());
	}

#if 0
	template <class S>
	operator never_ptr<S>() throw() const;
#endif

	/**
		Dynamic cast template wrapper.  
		Can't overload dynamic_cast, because it's a keyword.  
		Avoid returning naked pointers!
	 */
	// TODO: use pointer_traits cast-type.  
	template <class S>
	never_ptr<S>		// nontransfer_cast_type
	is_a(void) const;

	/**
		Ownership transferring dynamic cast.  
	 */
	template <class S>
	excl_ptr<S, deallocation_policy>		// transfer_cast_type
	is_a_xfer(void);

	/** static cast */
	template <class S>
	never_ptr<S>		// nontransfer_cast_type
	as_a(void) const;

	/** static cast with xfer */
	template <class S>
	excl_ptr<S, deallocation_policy>		// transfer_cast_type
	as_a_xfer(void);

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

// permissible assignments

// non-member functions

	/**
		We tentatively disallow rebinding of the allocator policy.
	 */
	template <class S>
	struct rebind {
		typedef	excl_ptr<S, deallocation_policy>	type;
	};

};	// end class excl_ptr

//=============================================================================
/**
	Sticky pointers can acquire ownership, but will never relinquish it.  
	Only explicit call to release() will relinquish ownership. 
	This is useful for containers that are never copy-constructed.  
	Upon copy-construction, the new copies will be NULL.  
	(Pseudo-copy-constructible)
	To rephrase yet another way, any struct or container with 
	sticky pointers will not be copyable, but the compiler
	will not complain about it, it will only manifest
	as a run-time error when you try to use a pointer copy
	that don't expect to be NULL.  
 */
template <class T, class Dealloc>
class sticky_ptr {
friend class pointer_manipulator;
template <class, class> friend class excl_ptr;
template <class, class> friend class some_ptr;
template <class> friend class never_ptr;
	typedef	sticky_ptr<T,Dealloc>	this_type;
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
	static const bool		sometimes_owns = false;
	static const bool		never_owns = true;
	// really wish for template typedefs!!!
	typedef	never_ptr<T>		nontransfer_cast_type;
	typedef	never_ptr<T>		transfer_cast_type;
	// never has ownership to transfer!
private:
	pointer				ptr;

	const pointer&
	base_pointer(void) const { return ptr; }

	pointer
	release(void) throw() {
		T* ret = this->ptr;
		this->ptr = NULL;
		return ret;
	}

	void
	reset(T* p = NULL) throw() {
		if (this->ptr) {
			INVARIANT(this->ptr != p);
			// violation of exclusion!
			// delete this->ptr;
			deallocation_policy()(this->ptr);
		}
		this->ptr = p;
	}

public:
// constructors
	sticky_ptr() : ptr(NULL) { }

	explicit
	sticky_ptr(T* p) : ptr(p) { }

	template <class S>
	explicit
	sticky_ptr(S* p) : ptr(p) { }

	/**
		Copy constructor never transfers ownership.  
	 */
	template <class S>
	sticky_ptr(const sticky_ptr<S, deallocation_policy>& p) : ptr(NULL) { }

#if 0
	/**
		Will acquire ownership from non-const excl_ptr.  
	 */
	template <class S>
	sticky_ptr(excl_ptr<S>& p) : ptr(p.release()) { }
#endif

	template <class S>
	sticky_ptr(excl_ptr<S, deallocation_policy>& p) :
		ptr(p.release()) { INVARIANT(!p); }

	~sticky_ptr() { this->reset(); }

	/**
		\return a deep copy of this, using operator new to allocate.
	 */
	this_type
	deep_copy(void) const {
		return this_type(new T(*ptr));
	}

	reference
	operator * () const { EXCL_PTR_NEVER_NULL(ptr); return *ptr; }

	pointer
	operator -> () const { EXCL_PTR_NEVER_NULL(ptr); return ptr; }

	/**
		Sticky pointers cannot steal ownership from each other.  
	 */
	template <class S>
	sticky_ptr&
	operator = (const sticky_ptr<S, deallocation_policy>& p) {
		return *this;
	}

	template <class S>
	sticky_ptr&
	operator = (excl_ptr<S, deallocation_policy>& p) {
		this->reset(p.release());
		INVARIANT(!p);
		return *this;
	}

	operator bool () const { return ptr != NULL; }

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

	// TODO: finish me...

// friends for special cases
template <class _T1, class _T2, class _DA >
friend
void
std::_Construct(sticky_ptr<_T1, _DA>*, const sticky_ptr<_T2, _DA>& );

	/**
		We tentatively disallow rebinding of the allocator policy.
	 */
	template <class S>
	struct rebind {
		typedef	sticky_ptr<S, deallocation_policy>	type;
	};

};	// end class sticky_ptr

//=============================================================================
/**
	Pointer that is NEVER to be deleted, even if delete is called on it.  
	For safety (and inconvenience), forbid direct interaction with
	naked pointers.  
 */
template <class T>
class never_ptr {
friend class pointer_manipulator;
template <class, class> friend class excl_ptr;
template <class, class> friend class some_ptr;
template <class> friend class never_ptr;
	typedef	never_ptr<T>		this_type;
public:
	typedef	this_type		type;
	typedef	T			element_type;
	typedef	T&			reference;
	typedef	T*			pointer;
	typedef	never_owner_pointer_tag	pointer_category;
	static const bool		is_array = false;
	static const bool		is_intrusive = false;
	static const bool		is_counted = false;
	static const bool		is_shared = true;
	static const bool		always_owns = false;
	static const bool		sometimes_owns = false;
	static const bool		never_owns = true;
	// really wish for template typedefs!!!
	typedef	never_ptr<T>		nontransfer_cast_type;
	typedef	never_ptr<T>		transfer_cast_type;
	// never has ownership to transfer!
private:
	pointer				ptr;

	/**
		Accessor provided as a method for consistency
		with standard pointer operations.  
	 */
	const pointer&
	base_pointer(void) const { return ptr; }

public:
// private:
/**
	Constructors that use naked pointers *should* be restricted.  
	But situations arise where this constructor needs to be publicly
	available, such as when converting and casting between types.  
 */
	explicit
	never_ptr(T* p) throw() : ptr(p) { }

#if 0
public:		// public ok for dynamic cast?
template <class S>
explicit never_ptr(S* p) throw() : base_ptr<T>(dynamic_cast<T*>(p)) { }
#endif

public:
/**
	Ownership query, always returns false.
 */
	bool
	owned(void) const { return false; }

/**
	The only stipulation on p is that someone ELSE has the responsibility 
	for deleting the pointer.  
	Or should we forbid construction with naked pointers, 
		and limit arguments to possible owners?
 */
	explicit
	never_ptr(void) throw() : ptr(NULL) { }
// constructors with same element type

// this constructor covers the following two
/**
	It should be always safe to implicitly convert any base_ptr
	to a never_ptr.  
	It will never modify the argument.  
	This covers all pointer types.  
	It is an error on the user's part to hold onto a never_ptr
	that refers to expired memory!  
 */
	template <class Dealloc>
	never_ptr(const excl_ptr<T,Dealloc>& p) throw() : ptr(p.ptr) { }

	template <class S, class Dealloc>
	never_ptr(const excl_ptr<S,Dealloc>& p) throw() : ptr(p.ptr) { }

	// defined below
	template <class Dealloc>
	never_ptr(const some_ptr<T, Dealloc>& p) throw();

	template <class S, class Dealloc>
	never_ptr(const some_ptr<S, Dealloc>& p) throw();

	// copy-ctor
	never_ptr(const this_type& p) throw() : ptr(p.ptr) { }

	template <class S>
	never_ptr(const never_ptr<S>& p) throw() : ptr(p.ptr) { }

/*** cross-template member not friendly accessible (p is private)
template <class S>
	explicit
	never_ptr(const excl_ptr<S>& p) throw() : 
		base_ptr<T>(dynamic_cast<T*>(p.ptr)) { }
***/

	~never_ptr() { }

	/**
		\return reference at the pointer.  
	 */
	reference
	operator * () const throw() { EXCL_PTR_NEVER_NULL(ptr); return *ptr; }

	/**
		Pointer member/method dereference, unchecked.  
		\returns the pointer.  
	 */
	pointer
	operator -> () const throw() { EXCL_PTR_NEVER_NULL(ptr); return ptr; }

	operator bool() const { return ptr != NULL; }

#if 0
	TODO: relay on pointer_traits for this.
// also want cross-type versions?
	never_ptr<T>&
	operator = (const base_ptr<T>& e) throw() {
		this->ptr = e.ptr;
		return *this;
	}

/** safe up-cast */
	template <class S>
	never_ptr<T>&
	operator = (const base_ptr<S>& e) throw() {
		this->ptr = e.ptr;	// static up-cast
		return *this;
	}
#endif

	template <class Dealloc>
	never_ptr(excl_ptr_ref<T,Dealloc> r) throw() : ptr(r.ptr) { }

	// is this obsolete?
	template <class Dealloc>
	this_type&
	operator = (excl_ptr_ref<T, Dealloc> r) throw() {
		this->ptr = r.ptr;
		return *this;
	}

	// this needs to be generalized... to avoid
	// replicating for every pointer class
	// somehow leverage traits
	template <class S>
	this_type&
	operator = (const never_ptr<S>& p) {
		ptr = p.ptr;
		return *this;
	}

// should some operators have const?

#if 0
WRONG! cannot create an excl_ptr from a never_ptr!!!
	// safe type-casting
	template <class S>
	operator excl_ptr_ref<S>() throw() {
		return excl_ptr_ref<S>(this->ptr);
	}
#endif

	// type casting
	template <class S>
	never_ptr<S>
	is_a(void) const;

	/**
		Dynamic cast assertion.  
		No return value.  
	 */
	template <class S>
	void
	must_be_a(void) const {
		INVARIANT(dynamic_cast<S*>(this->ptr));
	}

	/**
		Safe up-cast.  
		Notice that this has constant semantics, unlike excl_ptr's.  
	 */
	template <class S>
	never_ptr<S>
	as_a(void) const {
		return never_ptr<S>(static_cast<S*>(this->ptr));
			// shouldn't be necessary to static_cast
			// but this make it clear what it's intended for
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

	/**
		We tentatively disallow rebinding of the allocator policy.
	 */
	template <class S>
	struct rebind {
		typedef	never_ptr<S>		type;
	};

};	// end class never_ptr

//=============================================================================
/**
	Pointer class with dynamic ownership.  
	Is sometimes an excl ptr, sometimes a never_ptr.  
	Simply uses an extra bool to keep track.  
	Consider trick, use LSB of address and alignment?
 */
template <class T, class Dealloc>
class some_ptr {
friend class pointer_manipulator;
template <class, class> friend class excl_ptr;
template <class, class> friend class some_ptr;
template <class> friend class never_ptr;
	typedef	some_ptr<T,Dealloc>	this_type;
public:
	typedef	this_type		type;
	typedef	T			element_type;
	typedef	Dealloc			deallocation_policy;
	typedef	T&			reference;
	typedef	T*			pointer;
	typedef	sometimes_owner_pointer_tag	pointer_category;
	static const bool		is_array = false;
	static const bool		is_intrusive = false;
	static const bool		is_counted = false;
	static const bool		is_shared = true;
	static const bool		always_owns = false;
	static const bool		sometimes_owns = true;
	static const bool		never_owns = false;
	// really wish for template typedefs!!!
	typedef	never_ptr<T>		nontransfer_cast_type;
	typedef	excl_ptr<T,deallocation_policy>
					transfer_cast_type;
private:
	pointer				ptr;
	bool				own;

private:
	/**
		Accessor provided as a method for consistency
		with standard pointer operations.  
	 */
	const pointer&
	base_pointer(void) const { return ptr; }

	/**
		Returns pointer even when not owned.  
		Leaves pointer intact when relinquishing ownership.  
		\return pointer.  
	 */
	pointer
	release(void) {
		this->own = false;
		return this->ptr;
	}

	/**
		Frees memory if valid pointer was owned and 
		resets with ownership of new pointer, if not NULL.  
		\param b new ownership status.  
		\param p new pointer, owned.  
	 */
	void
	reset(const bool b, T* p = NULL) {
		if (this->own && this->ptr) {
			// delete this->ptr;
			deallocation_policy()(this->ptr);
		}
		this->ptr = p;
		this->own = b && p != NULL;
	}
public:
	/**
		Ownership query.  
	 */
	bool
	owned(void) const { return own; }

	/**
		Default constructor, initializes to NULL;
	 */
	some_ptr(void) : ptr(NULL), own(false) { }

	/**
		Never steals ownership, shares by default.  
		Ownership can only be transferred by explicit conversion
		to an excl_ptr.  
		TODO: own should come from pointer_traits
	 */
	some_ptr(const never_ptr<T>& p) : ptr(p.ptr), own(false) { }
	some_ptr(const this_type& p) : ptr(p.ptr), own(false) { }

	template <class S>
	some_ptr(const some_ptr<S, deallocation_policy>& p) :
		ptr(p.ptr), own(false) { }
	// need some constructor where this owns the pointer...

	/**
		Take ownership from an excl_ptr, which nullifies it.  
		Of course, we only own it if it's not NULL.  
	 */
	explicit
	some_ptr(excl_ptr<T, deallocation_policy>& p) :
		ptr(p.release()), own(this->ptr != NULL) {
		INVARIANT(!p);		// just to make sure
	}

	template <class S>
	explicit
	some_ptr(excl_ptr<S, deallocation_policy>& p) :
		ptr(p.release()), own(this->ptr != NULL) {
		INVARIANT(!p);		// just to make sure
	}

// explicit some_ptr(const excl_ptr<T>& p);

	/**
		Conditional destructor.  De-allocates only if it owns.  
	 */
	~some_ptr(void) {
		// same as reset(), but shorter
		if (this->ptr && this->own) {
			deallocation_policy()(this->ptr);
			// delete this->ptr;
		}
	}

	/**
		\return reference at the pointer.  
	 */
	reference
	operator * () const throw() { EXCL_PTR_NEVER_NULL(ptr); return *ptr; }

	/**
		Pointer member/method dereference, unchecked.  
		\returns the pointer.  
	 */
	pointer
	operator -> () const throw() { EXCL_PTR_NEVER_NULL(ptr); return ptr; }

	operator bool() const { return ptr != NULL; }

// cannot accept naked pointer, must wrap with excl or never pointer
// explicit some_ptr(T* p) : base_ptr<T>(ptr), own(true) { }

// conversion operators:
#if 0
// copied from auto_ptr_ref
	some_ptr(base_some_ptr_ref<T> r) throw() : ptr(r.ptr), own(r.own) { }
#endif

	this_type&
	operator = (const never_ptr<T>& p) throw() {
		this->reset(false, p.ptr);
		return *this;
	}

	template <class S>
	this_type&
	operator = (const never_ptr<S>& p) throw() {
		this->reset(false, p.ptr);
		return *this;
	}

	// copy-ctor
	this_type&
	operator = (const this_type& p) throw() {
		this->reset(false, p.ptr);
		return *this;
	}

	/**
		Use assignment operator to transfer.  
		Transfers ownership.  
	 */
	this_type&
	operator = (this_type& p) throw() {
		const bool o = p.own;
		const pointer t(p.release());
		// forced sequence point
		this->reset(o, t);
		return *this;
	}

#if 0
	/**
		Doesn't transfer ownership.
	 */
	some_ptr<T>&
	operator = (const excl_ptr<T>& p) throw() {
		this->reset(false, p.ptr);
		return *this;
	}
#endif

	/**
		Transfers ownership.
		Owns if not null.  
	 */
	this_type&
	operator = (excl_ptr<T, deallocation_policy>& p) throw() {
		const pointer t(p.release());
		// forced sequence point
		INVARIANT(!p);
		this->reset(t, t);
		return *this;
	}

	/**
		Transfers ownership.
	 */
	template <class S>
	some_ptr<T, deallocation_policy>&
	operator = (excl_ptr<S, deallocation_policy>& p) throw() {
		const pointer t(p.release());
		// forced sequence point
		INVARIANT(!p);
		this->reset(t, t);
		return *this;
	}

#if 0
	/**
		With transfer of ownership.
	 */
	some_ptr<T>&
	operator = (base_some_ptr_ref<T> r) throw() {
		this->reset(r.own, r.ptr);
		r.own = false;
		return *this;
	}
#endif

#if 0
	// pedantic: ambiguous conversion from some_ptr to never_ptr
	//	by default, prefers constructor.  
	operator never_ptr<T> () const throw() {
		return never_ptr<T>(this->ptr);
	}
#endif

#if 0
	// safe type-casting
	template <class S>
	operator base_some_ptr_ref<S>() throw() {
		bool o = own;
		own = false;
		return base_some_ptr_ref<S>(this->ptr, o);
	}
#endif

	/**
		Dynamic type-cast.  
	 */
	template <class S>
	never_ptr<S>
	is_a(void) const;

	/**
		Dynamic cast assertion.  
		No return value.  
	 */
	template <class S>
	void
	must_be_a(void) const {
		INVARIANT(dynamic_cast<S*>(this->ptr));
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

	/**
		We tentatively disallow rebinding of the allocator policy.
	 */
	template <class S>
	struct rebind {
		typedef	some_ptr<S, deallocation_policy>	type;
	};

};	// end class some_ptr

//=============================================================================
EXCL_PTR_TEMPLATE_SIGNATURE
// inline
EXCL_PTR_TEMPLATE_CLASS::excl_ptr(some_ptr<T,Dealloc>& s) throw() : ptr(s.ptr) {
	INVARIANT(s.own);		// else it didn't own it before!
	s.own = false;
}

template <class T>
template <class Dealloc>
// inline
never_ptr<T>::never_ptr(const some_ptr<T,Dealloc>& p) throw() : ptr(p.ptr) { }

template <class T>
template <class S, class Dealloc>
// inline
never_ptr<T>::never_ptr(const some_ptr<S,Dealloc>& p) throw() : ptr(p.ptr) { }

//-----------------------------------------------------------------------------
EXCL_PTR_TEMPLATE_SIGNATURE
template <class S>
// inline
never_ptr<S>
EXCL_PTR_TEMPLATE_CLASS::is_a(void) const {
	return never_ptr<S>(dynamic_cast<S*>(this->ptr));
	// uses cross-type pointer constructor with dynamic cast
}

EXCL_PTR_TEMPLATE_SIGNATURE
template <class S>
inline
excl_ptr<S,Dealloc>
EXCL_PTR_TEMPLATE_CLASS::is_a_xfer(void) {
	S* s = dynamic_cast<S*>(this->ptr);
	// momentary violation of exclusion
	if (s) release();
	return excl_ptr<S,Dealloc>(s);
	// uses cross-type pointer constructor with dynamic cast
}

EXCL_PTR_TEMPLATE_SIGNATURE
template <class S>
inline
never_ptr<S>
EXCL_PTR_TEMPLATE_CLASS::as_a(void) const {
	return never_ptr<S>(static_cast<S*>(this->ptr));
}

EXCL_PTR_TEMPLATE_SIGNATURE
template <class S>
inline
excl_ptr<S, Dealloc>
EXCL_PTR_TEMPLATE_CLASS::as_a_xfer(void) {
	return excl_ptr<S, Dealloc>(static_cast<S*>(this->release()));
}

template <class T>
template <class S>
inline
never_ptr<S>
never_ptr<T>::is_a(void) const {
	return never_ptr<S>(dynamic_cast<S*>(this->ptr));
	// uses cross-type pointer constructor with dynamic cast
}

template <class T, class Dealloc>
template <class S>
inline
never_ptr<S>
some_ptr<T,Dealloc>::is_a(void) const {
	return never_ptr<S>(dynamic_cast<S*>(this->ptr));
	// uses cross-type pointer constructor with dynamic cast
}

//=============================================================================
// standard pointer comparison operators
// don't want to have to write this for every combination...
// however, we only want this to apply to template classes

#if 0
template <class P1, class P2>
bool
pointer_equal(const P1& p1, const P2& p2) {
	return p1.the_pointer() == p2.the_pointer();
}
#endif

#if 0
template <template <class> class P1, class T1, 
	template <class> class P2, class T2>
bool
operator == (const P1<T1>& p1, const P2<T2>& p2) {

}
#endif

//=============================================================================
// pointer traits specializations

SPECIALIZE_ALL_POINTER_TRAITS_2(excl_ptr)
SPECIALIZE_ALL_POINTER_TRAITS_2(some_ptr)
SPECIALIZE_ALL_POINTER_TRAITS(never_ptr)
SPECIALIZE_ALL_POINTER_TRAITS_2(sticky_ptr)

//=============================================================================
}	// end namespace memory
}	// end namespace util

//=============================================================================
namespace std {
using util::memory::sticky_ptr;

// just a declaration before definition
template <class _T1, class _T2, class Dealloc>
void
_Construct(sticky_ptr<_T1,Dealloc>*, const sticky_ptr<_T2,Dealloc>&);

/**
	Attempt to specialize/overload std::_Construct for special-case
	behavior when placement-constructing sticky pointers.  
	This gives standard containers destructive copy semantics.  
 */
// don't allow conversions by default, this also also causes
// amibugity w.r.t. the next specialization...
template <class _T1, class _T2, class Dealloc>
// inline
void
_Construct(sticky_ptr<_T1,Dealloc>* __p,
		const sticky_ptr<_T2,Dealloc>& __value) {
	typedef	sticky_ptr<_T1,Dealloc>		ptr1_type;
	typedef	sticky_ptr<_T2,Dealloc>		ptr2_type;
	new (static_cast<void*>(__p))
		ptr1_type(const_cast<ptr2_type&>(__value).release());
}

#define	INSTANTIATE_STD_CONSTRUCT_STICKY_PTR_2(T1, T2, D)		\
template void _Construct(sticky_ptr<T1,D>*, const sticky_ptr<T2,D>&);

//=============================================================================
}	// end namespace std

#undef	EXCL_PTR_TEMPLATE_SIGNATURE
#undef	EXCL_PTR_TEMPLATE_CLASS

#endif	//	__UTIL_MEMORY_EXCL_PTR_H__

