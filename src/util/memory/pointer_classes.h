/**
	\file "excl_ptr.h"
	Library of pointers for explicit memory management.  
	The pointer classes contained herein are all non-counted.  
	For reference-counted classes, see "count_ptr.h". 

	Initially just copied from ptrs.h, but needs to be overhauled:
	eliminate derivation from abstract ptr class.  
	Eliminate const- duplication, should be able to
	specialize as const T if necessary?

	Maybe define pointer-like concept (like iterator).  

	Be able to attach pointer to allocator? oooooo....
	Be able to pass pointers between regions?  maybe not...

	$Id: pointer_classes.h,v 1.6 2005/01/12 03:20:02 fang Exp $
 */
// all methods in this file are to be defined here, to be inlined

// to do: references! (as non-null pointers)

// major issue: pointer type conversion emulation with classes
// friends and templates:
//	pointer classes can't access each other private constructors
//	because can't template friend classes, no generalization... :(

// lock pointers? non-transferrable exclusive pointers.  

#ifndef	__POINTER_CLASSES_H__
#define	__POINTER_CLASSES_H__

#include "config.h"
#if HAVE_STDDEF_H
#include <stddef.h>			// for size_t
#endif

#include "macros.h"
#include "memory/pointer_classes_fwd.h"
#include "memory/pointer_traits.h"
#include "STL/construct_fwd.h"

//=============================================================================
// debugging stuff

// for debugging strange behavior when excl_ptr destructor is "correct"
// predefine as 1 to override
#ifndef	FIX_RESET
#define	FIX_RESET	1
#endif

//=============================================================================

namespace util {
// forward declaration only
class persistent_object_manager;

/**
	The namespace for pointer-classes, written by Fang.  
	The classes contained herein offer efficient light-weight
	pointer management with static type-checking.  
 */
namespace memory {
//=============================================================================

/**
	Not a class for objects, but rather, a private namespace
	for functions to be trusted with pointer manipulations.  
 */
class pointer_manipulator {
// declared and defined in "persistent_object_manager.h"
friend class persistent_object_manager;
private:
	/**
		\param T is a raw pointer type.
	 */
	template <class T>
	inline
	static
	const T&
	__get_pointer(const T& p, raw_pointer_tag) {
		return p;
	}

	/**
		\param T is any pointer class type.  
	 */
	template <class T>
	inline
	static
	const typename T::pointer&
	__get_pointer(const T& p, pointer_class_base_tag) {
		return p.base_pointer();
	}

	template <class T>
	inline
	static
	const typename T::pointer&
	get_pointer(const T& p) {
		return __get_pointer(p, __pointer_category(p));
	}

public:
	/**
		\return true if pointers are equal.
	 */
	template <class P1, class P2>
	inline
	static
	bool
	compare_pointers_equal(const P1& p1, const P2& p2) {
#if 0
		// should work, but compiler can't find get_pointer?
//		return get_pointer(p1) == get_pointer(p2);
		return get_pointer<P1>(p1) == get_pointer<P2>(p2);
#else
		return (__get_pointer(p1, __pointer_category(p1)) ==
			__get_pointer(p2, __pointer_category(p2)));
#endif
	}

public:
	/**
		\return true if pointers are unequal.
	 */
	template <class P1, class P2>
	inline
	static
	bool
	compare_pointers_unequal(const P1& p1, const P2& p2) {
#if 0
		// should work, but compiler can't find get_pointer?
//		return get_pointer(p1) != get_pointer(p2);
		return get_pointer<P1>(p1) != get_pointer<P2>(p2);
#else
		return (__get_pointer(p1, __pointer_category(p1)) !=
			__get_pointer(p2, __pointer_category(p2)));
#endif
	}

};	// end class pointer_manipulator

//=============================================================================
#if 0
/**
	Grand ancestral abstract pointer.
	Trying to get friend class to work across template instances.  
	Template members.  
 */
class abstract_ptr {
public:
virtual	~abstract_ptr() { }

protected:
// but a member template cannot be virtual... *sigh*
// template <class T>
// virtual	T* get_temp(void) const = 0;

// must settle with the following:
virtual	void* void_ptr(void) const = 0;
virtual	const void* const_void_ptr(void) const = 0;

public:
	operator bool() const { return const_void_ptr() != NULL; }

	/**
		Comparing against raw pointers' addresses.  
	 */
	bool operator == (const void* p) const
		{ return const_void_ptr() == p; }
	bool operator != (const void* p) const
		{ return const_void_ptr() != p; }
	/**
		Pointer comparison for pointer classes.  
		Passing by value, because by reference is slower, 
		adds an implicit indirection.  
		\param p must be a reference because abstract_ptr is abstract.
		\return truth of comparison.  
	 */
	bool operator == (const abstract_ptr& p) const
		{ return const_void_ptr() == p.const_void_ptr(); }
	bool operator != (const abstract_ptr& p) const
		{ return const_void_ptr() != p.const_void_ptr(); }


virtual	bool owned(void) const = 0;
};	// end class abstract_ptr

//=============================================================================
/**
	Mother pointer.  
	No public constructors, only public methods are inherited.  
 */
template <class T>
class base_ptr : public abstract_ptr {
template <class> friend class excl_ptr;
template <class> friend class never_ptr;
template <class> friend class excl_const_ptr;
template <class> friend class never_const_ptr;
template <class> friend class some_ptr;
template <class> friend class some_const_ptr;

public:
	typedef	T			element_type;
	typedef	T&			reference;
	typedef	T*			pointer;
	typedef	const T&		const_reference;
	typedef	const T*		const_pointer;

protected:
	T*	ptr;

protected:
	void* void_ptr(void) const { return ptr; }
	const void* const_void_ptr(void) const { return ptr; }
		// consume with a reinterpret_cast?

explicit base_ptr(T* p) throw() : abstract_ptr(), ptr(p) { }

public:
/**
	This destructor does nothing, it is up to derived classes to act.  
 */
virtual	~base_ptr() { }

/**
	Dereference and return reference, just like a pointer.  
	The assertion adds a little overhead.  
	\return reference to the pointed element. 
 */
T&	operator * () const throw() { INVARIANT(ptr); return *ptr; }

/**
	Pointer member/method dereference, unchecked.  
	\returns the pointer.  
 */
T*	operator -> () const throw() { INVARIANT(ptr); return ptr; }


// I give up, completely protecting raw pointer from the external abuse
// is practically impossible because friends don't work across different
// instances of template, i.e. there are no template-general friends.  

/**
	"Unsafe" because it is public and vulnerable to abuse of the
	returned pointer.  
	Don't use this unless you KNOW what you're doing.  
 */
T*	unprotected_ptr(void) const { return ptr; }
const T*	unprotected_const_ptr(void) const { return ptr; }

	/**
		Should be pure virtual...
	 */
virtual	bool owned(void) const = 0;
};	// end class base_ptr

#endif

//=============================================================================
/**
	Helper class for type-casting.  
	Is private to the outside world, only usable by template friends.  
 */
template <class S>
class excl_ptr_ref {
template <class> friend class excl_ptr;
template <class> friend class some_ptr;
template <class> friend class never_ptr;
private:
	S*	ptr;
	explicit	excl_ptr_ref(S* p) throw() : ptr(p) { }
};	// end struct excl_ptr_ref

//=============================================================================
#if 0
/**
	Helper class for type-casting for the some_ptr class.  
 */
// really don't want this visible to the outside...
template <class S>
struct base_some_ptr_ref {
	S*	ptr;
	bool	own;
	explicit	base_some_ptr_ref(S* p, bool o) throw() :
				ptr(p), own(o) { }
};	// end struct base_some_ptr_ref

#endif

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
 */
template <class T>
class excl_ptr {
friend class pointer_manipulator;
template <class> friend class excl_ptr;
template <class> friend class sticky_ptr;
template <class> friend class never_ptr;
template <class> friend class some_ptr;

public:
	typedef	T			element_type;
	typedef	T&			reference;
	typedef	T*			pointer;
	typedef	single_owner_pointer_tag	pointer_category;
	static const bool		is_array = false;
	static const bool		is_intrusive = false;
	static const bool		is_counted = false;
	static const bool		is_shared = true;
	static const bool		always_owns = true;
	static const bool		sometimes_owns = true;
	static const bool		never_owns = false;
	// really wish for template typedefs!!!
	typedef	never_ptr<T>		nontransfer_cast_type;
	typedef	excl_ptr<T>		transfer_cast_type;

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
		T* ret = this->ptr;
		this->ptr = NULL;
		return ret;
	}

/**
	Sets the member pointer, deleting the previous value
	if appropriate.  
	Must be private, not publicly available.  
	\param p non-const pointer that is exclusively owned.  
 */
	void
	reset(T* p = NULL) throw() {
		if (this->ptr) {
			INVARIANT(this->ptr != p);
			// violation of exclusion!
			delete this->ptr;
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
 */
	explicit
	excl_ptr(T* p) throw() : ptr(p) { }

	excl_ptr(void) throw() : ptr(NULL) { }

/**
	Destructive copy constructor, non-const source.  
	Transfers ownership.  
	To make non-transferrable, declare as const excl_ptr<>.  
	This is not redundant, need this.  
 */
	excl_ptr(excl_ptr<T>& e) throw() : ptr(e.release()) { }

/**
	For safe up-cast, pointer constructor.  
 */
	template <class S>
	excl_ptr(excl_ptr<S>& e) throw() : ptr(e.release()) { }

/**
	Transfer from some_ptr, changes ownership, while leaving
	the argument's pointer intact.  
 */
	explicit
	excl_ptr(some_ptr<T>& s) throw();

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

/**
	De-allocates memory.  
	Should be safe to delete NULL?
	non-virtual destructor => don't use inheritance with pointers 
	to pointer-classes.
 */
	// virtual
#if FIX_RESET
	~excl_ptr() { this->reset(); }
#else
	~excl_ptr() { this->release(); }	// release is wrong!
#endif

public:
	/**
		\return reference at the pointer.  
	 */
	reference
	operator * () const throw() { NEVER_NULL(ptr); return *ptr; }

	/**
		Pointer member/method dereference, unchecked.  
		\returns the pointer.  
	 */
	pointer
	operator -> () const throw() { NEVER_NULL(ptr); return ptr; }

	operator bool() const { return ptr != NULL; }

/**
	Will not accept const pointer of p.  
	Naked p MUST be an exclusive owner.  
	Forbidding direct interaction with naked pointer.  
excl_ptr<T>& operator = (T* p) throw() {
		reset(p);
		return *this;
	}
**/

	excl_ptr<T>&
	operator = (excl_ptr<T>& e) throw() {
		reset(e.release());
		return *this;
	}

	/**
		Acceptable transfer for safe-casts.  
	 */
	template <class S>
	excl_ptr<T>&
	operator = (excl_ptr<S>& e) throw() {
		reset(e.release());
		return *this;
	}


	// copied from auto_ptr_ref
	excl_ptr(excl_ptr_ref<T> r) throw() : ptr(r.ptr) { }

#if 1
	// not in book...
	template <class S>
	excl_ptr(excl_ptr_ref<S> r) throw() : ptr(r.ptr) { }
#endif

	excl_ptr<T>&
	operator = (excl_ptr_ref<T> r) throw() {
		reset(r.ptr);
		r.ptr = NULL;
		return *this;
	}

	/**
		Acquiring ownership from a some_ptr, destructive transfer.
		Transfer is conditional on the ownership of the source.  
	 */
	excl_ptr<T>&
	operator = (some_ptr<T>& r) throw() {
		if (r.owned())
			this->ptr = r.release();
		return *this;
	}

#if 0
	/**
		Does nothing!  can't acquire ownership from a const source.  
	 */
excl_ptr<T>& operator = (const some_ptr<T>& r) throw() { }
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
	operator excl_ptr<S> () throw() {
		return excl_ptr<S>(this->release());
	}

	// safe type-casting
	template <class S>
	operator excl_ptr_ref<S>() throw() {
		return excl_ptr_ref<S>(this->release());
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
	excl_ptr<S>		// transfer_cast_type
	is_a_xfer(void);

	/** static cast */
	template <class S>
	never_ptr<S>		// nontransfer_cast_type
	as_a(void) const;

	/** static cast with xfer */
	template <class S>
	excl_ptr<S>		// transfer_cast_type
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

#if 0
	/**
		Destructive transfer, safe up-cast.  
		constructor probably not available yet...
	 */
	template <class S>
	excl_ptr<S>&	as_a(void) {
		static_cast<S*>(this->ptr);	// safety check
			// potentially dangerous
		return reinterpret_cast<excl_ptr<S>&>(*this);
	}

	template <class S>
	excl_ptr<S>	as_a(void) {
		return excl_ptr<S>(static_cast<S*>(this->release()));
			// shouldn't be necessary to static_cast
			// but this make it clear what it's intended for
	}
#endif

// permissible assignments

// non-member functions

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
template <class T>
class sticky_ptr {
friend class pointer_manipulator;
template <class> friend class excl_ptr;
template <class> friend class some_ptr;
template <class> friend class never_ptr;

public:
	typedef	T			element_type;
	typedef	T&			reference;
	typedef	T*			pointer;
	typedef	single_owner_pointer_tag	pointer_category;
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
			delete this->ptr;
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
	sticky_ptr(const sticky_ptr<S>& p) : ptr(NULL) { }

	/**
		Will acquire ownership from non-const excl_ptr.  
	 */
#if 0
	template <class S>
	sticky_ptr(excl_ptr<S>& p) : ptr(p.release()) { }
#endif

	template <class S>
	sticky_ptr(excl_ptr<S>& p) : ptr(p.release()) { INVARIANT(!p); }

#if FIX_RESET
	~sticky_ptr() { this->reset(); }
#else
	~sticky_ptr() { this->release(); }
#endif

	reference
	operator * () const { NEVER_NULL(ptr); return *ptr; }

	pointer
	operator -> () const { NEVER_NULL(ptr); return ptr; }

	/**
		Sticky pointers cannot steal ownership from each other.  
	 */
	template <class S>
	sticky_ptr&
	operator = (const sticky_ptr<S>& p) {
		return *this;
	}

	template <class S>
	sticky_ptr&
	operator = (excl_ptr<S>& p) {
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
template <class _T1, class _T2>
friend
void
std::_Construct(sticky_ptr<_T1>*, const sticky_ptr<_T2>& );

};	// end class sticky_ptr

//=============================================================================
/**
	No copy of this pointer shall exist anywhere.  
 */
template <class T>
class unique_ptr {
friend class pointer_manipulator;
	// TODO: finish me!!!
};	// end class unique_ptr

//=============================================================================
/**
	Pointer that is NEVER to be deleted, even if delete is called on it.  
	For safety (and inconvenience), forbid direct interaction with
	naked pointers.  
 */
template <class T>
class never_ptr {
friend class pointer_manipulator;
template <class> friend class excl_ptr;
template <class> friend class some_ptr;
template <class> friend class never_ptr;

public:
	typedef	T			element_type;
	typedef	T&			reference;
	typedef	T*			pointer;
	typedef	single_owner_pointer_tag	pointer_category;
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
	never_ptr(const excl_ptr<T>& p) throw() : ptr(p.ptr) { }

	template <class S>
	never_ptr(const excl_ptr<S>& p) throw() : ptr(p.ptr) { }

	// defined below
	never_ptr(const some_ptr<T>& p) throw();

	template <class S>
	never_ptr(const some_ptr<S>& p) throw();

	never_ptr(const never_ptr<T>& p) throw() : ptr(p.ptr) { }

	template <class S>
	never_ptr(const never_ptr<S>& p) throw() : ptr(p.ptr) { }

/*** cross-template member not friendly accessible (p is private)
template <class S>
explicit never_ptr(const excl_ptr<S>& p) throw() : 
		base_ptr<T>(dynamic_cast<T*>(p.ptr)) { }
***/

	// virtual
	~never_ptr() { }

	/**
		\return reference at the pointer.  
	 */
	reference
	operator * () const throw() { NEVER_NULL(ptr); return *ptr; }

	/**
		Pointer member/method dereference, unchecked.  
		\returns the pointer.  
	 */
	pointer
	operator -> () const throw() { NEVER_NULL(ptr); return ptr; }

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

	never_ptr(excl_ptr_ref<T> r) throw() : ptr(r.ptr) { }

	// is this obsolete?
	never_ptr<T>&
	operator = (excl_ptr_ref<T> r) throw() {
		this->ptr = r.ptr;
		return *this;
	}

	// this needs to be generalized... to avoid
	// replicating for every pointer class
	// somehow leverage traits
	template <class S>
	never_ptr<T>&
	operator = (const never_ptr<S>& p) {
		ptr = p.ptr;
		return *this;
	}

// should some operators have const?

	// safe type-casting
	template <class S>
	operator excl_ptr_ref<S>() throw() {
		return excl_ptr_ref<S>(this->ptr);
	}

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


// non-member functions
};	// end class never_ptr


//=============================================================================
/**
	Pointer class with dynamic ownership.  
	Is sometimes an excl ptr, sometimes a never_ptr.  
	Simply uses an extra bool to keep track.  
	Consider trick, use LSB of address and alignment?
 */
template <class T>
class some_ptr {
friend class pointer_manipulator;
template <class> friend class excl_ptr;
template <class> friend class some_ptr;
template <class> friend class never_ptr;

public:
	typedef	T			element_type;
	typedef	T&			reference;
	typedef	T*			pointer;
	typedef	single_owner_pointer_tag	pointer_category;
	static const bool		is_array = false;
	static const bool		is_intrusive = false;
	static const bool		is_counted = false;
	static const bool		is_shared = true;
	static const bool		always_owns = false;
	static const bool		sometimes_owns = true;
	static const bool		never_owns = false;
	// really wish for template typedefs!!!
	typedef	never_ptr<T>		nontransfer_cast_type;
	typedef	excl_ptr<T>		transfer_cast_type;

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
		if (this->own && this->ptr)
			delete this->ptr;
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
	some_ptr(const some_ptr<T>& p) : ptr(p.ptr), own(false) { }

	template <class S>
	some_ptr(const some_ptr<S>& p) : ptr(p.ptr), own(false) { }
	// need some constructor where this owns the pointer...

	/**
		Take ownership from an excl_ptr, which nullifies it.  
		Of course, we only own it if it's not NULL.  
	 */
	explicit
	some_ptr(excl_ptr<T>& p) : ptr(p.release()), own(this->ptr != NULL) {
		INVARIANT(!p);		// just to make sure
	}

	template <class S>
	explicit
	some_ptr(excl_ptr<S>& p) : ptr(p.release()), own(this->ptr != NULL) {
		INVARIANT(!p);		// just to make sure
	}

// explicit some_ptr(const excl_ptr<T>& p);

	/**
		Conditional destructor.  De-allocates only if it owns.  
	 */
	~some_ptr(void) {
		// same as reset(), but shorter
		if (this->ptr && this->own)
			delete this->ptr;
	}

	/**
		\return reference at the pointer.  
	 */
	reference
	operator * () const throw() { NEVER_NULL(ptr); return *ptr; }

	/**
		Pointer member/method dereference, unchecked.  
		\returns the pointer.  
	 */
	pointer
	operator -> () const throw() { NEVER_NULL(ptr); return ptr; }

	operator bool() const { return ptr != NULL; }

// cannot accept naked pointer, must wrap with excl or never pointer
// explicit some_ptr(T* p) : base_ptr<T>(ptr), own(true) { }

// conversion operators:
#if 0
// copied from auto_ptr_ref
	some_ptr(base_some_ptr_ref<T> r) throw() : ptr(r.ptr), own(r.own) { }
#endif

	some_ptr<T>&
	operator = (const never_ptr<T>& p) throw() {
		this->reset(false, p.ptr);
		return *this;
	}

	template <class S>
	some_ptr<T>&
	operator = (const never_ptr<S>& p) throw() {
		this->reset(false, p.ptr);
		return *this;
	}

	some_ptr<T>&
	operator = (const some_ptr<T>& p) throw() {
		this->reset(false, p.ptr);
		return *this;
	}

	/**
		Transfers ownership.  
	 */
	some_ptr<T>&
	operator = (some_ptr<T>& p) throw() {
		this->reset(p.own, p.release());
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
	 */
	some_ptr<T>&
	operator = (excl_ptr<T>& p) throw() {
		this->reset(p.ptr != NULL, p.release());
		INVARIANT(!p);
		return *this;
	}

	/**
		Transfers ownership.
	 */
	template <class S>
	some_ptr<T>&
	operator = (excl_ptr<S>& p) throw() {
		this->reset(p.ptr != NULL, p.release());
		INVARIANT(!p);
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

	operator never_ptr<T> () const throw() {
		return never_ptr<T>(this->ptr);
	}

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

};	// end class some_ptr

//=============================================================================
template <class T>
excl_ptr<T>::excl_ptr(some_ptr<T>& s) throw() : ptr(s.ptr) {
	INVARIANT(s.own);		// else it didn't own it before!
	s.own = false;
}

template <class T>
never_ptr<T>::never_ptr(const some_ptr<T>& p) throw() : ptr(p.ptr) { }

template <class T>
template <class S>
never_ptr<T>::never_ptr(const some_ptr<S>& p) throw() : ptr(p.ptr) { }

//-----------------------------------------------------------------------------
#if 0
template <class T>
template <class S>
base_ptr<S> is_a(const base_ptr<T>& p) {
	return base_ptr<S>(dynamic_cast<S*>(p.ptr));
}
#endif

template <class T>
template <class S>
never_ptr<S>
excl_ptr<T>::is_a(void) const {
	return never_ptr<S>(dynamic_cast<S*>(this->ptr));
	// uses cross-type pointer constructor with dynamic cast
}

template <class T>
template <class S>
excl_ptr<S>
excl_ptr<T>::is_a_xfer(void) {
	S* s = dynamic_cast<S*>(this->ptr);
	// momentary violation of exclusion
	if (s) release();
	return excl_ptr<S>(s);
	// uses cross-type pointer constructor with dynamic cast
}

template <class T>
template <class S>
never_ptr<S>
excl_ptr<T>::as_a(void) const {
	return never_ptr<S>(static_cast<S*>(this->ptr));
}

template <class T>
template <class S>
excl_ptr<S>
excl_ptr<T>::as_a_xfer(void) {
	return excl_ptr<S>(static_cast<S*>(this->release()));
}

template <class T>
template <class S>
never_ptr<S>
never_ptr<T>::is_a(void) const {
	return never_ptr<S>(dynamic_cast<S*>(this->ptr));
	// uses cross-type pointer constructor with dynamic cast
}

template <class T>
template <class S>
never_ptr<S>
some_ptr<T>::is_a(void) const {
	return never_ptr<S>(dynamic_cast<S*>(this->ptr));
	// uses cross-type pointer constructor with dynamic cast
}

//=============================================================================
// standard pointer comparison operators
// don't want to have to write this for every combination...
// however, we only want this to apply to template classes

template <class P1, class P2>
bool
pointer_equal(const P1& p1, const P2& p2) {
	return p1.the_pointer() == p2.the_pointer();
}

#if 0
template <template <class> class P1, class T1, 
	template <class> class P2, class T2>
bool
operator == (const P1<T1>& p1, const P2<T2>& p2) {

}
#endif

/***
	Reference-counting pointer class templates.

	Generally not intended for mixed use with pointers from
	the non-counted pointer classes in "ptrs.h":
		excl_ptr, some_ptr, never_ptr.  
***/
//=============================================================================
#if 0
/**
	Base class for all reference-count pointers.  
 */
class abstract_base_count_ptr {
	// parent could be abstract_ptr
template <class> friend class count_ptr;

protected:
	/**
		The reference count.  Need not be mutable.  
	 */
	size_t*	ref_count;

protected:
	abstract_base_count_ptr(size_t* s = NULL) : ref_count(s) { }

	/**
		This destructor does absolutely nothing.  
		It is subclasses' responsibility to take care of 
		deleting ref_count when appropriate.  
	 */
virtual	~abstract_base_count_ptr() { }

	/**
		Raw pointer.  
	 */
virtual	void* void_ptr(void) const = 0;

	/**
		Raw pointer, const.  
	 */
virtual	const void* const_void_ptr(void) const = 0;

public:
	/**
		Reference count.  
	 */
	size_t refs(void) const {
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

	/**
		Ditches pointer without returning it, 
		maintaining reference count, deleting when appropriate.  
	 */
virtual	void abandon(void) = 0;
	/**
		Alias for abandon.
	 */
	void nullify(void) { abandon(); }
	void ditch(void) { abandon(); }
	void scrap(void) { abandon(); }

	/**
		Test for nullity.  
		Will overriding in sub-classes be faster, 
		without virtual call?
	 */
	operator bool () const { return const_void_ptr() != NULL; }

	bool operator == (const void* p) const
		{ return const_void_ptr() == p; }
	bool operator != (const void* p) const
		{ return const_void_ptr() != p; }
	bool operator == (const abstract_base_count_ptr& p) const
		{ return const_void_ptr() == p.const_void_ptr(); }
	bool operator != (const abstract_base_count_ptr& p) const
		{ return const_void_ptr() != p.const_void_ptr(); }

};	// end class abstract_base_count_ptr
#endif

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

#if 0
	void* void_ptr(void) const { return ptr; }
	const void* const_void_ptr(void) const { return ptr; }
#endif

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
		if (this->ref_count)
			(*this->ref_count)++;
	}

	/**
		Safe, up-cast copy constructor.  
		Still maintains reference count.  
	 */
	template <class S>
	count_ptr(const count_ptr<S>& c) :
			ptr(c.ptr), ref_count(c.ref_count) {
		if (this->ref_count)
			(*this->ref_count)++;
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
		} else
			INVARIANT(!c);		// counter must also be NULL
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
			(*this->ref_count)--;
			if (!*this->ref_count) {
				NEVER_NULL(ptr);
				delete ptr;
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
			(*this->ref_count)--;
			if (!*this->ref_count) {
				NEVER_NULL(ptr);
				delete ptr;
				delete this->ref_count;
			}
		} else {
			INVARIANT(!ptr);
		}
		ptr = p;
		this->ref_count = (ptr) ? c : NULL;
		if (ptr && this->ref_count)
			(*this->ref_count)++;
	}

public:
	/**
		Destructor, which conditionally deletes pointer if is
		last reference.  
	 */
	~count_ptr() { release(); }

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

//=============================================================================
}	// end namespace util

//=============================================================================
namespace std {
using util::memory::sticky_ptr;

/**
	Attempt to specialize/overload std::_Construct for special-case
	behavior when placement-constructing sticky pointers.  
 */
template <class _T1, class _T2>
inline void
_Construct(sticky_ptr<_T1>* __p, const sticky_ptr<_T2>& __value) {
	typedef	sticky_ptr<_T1>		ptr1_type;
	typedef	sticky_ptr<_T2>		ptr2_type;
	new (static_cast<void*>(__p))
		ptr1_type(const_cast<ptr2_type&>(__value).release());
}


}

#endif	//	__POINTER_CLASSES_H__


