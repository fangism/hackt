/**
	\file "util/union_find.h"
	Classic union-find data structure.
	$Id: union_find.h,v 1.3 2006/02/26 02:28:03 fang Exp $
 */

#ifndef	__UTIL_UNION_FIND_H__
#define	__UTIL_UNION_FIND_H__

#include "util/macros.h"

#define	WANT_UNION_FIND_DEREFERENCE_OPERATORS		1

namespace util {
//=============================================================================
/**
	Safely down-castable base class for template union_find.  
	Union-finds do not provide in interator interface.  
	Union-find nodes are also non-copyable.  
 */
class union_find_base {
private:
	typedef	union_find_base			this_type;
public:
	typedef	this_type*			find_type;
	typedef	const this_type*		const_find_type;
protected:
	/**
		Continuation pointer.  
		Chase the pointer until it finds itself.  
	 */
	find_type				next;

	union_find_base() : next(this) { }

protected:

	// trivial destructor

	/**
		Performs path-compression along the way.
	 */
	find_type
	__find(void) {
		NEVER_NULL(this->next);
		if (this->next != this->next->next) {
			this->next = this->next->__find();
		}
		return this->next;
	}

	/**
		Non-modifying find(), suppresses path compression.  
	 */
	const_find_type
	__find(void) const {
		const_find_type tmp = this;
		while (tmp != tmp->next) {
			tmp = tmp->next;
		}
		return tmp;
	}

	void
	__union(this_type& r) {
		__find()->next = r.__find();
	}

	/**
		NOTE: assignment overloaded operator takes non-const argument
	 */
	this_type&
	operator = (this_type& r) {
		__union(r);
		return *this;
	}

	/**
		Coercively sets the next pointer.
		Use only if you know what you're doing!
		Useful for object deserialization and graph reconstruction.  
		Could include a paranoid check for cycle detection...
	 */
	void
	__set(const find_type n) {
		this->next = n;
	}

	const_find_type
	__peek(void) const { return next; }

private:
	// private, undefined copy-constructor
	explicit
	union_find_base(const this_type&);

	// private, undefined assignment
	this_type&
	operator = (const this_type&);

};	// end class union_find_base

//=============================================================================
/**
	NOTE: this does NOT do any memory management.
	Do we want pointer/dereference semantics?
 */
template <class T>
class union_find : protected union_find_base {
	typedef	union_find<T>		this_type;
protected:
	typedef	union_find_base		parent_type;
public:
	typedef	T			value_type;
	typedef	T			element_type;
	typedef	T&			reference;
	typedef	const T&		const_reference;
	typedef	T*			pointer;
	typedef	const T*		const_pointer;
protected:
	value_type			_value;
public:
	union_find() : parent_type(), _value() { }

	union_find(const value_type& v) : parent_type(), _value(v) { }

	~union_find() { }

#if WANT_UNION_FIND_DEREFERENCE_OPERATORS
	reference
	operator * () { return this->_value; }

	const_reference
	operator * () const { return this->value; }

	pointer
	operator -> () { return &this->_value; }

	const_pointer
	operator -> () const { return &this->_value; }
#endif

	reference
	value(void) { return this->_value; }

	const_reference
	value(void) const { return this->_value; }

	void
	unite(this_type& t) { parent_type::__union(t); }

	/**
		NOTE: Unusual overload of assignment: non-const argument.
		This does NOT assign values, only connects pointers.
	 */
	this_type&
	operator = (this_type& t) {
		return static_cast<this_type&>(parent_type::operator=(t));
	}

	this_type*
	find(void) { return static_cast<this_type*>(parent_type::__find()); }

	const this_type*
	find(void) const {
		return static_cast<const this_type*>(parent_type::__find());
	}

	const this_type*
	peek(void) const {
		return static_cast<const this_type*>(this->__peek());
	}

protected:
	void
	set(const this_type* n) { this->__set(n); }

private:
	/// non-copy-able
	explicit
	union_find(const this_type&);

};	// end class union_find

//=============================================================================
/**
	Variation where the struct is derived from the element type.  
	Useful when base value type is polymorphic.  
 */
template <class T>
class union_find_derived : protected union_find_base, public T {
	typedef	union_find_derived<T>	this_type;
protected:
	typedef	union_find_base		parent_type;
	typedef	T			value_type;
	typedef	T			element_type;
	typedef	T&			reference;
	typedef	const T&		const_reference;
	typedef	T*			pointer;
	typedef	const T*		const_pointer;
public:
	union_find_derived() : parent_type(), value_type() { }

	union_find_derived(const value_type& v) :
		parent_type(), value_type(v) { }

	~union_find_derived() { }

#if WANT_UNION_FIND_DEREFERENCE_OPERATORS
	reference
	operator * () { return *this; }

	const_reference
	operator * () const { return *this; }

	pointer
	operator -> () { return this; }

	const_pointer
	operator -> () const { return this; }
#endif

	reference
	value(void) { return *this; }

	const_reference
	value(void) const { return *this; }

	void
	unite(this_type& t) { parent_type::__union(t); }

	/**
		NOTE: Unusual overload of assignment: non-const argument.
		This does NOT assign values, only connects pointers.
	 */
	this_type&
	operator = (this_type& t) {
		return static_cast<this_type&>(parent_type::operator=(t));
	}

	this_type*
	find(void) { return static_cast<this_type*>(parent_type::__find()); }

	const this_type*
	find(void) const {
		return static_cast<const this_type*>(parent_type::__find());
	}

	const this_type*
	peek(void) const {
		return static_cast<const this_type*>(this->__peek());
	}

protected:
	void
	set(const this_type* n) { this->__set(n); }

};	// end class union_find_derived

//=============================================================================
}	// end namespace util

#endif	// __UTIL_UNION_FIND_H__

