// "ptrs.h" -- need better name?
// library of pointers for explicit memory management
// all methods in this file are to be defined here, to be inlined

// to do: references! (as non-null pointers)

// major issue: pointer type conversion emulation with classes
// friends and templates:
//	pointer classes can't access each other private/protected constructors
//	because can't template friend classes, no generalization... :(

// documentation style is for doxygen

#ifndef	__PTRS_H__
#define	__PTRS_H__

#include <assert.h>

// overrideable namespace
#ifndef	PTRS_NAMESPACE
#define	PTRS_NAMESPACE			fang
#endif

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
template <class T>	class base_ptr;		// generic pointer
template <class T>	class base_const_ptr;	// generic pointer
template <class T>	class some_ptr;		// sometimes owner
template <class T>	class some_const_ptr;	// sometimes owner
template <class T>	class excl_ptr;		// exclusive owner and deleter
template <class T>	class excl_const_ptr;	// exclusive owner and deleter
template <class T>	class never_ptr;	// never owner
template <class T>	class never_const_ptr;	// never owner
template <class T>	class dynconst_ptr;	// dynamic const-ness

// never is a bad name...
// never_const is always const!

//=============================================================================
/**
	Grand ancestral abstract pointer.
	Trying to get friend class to work across template instances.  
	Template members.  
 */
class abstract_ptr {
protected:
virtual	~abstract_ptr() { }

// but a member template cannot be virtual... *sigh*
// template <class T>
// virtual	T* get_temp(void) const = 0;
// must settle with the following:
virtual	void* void_ptr(void) const { return NULL; }
	// because const_ptr cannot return non-const
virtual	const void* const_void_ptr(void) const = 0;

public:
	operator bool() const { return const_void_ptr() != NULL; }
	bool operator == (const abstract_ptr& p) const
		{ return const_void_ptr() == p.const_void_ptr(); }
	bool operator != (const abstract_ptr& p) const
		{ return const_void_ptr() != p.const_void_ptr(); }
};	// end class abstract_ptr

//=============================================================================
/**
	Mother pointer.  
	No public constructors, only public methods are inherited.  
 */
template <class T>
class base_ptr : public abstract_ptr {
friend class excl_ptr<T>;
friend class never_ptr<T>;
friend class excl_const_ptr<T>;
friend class never_const_ptr<T>;

protected:
	T*	ptr;

protected:
	void* void_ptr(void) const { return ptr; }
	const void* const_void_ptr(void) const { return ptr; }
		// consume with a reinterpret_cast?

explicit base_ptr(T* p) throw() : ptr(p) { }

/**
	This destructor does nothing, it is up to derived classes to act.  
 */
virtual	~base_ptr() { }

public:
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

#if 0
template <class S>
friend
base_ptr<S> is_a(const base_ptr<T>& p);
#endif
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
friend class never_ptr<T>;
friend class never_const_ptr<T>;

protected:
	const T*	cptr;

protected:
explicit base_const_ptr(const T* p) throw() : cptr(p) { }
/**
	This destructor does nothing, it is up to derived classes to act.  
 */
virtual	~base_const_ptr() { }

protected:
// want to avoid leaking out naked pointers...
//	const T* get(void) const { return cptr; }

	// no problems returning as non-const?
	const void* const_void_ptr(void) const { return cptr; }
		// consume with a reinterpret_cast!!!
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
friend class excl_const_ptr<T>;
friend class never_ptr<T>;
friend class never_const_ptr<T>;

protected:
/**
	Relinquishes resposibility for deleting pointer, by returning a
	copy of it, and nullifying its own pointer.  
	Must remain protected, hence not publicly accessible.  
	\return a pointer that must be deleted by it consumer if not NULL.  
 */
T*	release(void) throw() {
		T* ret = ptr;
		ptr = NULL;
		return ret;
	}

/**
	Sets the member pointer, deleting the previous value
	if appropriate.  
	Must be protected, not publicly available.  
	\param p non-const pointer that is exclusively owned.  
 */
void	reset(T* p = NULL) throw() {
		if (ptr) {
			assert (ptr != p);	// violation of exclusion!
			delete ptr;
		}
		ptr = p;
	}

public:
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
 */
	excl_ptr(excl_ptr<T>& e) throw() : base_ptr<T>(e.release()) { }

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

excl_ptr<T>& operator = (base_ptr_ref<T> r) throw() {
		reset(r.ptr);
		r.ptr = NULL;
		return *this;
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

	template <class S>
	never_ptr<S>	as_a(void) const {
		return static_cast<S*>(ptr);
	}

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
friend class excl_ptr<T>;
friend class never_ptr<T>;
friend class never_const_ptr<T>;

protected:
const T*	release(void) throw() {
		const T* ret = cptr;
		cptr = NULL;
		return ret;
	}

void	reset(const T* p = NULL) throw() {
		if (cptr) {
			assert (cptr != p);	// violation of exclusion!
			delete cptr;
		}
		cptr = p;
	}

public:
/**
	\param p should be a newly allocated pointer, which has not
		been tampered with or leaked out.  Vulnerability
		here is that we don't know what else has been done
		with p.  
		Can we make new T return an excl_ptr?  No.
 */
explicit excl_const_ptr(const T* p) throw() : base_const_ptr<T>(p) { }
	excl_const_ptr(void) throw() : base_const_ptr<T>(NULL) { }
/**
	Destructive copy constructor, non-const source.  
	Transfers ownership.  
 */
	excl_const_ptr(excl_const_ptr<T>& e) throw() :
		base_const_ptr<T>(e.release()) { }

// do we ever want to transfer ownership from non-const to const?
//	probably not, because will never be able to extract as non-const!
//	one-way trap!
//	there are mechanisms for it to be done explicitly...

/**
	Bogus copy constructor, const source.  
	Need this to make some standard containers happy.  
	Preserves exclusion.  
 */
#if 1
explicit excl_const_ptr(const excl_const_ptr<T>& e) throw() :
			base_const_ptr<T>(e.cptr) {
	}
explicit excl_const_ptr(const excl_ptr<T>& e) throw() :
			base_const_ptr<T>(e.cptr) {
	}
#endif

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
friend class excl_ptr<T>;
friend class excl_const_ptr<T>;
friend class never_const_ptr<T>;

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
	The only stipulation on p is that someone ELSE has the responsibility 
	for deleting the pointer.  
	Or should we forbid construction with naked pointers, 
		and limit arguments to possible owners?
 */
explicit never_ptr(void) throw() : base_ptr<T>(NULL) { }
// constructors with same element type

// this constructor covers the following two
explicit never_ptr(const base_ptr<T>& p) throw() : base_ptr<T>(p.ptr) { }

/*** cross-template member not friendly accessible (p is protected)
template <class S>
explicit never_ptr(const excl_ptr<S>& p) throw() : 
		base_ptr<T>(dynamic_cast<T*>(p.unprotected_ptr())) { }
***/

virtual	~never_ptr() { }

// also want cross-type versions?
never_ptr<T>& operator = (const base_ptr<T>& e) throw() {
		ptr = e.ptr;
		return *this;
	}

	never_ptr(base_ptr_ref<T> r) throw() : base_ptr<T>(r.ptr) { }

	never_ptr<T>& operator = (base_ptr_ref<T> r) throw() {
			ptr = r.ptr;
			return *this;
		}

	// safe type-casting
	template <class S>
	operator base_ptr_ref<S>() throw() {
		return base_ptr_ref<S>(ptr);
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
friend class excl_ptr<T>;
friend class never_ptr<T>;
friend class excl_const_ptr<T>;

public:
/**
	Constructors that use naked pointers must be restricted.  
 */
explicit never_const_ptr(const T* p) throw() : base_const_ptr<T>(p) { }

public:		// public ok for dynamic cast?
template <class S>
explicit never_const_ptr(const S* p) throw() :
		base_const_ptr<T>(dynamic_cast<const T*>(p)) { }

public:
/**
	The only stipulation on p is that someone ELSE has the responsibility 
	for deleting the pointer.  
	Or should we forbid construction with naked pointers, 
		and limit arguments to possible owners?
 */
explicit never_const_ptr(void) throw() : base_const_ptr<T>(NULL) { }
// constructors with same element type
explicit never_const_ptr(const base_ptr<T>& p) throw() :
		base_const_ptr<T>(p.ptr) { }
explicit never_const_ptr(const base_const_ptr<T>& p) throw() :
		base_const_ptr<T>(p.cptr) { }

virtual	~never_const_ptr() { }

never_const_ptr<T>& operator = (const base_ptr<T>& e) throw() {
		cptr = e.ptr;
		return *this;
	}

never_const_ptr<T>& operator = (const base_const_ptr<T>& e) throw() {
		cptr = e.cptr;
		return *this;
	}

	never_const_ptr(base_const_ptr_ref<T> r) throw() :
		base_const_ptr<T>(r.cptr) { }

never_const_ptr<T>& operator = (base_const_ptr_ref<T> r) throw() {
		cptr = r.cptr;
		return *this;
	}

	// safe type-casting
	template <class S>
	operator base_const_ptr_ref<S>() throw() {
		return base_const_ptr_ref<S>(cptr);
	}

	// dynamic type-casting
	template <class S>
	operator never_const_ptr<S> () throw() {
		return never_const_ptr<S>(dynamic_cast<const S*>(cptr));
	}

// non-member functions
};	// end class never_const_ptr

//=============================================================================

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
	return never_ptr<S>(dynamic_cast<S*>(ptr));
	// uses cross-type pointer constructor with dynamic cast
}


template <class T>
template <class S>
never_const_ptr<S>
excl_const_ptr<T>::is_a(void) const {
	return never_const_ptr<S>(cptr);
}

//=============================================================================
}	// end namespace PTRS_NAMESPACE

#endif	//	__PTRS_H__

