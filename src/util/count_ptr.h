/**
	\file "count_ptr.h"
	Reference-counting pointer class templates.

	Generally not intended for mixed use with pointers from
	the non-counted pointer classes in "ptrs.h":
		excl_ptr, some_ptr, never_ptr.  
	$Id: count_ptr.h,v 1.4 2004/11/02 07:52:13 fang Exp $
 */

#ifndef	__COUNT_PTR_H__
#define	__COUNT_PTR_H__

#include <assert.h>
#include "count_ptr_fwd.h"

#ifndef NULL
#define	NULL		0
#endif

/**
	Namespace for reference-count pointer-classes.  
 */
namespace COUNT_PTR_NAMESPACE {
//=============================================================================
/**
	Base class for all reference-count pointers.  
 */
class abstract_base_count_ptr {
	// parent could be abstract_ptr
template <class> friend class count_ptr;
template <class> friend class count_const_ptr;

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

//=============================================================================
/**
	Reference counted pointer, non-const flavor.  
	How to use...
 */
template <class T>
class count_ptr : public abstract_base_count_ptr {
template <class> friend class count_ptr;
template <class> friend class count_const_ptr;
protected:
	typedef	abstract_base_count_ptr		parent;
public:
	typedef T			value_type;
	typedef T&			reference;
	typedef T*			pointer;
	typedef const T&		const_reference;
	typedef const T*		const_pointer;

protected:
	/**
		The pointer to the referenced object.  
	 */
	T*	ptr;

	void* void_ptr(void) const { return ptr; }
	const void* const_void_ptr(void) const { return ptr; }

public:
	/**
		Constructor of reference-counter pointer.  
		Stupid things to avoid: 
		1) tampering with (leaking out) newly allocated
			pointer before passing to this constructor.  
		\param p should be a newly allocated pointer, ONLY.  
	 */
explicit count_ptr(T* p = NULL) : parent((p) ? new size_t(1) : NULL), ptr(p) { }

	/**
		Copy constructor.  
		No argument pointer should ever be NULL?
	 */
	count_ptr(const count_ptr<T>& c) : parent(c.ref_count), ptr(c.ptr) {
		if (this->ref_count)
			(*this->ref_count)++;
	}

	/**
		Safe, up-cast copy constructor.  
		Still maintains reference count.  
	 */
	template <class S>
	count_ptr(const count_ptr<S>& c) : parent(c.ref_count), ptr(c.ptr) {
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
	count_ptr(T* p, size_t* c) : parent(c), ptr(p) {
		if (p) {
			assert(c);		// counter must be valid
			(*c)++;			// increment here
		} else
			assert(!c);		// counter must also be NULL
	}

protected:
	/**
		Overkill safety checks.  
	 */
	T* release(void) {
		T* ret = ptr;
		if(this->ref_count) {
			(*this->ref_count)--;
			if (!*this->ref_count) {
				assert(ptr);
				delete ptr;
				delete this->ref_count;
			}
			ptr = NULL;
			this->ref_count = NULL;
		} else {
			assert(!ptr);
		}
		return ret;
	}

	/**
		\param p the new pointer, from some other count_ptr.
		\param c the corresponding reference count if p is valid, 
			else is ignored.  
	 */
	void reset(T* p, size_t* c) {
		if(this->ref_count) {
			(*this->ref_count)--;
			if (!*this->ref_count) {
				assert(ptr);
				delete ptr;
				delete this->ref_count;
			}
		} else {
			assert(!ptr);
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

	void abandon(void) { release(); }

	/**
		Dereference.  
	 */
	T& operator * (void) const throw() { assert(ptr); return *ptr; }

	/**
		Dereference to member or method.  
	 */
	T* operator -> (void) const throw() { assert(ptr); return ptr; }

	/**
		Assignment operator.  
	 */
	count_ptr<T>& operator = (const count_ptr<T>& c) {
		reset(c.ptr, c.ref_count);
		return *this;
	}

	/**
		Returns a naked pointer only if this is the final reference
		to the object, otherwise returns NULL.  
	 */
	T* exclusive_release(void) {
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
	count_ptr<S> as_a(void) const {
		count_ptr<S> ret;
		ret.reset(static_cast<S*>(this->ptr), this->ref_count);
		return ret;
	}

	/**
		Dynamic cast.  
	 */
	template <class S>
	count_ptr<S> is_a(void) const {
		count_ptr<S> ret;
		ret.reset(dynamic_cast<S*>(this->ptr), this->ref_count);
		return ret;
	}

	/**
		Dynamic cast assertion.  
	 */
	template <class S>
	void must_be_a(void) const {
		assert(dynamic_cast<S*>(this->ptr));
	}

};	// end class count_ptr

//=============================================================================
/**
	Read-only version of reference-counted pointer.  
 */
template <class T>
class count_const_ptr : public abstract_base_count_ptr {
template <class> friend class count_const_ptr;

public:
	typedef T			value_type;
//	typedef T&			reference;
//	typedef T*			pointer;
	typedef const T&		const_reference;
	typedef const T*		const_pointer;

protected:
	typedef	abstract_base_count_ptr		parent;
protected:
	const T*	cptr;

	void* void_ptr(void) const { return NULL; }
	const void* const_void_ptr(void) const { return cptr; }

public:

	/**
		Constructor of reference-counter pointer.  
		Stupid things to avoid: 
		1) tampering with (leaking out) newly allocated
			pointer before passing to this constructor.  
		\param p should be a newly allocated pointer, ONLY.  
	 */
explicit count_const_ptr(const T* p = NULL) :
		parent((p) ? new size_t(1) : NULL), cptr(p) { }

	/**
		Copy constructor.  
		No argument pointer should ever be NULL?
	 */
	count_const_ptr(const count_ptr<T>& c) :
		parent(c.ref_count), cptr(c.ptr) {
		if (this->ref_count)
			(*this->ref_count)++;
	}
	count_const_ptr(const count_const_ptr<T>& c) :
		parent(c.ref_count), cptr(c.cptr) {
		if (this->ref_count)
			(*this->ref_count)++;
	}

	/**
		Safe, up-cast copy constructor.  
		Still maintains reference count.  
	 */
	template <class S>
	count_const_ptr(const count_ptr<S>& c) :
		parent(c.ref_count), cptr(c.ptr) {
		if (this->ref_count)
			(*this->ref_count)++;
	}
	template <class S>
	count_const_ptr(const count_const_ptr<S>& c) :
		parent(c.ref_count), cptr(c.cptr) {
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
	count_const_ptr(const T* p, size_t* c) : parent(c), cptr(p) {
		if (p) {
			assert(c);		// counter must be valid
			(*c)++;			// increment here
		} else
			assert(!c);		// counter must also be NULL
	}

protected:
	/**
		Overkill safety checks.  
	 */
	const T* release(void) {
		const T* ret = cptr;
		if(this->ref_count) {
			(*this->ref_count)--;
			if (!*this->ref_count) {
				assert(cptr);
				delete cptr;
				delete this->ref_count;
			}
			cptr = NULL;
			this->ref_count = NULL;
		} else {
			assert(!cptr);
		}
		return ret;
	}

	/**
		\param p the new pointer, from some other count_const_ptr.
		\param c the corresponding reference count if p is valid, 
			else is ignored.  
	 */
	void reset(const T* p, size_t* c) {
		if(this->ref_count) {
			(*this->ref_count)--;
			if (!*this->ref_count) {
				assert(cptr);
				delete cptr;
				delete this->ref_count;
			}
		} else {
			assert(!cptr);
		}
		cptr = p;
		this->ref_count = (cptr) ? c : NULL;
		if (cptr && this->ref_count)
			(*this->ref_count)++;
	}

public:
	/**
		Destructor, which conditionally deletes pointer if is
		last reference.  
	 */
	~count_const_ptr() { release(); }

	void abandon(void) { release(); }

	/**
		Dereference.  
	 */
	const T& operator * (void) const throw() { assert(cptr); return *cptr; }

	/**
		Dereference to member or method.  
	 */
	const T* operator -> (void) const throw() { assert(cptr); return cptr; }

	/**
		Assignment operator.  
	 */
	count_const_ptr<T>& operator = (const count_ptr<T>& c) {
		reset(c.ptr, c.ref_count);
		return *this;
	}
	count_const_ptr<T>& operator = (const count_const_ptr<T>& c) {
		reset(c.cptr, c.ref_count);
		return *this;
	}

	/**
		Returns a naked pointer only if this is the final reference
		to the object, otherwise returns NULL.  
	 */
	const T* exclusive_release(void) {
		if (this->owned()) {
			const T* ret = cptr;
			cptr = NULL;
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
	count_const_ptr<S> as_a(void) const {
		count_const_ptr<S> ret;
		ret.reset(static_cast<const S*>(this->cptr), this->ref_count);
		return ret;
	}

	/**
		Dynamic cast.  
	 */
	template <class S>
	count_const_ptr<S> is_a(void) const {
		count_const_ptr<S> ret;
		ret.reset(dynamic_cast<const S*>(this->cptr), this->ref_count);
		return ret;
	}

	/**
		Dynamic cast assertion.  
	 */
	template <class S>
	void must_be_a(void) const {
		assert(dynamic_cast<const S*>(this->cptr));
	}

};	// end class count_const_ptr

//=============================================================================
}	// end namespace COUNT_PTR_NAMESPACE

#endif	//	__COUNT_PTR_H__

