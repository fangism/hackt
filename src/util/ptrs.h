// "ptrs.h" -- need better name?
// library of pointers for explicit memory management
// all methods in this file are to be defined here, to be inlined

// to do: references! (as non-null pointers)

// major issue: pointer type conversion emulation with classes
// friends and templates:
//	pointer classes can't access each other private/protected constructors
//	because can't template friend classes, no generalization... :(

// lock pointers? non-transferrable exclusive pointers.  

// documentation style is for doxygen

#ifndef	__PTRS_H__
#define	__PTRS_H__

#include <assert.h>
#include "ptrs_fwd.h"

//=============================================================================
#ifndef	NULL
#define	NULL	0
#endif

/**
	The namespace for pointer-classes, written by Fang.  
	The classes contained herein offer efficient light-weight
	pointer management with static type-checking.  
	The name of the namespace can be overridden.  
 */
namespace PTRS_NAMESPACE {
//=============================================================================
// forward declarations
// pointers with "const" are read-only, duh!

			class abstract_ptr;	// abstract pointer
template <class>	class base_ptr;		// generic pointer
template <class>	class base_const_ptr;	// generic pointer

#if 0
template <class>	class some_ptr;		// sometimes owner
template <class>	class some_const_ptr;	// sometimes owner
template <class>	class excl_ptr;		// exclusive owner and deleter
template <class>	class excl_const_ptr;	// exclusive owner and deleter
template <class>	class never_ptr;	// never owner
template <class>	class never_const_ptr;	// never owner
#endif

template <class>	class dynconst_ptr;	// dynamic const-ness

// never is a bad name...
// never_const is always const!

//=============================================================================
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
T&	operator * () const throw() { assert(ptr); return *ptr; }

/**
	Pointer member/method dereference, unchecked.  
	\returns the pointer.  
 */
T*	operator -> () const throw() { assert(ptr); return ptr; }


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

//=============================================================================

/**
	Read-only base pointer.  
	Explanation:
	const base_ptr means that base_ptr's pointer value never changes, 
		however the contents of that location may change by invocation
		of non-const methods.  
	base_const_ptr means that the pointer value may change, but the
		contents of the memory may not, and thus one is 
		restricted to const-preserving methods only.  
	const base_const_ptr means that the pointer value may not change, 
		nor may its contents.  
 */
template <class T>
class base_const_ptr : public abstract_ptr {
template <class> friend class never_ptr;
template <class> friend class never_const_ptr;
template <class> friend class some_ptr;
template <class> friend class some_const_ptr;

protected:
	const T*	cptr;

protected:
// want to avoid leaking out naked pointers...
//	const T* get(void) const { return cptr; }
	void* void_ptr(void) const { return NULL; }
	const void* const_void_ptr(void) const { return cptr; }

protected:
explicit base_const_ptr(const T* p) throw() : abstract_ptr(), cptr(p) { }

public:
/**
	This destructor does nothing, it is up to derived classes to act.  
 */
virtual	~base_const_ptr() { }

public:
/**
	Dereference and return reference, just like a pointer.  
	The assertion adds a little overhead.  
 */
const T&	operator * () const throw() { assert(cptr); return *cptr; }

const T*	operator -> () const throw() { assert(cptr); return cptr; }

/**
	"Unsafe" because it is public and vulnerable to abuse of the
	returned pointer.  
	Don't use this unless you KNOW what you're doing.  
 */
const T*	unprotected_const_ptr(void) const { return cptr; }

	/**
		Should be pure virtual...
	 */
virtual	bool owned(void) const = 0;
};	// end class base_const_ptr

//=============================================================================
/**
	Helper class for type-casting.  
 */
// really don't want this visible to the outside...
template <class S>
struct base_ptr_ref {
	S*	ptr;
	explicit	base_ptr_ref(S* p) throw() : ptr(p) { }

};	// end struct base_ptr_ref

//-----------------------------------------------------------------------------
/**
	Helper class for type-casting of read-only pointers.  
 */
template <class S>
struct base_const_ptr_ref {
	const S*	cptr;
	explicit	base_const_ptr_ref(const S* p) throw() : cptr(p) { }
	explicit	base_const_ptr_ref(const base_ptr_ref<S>& p) throw() :
				cptr(p.ptr) { }
};	// end struct base_const_ptr_ref

//=============================================================================
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

//-----------------------------------------------------------------------------
/**
	Helper class for type-casting of read-only pointers.  
 */
template <class S>
struct base_some_const_ptr_ref {
	const S*	cptr;
	bool		own;
	explicit	base_some_const_ptr_ref(const S* p, bool o) throw() :
				cptr(p), own(o) { }
	explicit	base_some_const_ptr_ref(
				const base_some_ptr_ref<S>& p) throw() :
				cptr(p.ptr), own(p.own) { }
};	// end struct base_some_const_ptr_ref

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
class excl_ptr : public virtual base_ptr<T> {
template <class> friend class excl_ptr;
template <class> friend class excl_const_ptr;
template <class> friend class never_ptr;
template <class> friend class never_const_ptr;
template <class> friend class some_ptr;
template <class> friend class some_const_ptr;

protected:
/**
	Relinquishes resposibility for deleting pointer, by returning a
	copy of it, and nullifying its own pointer.  
	Must remain protected, hence not publicly accessible.  
	\return a pointer that must be deleted by it consumer if not NULL.  
 */
T*	release(void) throw() {
		T* ret = this->ptr;
		this->ptr = NULL;
		return ret;
	}

/**
	Sets the member pointer, deleting the previous value
	if appropriate.  
	Must be protected, not publicly available.  
	\param p non-const pointer that is exclusively owned.  
 */
void	reset(T* p = NULL) throw() {
		if (this->ptr) {
			assert (this->ptr != p);
			// violation of exclusion!
			delete this->ptr;
		}
		this->ptr = p;
	}

public:
/**
	Ownership query.
 */
	bool owned(void) const { return this->ptr != NULL; }

/**
	\param p should be a newly allocated pointer, which has not
		been tampered with or leaked out.  Vulnerability
		here is that we don't know what else has been done with p.  
		Can we make new T return an excl_ptr?  No.
 */
explicit excl_ptr(T* p) throw() : base_ptr<T>(p) { }

	excl_ptr(void) throw() : base_ptr<T>(NULL) { }

/**
	Destructive copy constructor, non-const source.  
	Transfers ownership.  
	To make non-transferrable, declare as const excl_ptr<>.  
	This is not redundant, need this.  
 */
	excl_ptr(excl_ptr<T>& e) throw() : base_ptr<T>(e.release()) { }

/**
	For safe up-cast, pointer constructor.  
 */
	template <class S>
	excl_ptr(excl_ptr<S>& e) throw() : base_ptr<T>(e.release()) { }

/**
	Transfer from some_ptr, changes ownership, while leaving
	the argument's pointer intact.  
 */
explicit excl_ptr(some_ptr<T>& s) throw();

/**
	Bogus copy constructor, const source.  
	Doesn't actually copy source.  
	Need this to make some standard containers happy.  
	Copy constructor of containers will result in NULL members.  
	Preserves exclusion.  
	Consider: creating a container of never_ptrs from a container
		of excl_ptrs... possible to write such a template?
 */
explicit excl_ptr(const excl_ptr<T>& e) throw() : base_ptr<T>(NULL) {
		assert(!e);
		// issue a warning?
	}


/**
	De-allocates memory.  
	Should be safe to delete NULL?
 */
virtual	~excl_ptr() { release(); }


public:
/**
	Will not accept const pointer of p.  
	Naked p MUST be an exclusive owner.  
	Forbidding direct interaction with naked pointer.  
excl_ptr<T>& operator = (T* p) throw() {
		reset(p);
		return *this;
	}
**/

excl_ptr<T>& operator = (excl_ptr<T>& e) throw() {
		reset(e.release());
		return *this;
	}


// copied from auto_ptr_ref
	excl_ptr(base_ptr_ref<T> r) throw() : base_ptr<T>(r.ptr) { }

#if 0
	// not in book...
	template <class S>
	excl_ptr(base_ptr_ref<S> r) throw() : base_ptr<T>(r.ptr) { }
#endif

excl_ptr<T>& operator = (base_ptr_ref<T> r) throw() {
		reset(r.ptr);
		r.ptr = NULL;
		return *this;
	}

	/**
		Acquiring ownership from a some_ptr, destructive transfer.
		Transfer is conditional on the ownership of the source.  
	 */
excl_ptr<T>& operator = (some_ptr<T>& r) throw() {
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
	operator base_ptr_ref<S>() throw() {
		return base_ptr_ref<S>(this->release());
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
	template <class S>
	never_ptr<S>	is_a(void) const;

	/**
		Ownership transferring dynamic cast.  
	 */
	template <class S>
	excl_ptr<S>	is_a_xfer(void);

	/**
		Dynamic cast assertion.  
		No return value.  
	 */
	template <class S>
	void	must_be_a(void) const {
		assert(dynamic_cast<S*>(this->ptr));
	}

	/**
		Destructive transfer, safe up-cast.  
		constructor probably not available yet...
	 */
#if 0
	template <class S>
	excl_ptr<S>&	as_a(void) {
		static_cast<S*>(this->ptr);	// safety check
			// potentially dangerous
		return reinterpret_cast<excl_ptr<S>&>(*this);
	}
#else	// old, can't overload
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
	This pointer is the exclusive owner of the pointed memory, 
	no one else may delete it.  Others who use it must promise
	not to delete it.  Very similar to auto_ptr, with destructive
	transfer of ownership, (as if we derived from it privately).
	In addition, the contained pointer has const (read-only) semantics.  
	The only methods of releasing memory are through
	destructive transfer, or end of scope.  
	We want excl_const_ptr to be usable with standard containers.  
	A const excl_const_ptr prevents transfer of ownership.  
 */
template <class T>
class excl_const_ptr : public virtual base_const_ptr<T> {
template <class> friend class excl_const_ptr;
template <class> friend class never_const_ptr;
template <class> friend class some_const_ptr;

protected:
const T*	release(void) throw() {
		const T* ret = this->cptr;
		this->cptr = NULL;
		return ret;
	}

void	reset(const T* p = NULL) throw() {
		if (this->cptr) {
			assert (this->cptr != p);
			// violation of exclusion!
			delete this->cptr;
		}
		this->cptr = p;
	}

public:
/**
	Ownership query.
 */
	bool owned(void) const { return this->cptr != NULL; }

/**
	Destructive copy constructor, non-const source.  
	Transfers ownership.  
	Allowed to be implicit.  
	Can gcc-2.95.3 find this?
 */
	excl_const_ptr(excl_const_ptr<T>& e) throw() :
		base_const_ptr<T>(e.release()) { }

/**
	\param p should be a pointer, where another object or scope
		has assumed the responsibility for de-allocating it.  
		Defaults to NULL.  
 */
explicit excl_const_ptr(const T* p) throw() : base_const_ptr<T>(p) { }
	excl_const_ptr(void) throw() : base_const_ptr<T>(NULL) { }

// do we ever want to transfer ownership from non-const to const?
//	probably not, because will never be able to extract as non-const!
//	one-way trap!
//	there are mechanisms for it to be done explicitly...

/**
	Bogus copy constructor, const source.  
	Need this to make some standard containers happy.  
	Preserves exclusion, by assigning NULL.  
 */
explicit excl_const_ptr(const excl_const_ptr<T>& e) throw() :
			base_const_ptr<T>(e.cptr) { }
				// incorrect, violates exclusion!?
	// forbid copy constructing?
//			base_const_ptr<T>(NULL) { }

/**
	Converting from write-able to read-only pointer.  
	Permissible, but better damn well be explicit and intentional.  
 */
explicit excl_const_ptr(excl_ptr<T>& e) throw() :
			base_const_ptr<T>(e.release()) { }

/**
	For safe up-cast, pointer constructor.  
 */
	template <class S>
	excl_const_ptr(excl_const_ptr<S>& e) throw() :
		base_const_ptr<T>(e.release()) { }

/**
	De-allocates memory.  
	Should be safe to delete NULL?
 */
virtual	~excl_const_ptr() { release(); }


public:
// again, don't want to transfer ownership from non-const to const automatically
// excl_const_ptr<T>& operator = (excl_ptr<T>& e) throw();
excl_const_ptr<T>& operator = (excl_const_ptr<T>& e) throw() {
		reset(e.release());
		return *this;
	}

// copied from auto_ptr_ref
	excl_const_ptr(base_const_ptr_ref<T> r) throw() :
		base_const_ptr<T>(r.cptr) { }
	excl_const_ptr(base_ptr_ref<T> r) throw() :
		base_const_ptr<T>(r.ptr) { }

	template <class S>
	excl_const_ptr(base_const_ptr_ref<S> r) throw() :
		base_const_ptr<T>(r.cptr) { }
	template <class S>
	excl_const_ptr(base_ptr_ref<S> r) throw() :
		base_const_ptr<T>(r.ptr) { }

excl_const_ptr<T>& operator = (base_const_ptr_ref<T> r) throw() {
		reset(r.cptr);
		r.cptr = NULL;
		return *this;
	}
excl_const_ptr<T>& operator = (base_ptr_ref<T> r) throw() {
		reset(r.ptr);
		r.ptr = NULL;
		return *this;
	}

	// safe type-casting
	template <class S>
	operator base_const_ptr_ref<S>() throw() {
		return base_const_ptr_ref<S>(this->release());
	}

	/**
		Dynamic cast template wrapper.  
		Can't overload dynamic_cast, because it's a keyword.  
		Avoid returning naked pointers!
	 */
	template <class S>
	never_const_ptr<S>	is_a(void) const;

	/**
		Ownership transferring version. 
	 */
	template <class S>
	excl_const_ptr<S>	is_a_xfer(void);

	/**
		Dynamic cast assertion.  
		No return value.  
	 */
	template <class S>
	void	must_be_a(void) const {
		assert(dynamic_cast<const S*>(this->cptr));
	}

	/**
		Destructive transfer, safe up-cast.  
		constructor probably not available yet...
	 */
	template <class S>
	excl_const_ptr<S>	as_a(void) {
		return excl_const_ptr<S>(
			static_cast<const S*>(this->release()));
		// shouldn't be necessary to static_cast
		// but this make it clear what it's intended for
	}

// permissible assignments

// non-member functions
};	// end class excl_const_ptr

//=============================================================================
/**
	Pointer that is NEVER to be deleted, even if delete is called on it.  
	For safety (and inconvenience), forbid direct interaction with
	naked pointers.  
 */
template <class T>
class never_ptr : public virtual base_ptr<T> {
template <class> friend class excl_ptr;
template <class> friend class never_ptr;
template <class> friend class excl_const_ptr;
template <class> friend class never_const_ptr;

public:
// protected:
/**
	Constructors that use naked pointers *should* be restricted.  
	But situations arise where this constructor needs to be publicly
	available, such as when converting and casting between types.  
 */
explicit never_ptr(T* p) throw() : base_ptr<T>(p) { }

#if 0
public:		// public ok for dynamic cast?
template <class S>
explicit never_ptr(S* p) throw() : base_ptr<T>(dynamic_cast<T*>(p)) { }
#endif

public:
/**
	Ownership query, always returns false.
 */
	bool owned(void) const { return false; }

/**
	The only stipulation on p is that someone ELSE has the responsibility 
	for deleting the pointer.  
	Or should we forbid construction with naked pointers, 
		and limit arguments to possible owners?
 */
explicit never_ptr(void) throw() : base_ptr<T>(NULL) { }
// constructors with same element type

// this constructor covers the following two
/**
	It should be always safe to implicitly convert any base_ptr
	to a never_ptr.  
	It will never modify the argument.  
	This covers all pointer types.  
 */
	never_ptr(const excl_ptr<T>& p) throw() : base_ptr<T>(p.ptr) { }
	never_ptr(const some_ptr<T>& p) throw();
	never_ptr(const never_ptr<T>& p) throw() : base_ptr<T>(p.ptr) { }

/*** cross-template member not friendly accessible (p is protected)
template <class S>
explicit never_ptr(const excl_ptr<S>& p) throw() : 
		base_ptr<T>(dynamic_cast<T*>(p.ptr)) { }
***/

virtual	~never_ptr() { }

// also want cross-type versions?
never_ptr<T>& operator = (const base_ptr<T>& e) throw() {
		this->ptr = e.ptr;
		return *this;
	}

/** safe up-cast */
template <class S>
never_ptr<T>& operator = (const base_ptr<S>& e) throw() {
		this->ptr = e.ptr;	// static up-cast
		return *this;
	}

	never_ptr(base_ptr_ref<T> r) throw() : base_ptr<T>(r.ptr) { }

	never_ptr<T>& operator = (base_ptr_ref<T> r) throw() {
			this->ptr = r.ptr;
			return *this;
		}

// should some operators have const?

	// safe type-casting
	template <class S>
	operator base_ptr_ref<S>() throw() {
		return base_ptr_ref<S>(this->ptr);
	}

	// type casting
	template <class S>
	never_ptr<S> is_a(void) const;

	/**
		Dynamic cast assertion.  
		No return value.  
	 */
	template <class S>
	void	must_be_a(void) const {
		assert(dynamic_cast<S*>(this->ptr));
	}

	/**
		Safe up-cast.  
		Notice that this has constant semantics, unlike excl_ptr's.  
	 */
	template <class S>
	never_ptr<S>	as_a(void) const {
		return never_ptr<S>(static_cast<S*>(this->ptr));
			// shouldn't be necessary to static_cast
			// but this make it clear what it's intended for
	}

// non-member functions
};	// end class never_ptr


//=============================================================================
/**
	Pointer that is NEVER to be deleted, even if delete is called on it.  
	For safety (and inconvenience), forbid direct interaction with
	naked pointers.  
 */
template <class T>
class never_const_ptr : public virtual base_const_ptr<T> {
template <class> friend class excl_ptr;
template <class> friend class never_ptr;
template <class> friend class excl_const_ptr;
template <class> friend class never_const_ptr;

public:
/**
	Constructors that use naked pointers must be restricted.  
 */
explicit never_const_ptr(const T* p) throw() : base_const_ptr<T>(p) { }

public:		// public ok for dynamic cast?
/**
	Is this a good idea?  automatic dynamic-type cast in constructor?
 */
template <class S>
explicit never_const_ptr(const S* p) throw() :
		base_const_ptr<T>(dynamic_cast<const T*>(p)) { }

public:
/**
	Ownership query, always false.  
 */
	bool owned(void) const { return false; }

/**
	The only stipulation on p is that someone ELSE has the responsibility 
	for deleting the pointer.  
	Or should we forbid construction with naked pointers, 
		and limit arguments to possible owners?
 */
explicit never_const_ptr(void) throw() : base_const_ptr<T>(NULL) { }

// constructors with same element type
/**
	Doesn't have to be explicit because it should always be safe
	to copy a never-delete, read-only pointer.  
 */
	never_const_ptr(const never_ptr<T>& p) throw() :
		base_const_ptr<T>(p.ptr) { }
	never_const_ptr(const some_ptr<T>& p) throw();
	never_const_ptr(const excl_ptr<T>& p) throw() :
		base_const_ptr<T>(p.ptr) { }

	/** safe up-cast versions */
	template <class S>
	never_const_ptr(const never_ptr<S>& p) throw() :
		base_const_ptr<T>(p.ptr) { }
	template <class S>
	never_const_ptr(const some_ptr<S>& p) throw();
	template <class S>
	never_const_ptr(const excl_ptr<S>& p) throw() :
		base_const_ptr<T>(p.ptr) { }

/**
	Doesn't have to be explicit because it should always be safe
	to copy a never-delete, read-only pointer.  
	Pass by reference to keep base_const_ptr abstract.
 */
	never_const_ptr(const never_const_ptr<T>& p) throw() :
		base_const_ptr<T>(p.cptr) { }
	never_const_ptr(const some_const_ptr<T>& p) throw();
	never_const_ptr(const excl_const_ptr<T>& p) throw() :
		base_const_ptr<T>(p.cptr) { }

	/** safe up-cast versions */
	template <class S>
	never_const_ptr(const never_const_ptr<S>& p) throw() :
		base_const_ptr<T>(p.cptr) { }
	template <class S>
	never_const_ptr(const some_const_ptr<S>& p) throw();
	template <class S>
	never_const_ptr(const excl_const_ptr<S>& p) throw() :
		base_const_ptr<T>(p.cptr) { }

virtual	~never_const_ptr() { }

never_const_ptr<T>& operator = (const base_ptr<T>& e) throw() {
		this->cptr = e.ptr;
		return *this;
	}

never_const_ptr<T>& operator = (const base_const_ptr<T>& e) throw() {
		this->cptr = e.cptr;
		return *this;
	}

	never_const_ptr(const base_const_ptr_ref<T> r) throw() :
		base_const_ptr<T>(r.cptr) { }

never_const_ptr<T>& operator = (const base_const_ptr_ref<T> r) throw() {
		this->cptr = r.cptr;
		return *this;
	}

	// safe type-casting
	template <class S>
	operator base_const_ptr_ref<S>() const throw() {
		return base_const_ptr_ref<S>(this->cptr);
	}

	template <class S>
	operator never_const_ptr<S> () const throw() {
		return never_const_ptr<S>(this->cptr);
	}

	// dynamic type-casting
	template <class S>
	never_const_ptr<S> is_a(void) const;

	/**
		Dynamic cast assertion.  
		No return value.  
	 */
	template <class S>
	void	must_be_a(void) const {
		assert(dynamic_cast<const S*>(this->cptr));
	}

	/**
		Safe up-cast.  
		Notice that this has constant semantics, unlike excl_ptr's.  
	 */
	template <class S>
	never_const_ptr<S>	as_a(void) const {
		return never_const_ptr<S>(static_cast<const S*>(this->cptr));
			// shouldn't be necessary to static_cast
			// but this make it clear what it's intended for
	}

// non-member functions
};	// end class never_const_ptr

//=============================================================================
/**
	Pointer class with dynamic ownership.  
	Is sometimes an excl ptr, sometimes a never_ptr.  
	Simply uses an extra bool to keep track.  
	Consider trick, use LSB of address and alignment?
 */
template <class T>
class some_ptr : public virtual base_ptr<T> {
template <class> friend class excl_ptr;
template <class> friend class excl_const_ptr;
template <class> friend class never_ptr;
template <class> friend class never_const_ptr;

protected:
	bool	own;
protected:
	/**
		Returns pointer even when not owned.  
		Leaves pointer intact when relinquishing ownership.  
		\return pointer.  
	 */
	T* release(void) {
		this->own = false;
		return this->ptr;
	}

	/**
		Frees memory if valid pointer was owned and 
		resets with ownership of new pointer, if not NULL.  
		\param b new ownership status.  
		\param p new pointer, owned.  
	 */
	void reset(const bool b, T* p = NULL) {
		if (this->own && this->ptr)
			delete this->ptr;
		this->ptr = p;
		this->own = b && p != NULL;
	}

public:
	/**
		Ownership query.  
	 */
	bool owned(void) const { return own; }

	/**
		Default constructor, initializes to NULL;
	 */
	some_ptr(void) : base_ptr<T>(NULL), own(false) { }

	/**
		Never steals ownership, shares by default.  
		Ownership can only be transferred by explicit conversion
		to an excl_ptr.  
	 */
	some_ptr(const never_ptr<T>& p) : base_ptr<T>(p.ptr), own(false) { }
	some_ptr(const some_ptr<T>& p) : base_ptr<T>(p.ptr), own(false) { }
	// need some constructor where this owns the pointer...

	/**
		Take ownership from an excl_ptr, which nullifies it.  
		Of course, we only own it if it's not NULL.  
	 */
explicit some_ptr(excl_ptr<T>& p) : base_ptr<T>(p.release()),
		own(this->ptr != NULL) {
		assert(!p);		// just to make sure
	}

// explicit some_ptr(const excl_ptr<T>& p);

	/**
		Conditional destructor.  De-allocates only if it owns.  
	 */
	~some_ptr(void) { if (this->ptr && this->own) delete this->ptr; }

// cannot accept naked pointer, must wrap with excl or never pointer
// explicit some_ptr(T* p) : base_ptr<T>(ptr), own(true) { }

// conversion operators:
// copied from auto_ptr_ref
	some_ptr(base_some_ptr_ref<T> r) throw() :
		base_ptr<T>(r.ptr), own(r.own) { }

some_ptr<T>& operator = (const never_ptr<T>& p) throw() {
		this->reset(false, p.ptr);
		return *this;
	}

some_ptr<T>& operator = (const some_ptr<T>& p) throw() {
		this->reset(false, p.ptr);
		return *this;
	}

/**
	Transfers ownership.  
 */
some_ptr<T>& operator = (some_ptr<T>& p) throw() {
		this->reset(p.own, p.release());
		return *this;
	}

/**
	Doesn't transfer ownership.
 */
some_ptr<T>& operator = (const excl_ptr<T>& p) throw() {
		this->reset(false, p.ptr);
		return *this;
	}

/**
	Transfers ownership.
 */
some_ptr<T>& operator = (excl_ptr<T>& p) throw() {
		this->reset(p.ptr != NULL, p.release());
		return *this;
	}

	/**
		With transfer of ownership.
	 */
some_ptr<T>& operator = (base_some_ptr_ref<T> r) throw() {
		this->reset(r.own, r.ptr);
		r.own = false;
		return *this;
	}

	operator never_ptr<T> () const throw() {
		return never_ptr<T>(this->ptr);
	}

	// safe type-casting
	template <class S>
	operator base_some_ptr_ref<S>() throw() {
		bool o = own;
		own = false;
		return base_some_ptr_ref<S>(this->ptr, o);
	}

	/**
		Dynamic type-cast.  
	 */
	template <class S>
	never_ptr<S>	is_a(void) const;

	/**
		Dynamic cast assertion.  
		No return value.  
	 */
	template <class S>
	void	must_be_a(void) const {
		assert(dynamic_cast<S*>(this->ptr));
	}

};	// end class some_ptr

//=============================================================================
/**
	Pointer that is sometimes owned, but always read-only.  
 */
template <class T>
class some_const_ptr : public virtual base_const_ptr<T> {
template <class> friend class excl_ptr;
template <class> friend class excl_const_ptr;
template <class> friend class never_ptr;
template <class> friend class never_const_ptr;

protected:
	bool	own;
public:
	/**
		Ownership query.  
	 */
	bool owned(void) const { return own; }

	/**
		Default constructor, initializes to NULL.  
	 */
	some_const_ptr(void) : base_const_ptr<T>(NULL), own(false) { }

	/**
		Never steals ownership, shares by default.  
		Ownership can only be transferred by explicit conversion
		to an excl_ptr.  
	 */
	some_const_ptr(const never_ptr<T>& p) :
		base_const_ptr<T>(p.ptr), own(false) { }
	some_const_ptr(const never_const_ptr<T>& p) :
		base_const_ptr<T>(p.cptr), own(false) { }

	some_const_ptr(const some_ptr<T>& p) :
		base_const_ptr<T>(p.ptr), own(false) { }
	some_const_ptr(const some_const_ptr<T>& p) :
		base_const_ptr<T>(p.cptr), own(false) { }

	/**
		Can this be implicit?
	 */
explicit some_const_ptr(excl_const_ptr<T>& p) :
		base_const_ptr<T>(p.release()), own(this->cptr != NULL) { }

	/**
		Conditional destructor.  De-allocates only if it owns.  
	 */
	~some_const_ptr(void) { if (this->cptr && this->own) delete this->cptr; }

// copied from auto_ptr_ref
	some_const_ptr(base_some_const_ptr_ref<T> r) throw() :
		base_const_ptr<T>(r.cptr), own(r.own) { }

some_const_ptr<T>& operator = (const never_const_ptr<T>& p) throw() {
		this->reset(false, p.ptr);
		return *this;
	}

some_const_ptr<T>& operator = (const some_const_ptr<T>& p) throw() {
		this->reset(false, p.ptr);
		return *this;
	}

/**
	Transfers ownership.  
 */
some_const_ptr<T>& operator = (some_const_ptr<T>& p) throw() {
		this->reset(p.own, p.release());
		return *this;
	}

some_const_ptr<T>& operator = (const excl_const_ptr<T>& p) throw() {
		this->reset(false, p.ptr);
		return *this;
	}

some_const_ptr<T>& operator = (excl_const_ptr<T>& p) throw() {
		this->reset(p.ptr != NULL, p.release());
		return *this;
	}

some_const_ptr<T>& operator = (base_const_ptr_ref<T> r) throw() {
		this->own = r.own;
		r.own = false;
		this->cptr = r.cptr;
		return *this;
	}

	operator never_const_ptr<T> () const throw() {
		return never_const_ptr<T>(this->cptr);
	}

	// safe type-casting
	template <class S>
	operator base_some_const_ptr_ref<S>() throw() {
		return base_some_const_ptr_ref<S>(this->cptr, this->own);
	}

	/**
		Dynamic type-cast.  
	 */
	template <class S>
	never_const_ptr<S>	is_a(void) const;

	/**
		Dynamic cast assertion.  
		No return value.  
	 */
	template <class S>
	void	must_be_a(void) const {
		assert(dynamic_cast<const S*>(this->cptr));
	}


};	// end class some_const_ptr

//=============================================================================
template <class T>
excl_ptr<T>::excl_ptr(some_ptr<T>& s) throw() : base_ptr<T>(s.ptr) {
	assert(s.own);		// else it didn't own it before!
	s.own = false;
}

template <class T>
never_ptr<T>::never_ptr(const some_ptr<T>& p) throw() : base_ptr<T>(p.ptr) { }

template <class T>
never_const_ptr<T>::never_const_ptr(const some_ptr<T>& p) throw() :
	base_const_ptr<T>(p.ptr) { }

template <class T>
never_const_ptr<T>::never_const_ptr(const some_const_ptr<T>& p) throw() :
	base_const_ptr<T>(p.cptr) { }

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
never_const_ptr<S>
excl_const_ptr<T>::is_a(void) const {
	return never_const_ptr<S>(dynamic_cast<const S*>(this->cptr));
}

template <class T>
template <class S>
excl_const_ptr<S>
excl_const_ptr<T>::is_a_xfer(void) {
	const S* s = dynamic_cast<const S*>(this->cptr);
	if (s) release();
	return excl_const_ptr<S>(s);
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
never_const_ptr<S>
never_const_ptr<T>::is_a(void) const {
	return never_const_ptr<S>(dynamic_cast<const S*>(this->cptr));
}

template <class T>
template <class S>
never_ptr<S>
some_ptr<T>::is_a(void) const {
	return never_ptr<S>(dynamic_cast<S*>(this->ptr));
	// uses cross-type pointer constructor with dynamic cast
}

template <class T>
template <class S>
never_const_ptr<S>
some_const_ptr<T>::is_a(void) const {
	return never_const_ptr<S>(dynamic_cast<const S*>(this->cptr));
}

//=============================================================================
}	// end namespace PTRS_NAMESPACE

#endif	//	__PTRS_H__

