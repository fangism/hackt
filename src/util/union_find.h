/**
	\file "util/union_find.h"
	Classic union-find data structure.
	$Id: union_find.h,v 1.3.2.3 2006/03/14 06:31:16 fang Exp $
 */

#ifndef	__UTIL_UNION_FIND_H__
#define	__UTIL_UNION_FIND_H__

#include "util/macros.h"
#include "util/NULL.h"
#include "util/size_t.h"
#include <iterator>

#define	WANT_UNION_FIND_DEREFERENCE_OPERATORS		1

namespace util {
//=============================================================================
class union_find_base;

template <class>
class union_find;

template <class>
class union_find_pseudo_iterator_base;

template <class, class, class>
class union_find_pseudo_iterator;

template <class>
struct union_find_pseudo_iterator_default;

template <class>
class union_find_derived;

template <class, class, class>
class union_find_derived_pseudo_iterator;

template <class>
struct union_find_derived_pseudo_iterator_default;

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
public:
	bool
	is_canonical(void) const { return this == next; }

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
	Deliberately not implementing std::iterator interface to
	avoid potential confusion and misuse.  
 */
template <class N>
class union_find_pseudo_iterator_base {
template <class, class, class> friend class union_find_pseudo_iterator;
private:
	typedef	union_find_pseudo_iterator_base<N>	this_type;
public:
	typedef	N			node_type;
	typedef	N*			node_pointer;
	typedef	const N*		const_node_pointer;
#if 0
	typedef	size_t			size_type;
	typedef	ptrdiff_t		difference_type;
	typedef	std::forward_iterator_tag	iterator_category;
#endif
protected:
	node_pointer			current;
protected:
	union_find_pseudo_iterator_base() : current(NULL) { }

	explicit
	union_find_pseudo_iterator_base(const node_pointer p) : current(p) { }

#if 1
	// kludge:
	explicit
	union_find_pseudo_iterator_base(const const_node_pointer p) :
		current(const_cast<node_pointer>(p)) { }
#endif

public:
	void
	advance(void) { this->current = this->current->next; }

	bool
	operator == (const this_type& i) const {
		return this->current == i.current;
	}

	bool
	operator != (const this_type& i) const {
		return this->current != i.current;
	}

	bool
	operator == (const node_pointer i) const {
		return this->current == i;
	}

	bool
	operator != (const node_pointer i) const {
		return this->current != i;
	}
};	// end class union_find_pseudo_iterator_base

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Handy way of declaring standard iterator types for union-finds.  
 */
template <class T>
struct union_find_pseudo_iterator_default {
	typedef	union_find_pseudo_iterator<T, T&, T*>		type;
	typedef	union_find_pseudo_iterator<T, const T&, const T*>
								const_type;
};	// end struct union_find_pseudo_iterator_default

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	A union_find::iterator.
 */
template <class T, class R, class P>
class union_find_pseudo_iterator :
		protected union_find_pseudo_iterator_base<union_find_base> {
protected:
	typedef	union_find_pseudo_iterator_base<union_find_base>
						parent_type;
	typedef	union_find_pseudo_iterator<T, R, P>	this_type;
public:
	typedef	T				value_type;
	typedef	R				reference;
	typedef	P				pointer;
	typedef	typename union_find_pseudo_iterator_default<T>::type
						iterator;
	typedef	typename union_find_pseudo_iterator_default<T>::const_type
						const_iterator;
	typedef	union_find<T>			node_type;
	typedef	union_find_base			node_base_type;
public:
	explicit
	union_find_pseudo_iterator(const pointer n) : parent_type(n) { }

#if 1
	// arg... quick workaround to my own stupidity.
	explicit
	union_find_pseudo_iterator(const node_base_type* n) :
		parent_type(const_cast<node_base_type*>(n)) { }
#endif

	// using default copy constructor
	// iterators are allowed to point to the same nodes.  

	/**
		Allows both iterator and const_iterator to be 
		copy-constructed from iterator.  
	 */
	union_find_pseudo_iterator(const iterator& i) :
		parent_type(i.current) { }

	reference
	operator * () const {
		// return static_cast<reference>(*this->current);
		return *static_cast<node_type&>(*current);
	}

	pointer
	operator -> () const { return &(operator * ()); }

	/**
		Prefix increment / advance.
	 */
	this_type&
	operator ++ () {
		this->advance();
		return *this;
	}

	/**
		Postfix increment / advance.
	 */
	this_type
	operator ++ (int) {
		const this_type temp(*this);
		this->advance();
		return temp;
	}

};	// end class union_find_pseudo_iterator

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
	typedef	typename union_find_pseudo_iterator_default<T>::type
					pseudo_iterator;
	typedef	typename union_find_pseudo_iterator_default<T>::const_type
					pseudo_const_iterator;
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

	// this_type*
	pseudo_iterator
	find(void) {
		// return static_cast<this_type*>(parent_type::__find());
		return pseudo_iterator(
			static_cast<this_type*>(parent_type::__find()));
	}

	// const this_type*
	pseudo_const_iterator
	find(void) const {
		// return static_cast<const this_type*>(parent_type::__find());
		return pseudo_const_iterator(
			static_cast<const this_type*>(parent_type::__find()));
	}

	const this_type*
	peek(void) const {
		return static_cast<const this_type*>(this->__peek());
	}

protected:
	void
	set(this_type* n) { this->__set(n); }

private:
	/// non-copy-able
	explicit
	union_find(const this_type&);
};	// end class union_find

//=============================================================================
template <class T>
struct union_find_derived_pseudo_iterator_default {
	typedef	union_find_derived_pseudo_iterator<T, T&, T*>	type;
	typedef	union_find_derived_pseudo_iterator<T, const T&, const T*>
							const_type;
};	// end struct union_find_derived_pseudo_iterator_default

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T, class R, class P>
class union_find_derived_pseudo_iterator :
		protected union_find_pseudo_iterator_base<union_find_derived<T> > {
	typedef	union_find_derived_pseudo_iterator<T, R, P>	this_type;
	typedef	union_find_pseudo_iterator_base<union_find_derived<T> >
							parent_type;
public:
	typedef	T					value_type;
	typedef	R					reference;
	typedef	P					pointer;
	typedef	typename union_find_derived_pseudo_iterator_default<T>::type
							iterator;
	typedef	typename union_find_derived_pseudo_iterator_default<T>::const_type
							const_iterator;
	typedef	union_find_derived<T>			node_type;
	typedef	union_find_base				node_base_type;
public:
	union_find_derived_pseudo_iterator() : parent_type() { }

	explicit
	union_find_derived_pseudo_iterator(const pointer n) :
		parent_type(static_cast<
			typename parent_type::const_node_pointer>(n)) { }

#if 0
	// arg... quick workaround to my own stupidity.
	explicit
	union_find_derived_pseudo_iterator(const node_base_type* n) :
		parent_type(const_cast<node_base_type*>(n)) { }
#endif

	// using default copy constructor
	// iterators are allowed to point to the same nodes.  

	/**
		Allows both iterator and const_iterator to be 
		copy-constructed from iterator.  
	 */
	union_find_derived_pseudo_iterator(const iterator& i) :
		parent_type(i.current) { }

	reference
	operator * () const {
		return *static_cast<node_type&>(*this->current);
		// return static_cast<reference>(*this->current);
	}

	pointer
	operator -> () const { return &(operator * ()); }

	/**
		Prefix increment / advance.
	 */
	this_type&
	operator ++ () {
		this->advance();
		return *this;
	}

	/**
		Postfix increment / advance.
	 */
	this_type
	operator ++ (int) {
		const this_type temp(*this);
		this->advance();
		return temp;
	}
};	// end class union_find_derived_pseudo_iterator

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
	typedef	typename union_find_derived_pseudo_iterator_default<T>::type
					pseudo_iterator;
	typedef	typename union_find_derived_pseudo_iterator_default<T>::const_type
					pseudo_const_iterator;
public:
	union_find_derived() : parent_type(), value_type() { }

	union_find_derived(const value_type& v) :
		parent_type(), value_type(v) { }

protected:
	/**
		NOTE: this copy-constructor only preserves value, but
		not connectivity.  
	 */
	explicit
	union_find_derived(const this_type& r) :
		parent_type(), value_type(static_cast<const_reference>(r)) { }

public:
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

	// this_type*
	pseudo_iterator
	find(void) {
		// return static_cast<this_type*>(parent_type::__find());
		return pseudo_iterator(
			static_cast<this_type*>(parent_type::__find()));
	}

	// const this_type*
	pseudo_const_iterator
	find(void) const {
		// return static_cast<const this_type*>(parent_type::__find());
		return pseudo_const_iterator(
			static_cast<const this_type*>(parent_type::__find()));
	}

	const this_type*
	peek(void) const {
		return static_cast<const this_type*>(this->__peek());
	}

protected:
	void
	set(this_type* n) { this->__set(n); }

};	// end class union_find_derived

//=============================================================================
}	// end namespace util

#endif	// __UTIL_UNION_FIND_H__

