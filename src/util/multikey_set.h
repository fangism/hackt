/**
	\file "multikey_set.h"
	Multidimensional set class, using multikey_assoc as base interface. 
	$Id: multikey_set.h,v 1.1.2.1 2005/02/08 06:41:24 fang Exp $
 */

#ifndef	__UTIL_MULTIKEY_SET_H__
#define	__UTIL_MULTIKEY_SET_H__

#include <iosfwd>
#include "multikey_set_fwd.h"
#include "multikey_assoc.h"
#include "multikey.h"

namespace util {
using std::ostream;

//=============================================================================
/**
	Multidimensional set implemented as 1-level red-black tree
	by default.  Inherits facilities for multidimensional 
	slices from multikey_assoc base interface.  
	\param D the number of dimensions.
	\param T the key AND value type, which must be some derivative
		of multikey (or at least it's interface).
		The type of object must contain some multidimensional key, 
		which is defined with StrictWeakOrdering (less).  
	\param S the underlying set type, like std::set.
 */
MULTIKEY_SET_TEMPLATE_SIGNATURE
class multikey_set : public multikey_assoc<D, S<T> > {
private:
	typedef	multikey_set<D, T, S>			this_type;
protected:
	typedef	multikey_assoc<D, S<T> >		set_type;
	typedef	T					element_type;
public:
	typedef	typename set_type::key_type		key_type;
	typedef	typename set_type::mapped_type		mapped_type;
	typedef	typename set_type::value_type		value_type;
	typedef	typename set_type::key_compare		key_compare;
	typedef	typename set_type::allocator_type	allocator_type;

	typedef	typename set_type::reference		reference;
	typedef	typename set_type::const_reference	const_reference;
	typedef	typename set_type::iterator		iterator;
	typedef	typename set_type::const_iterator	const_iterator;
	typedef	typename set_type::reverse_iterator	reverse_iterator;
	typedef	typename set_type::const_reverse_iterator
							const_reverse_iterator;
	typedef	typename set_type::size_type		size_type;
	typedef	typename set_type::difference_type	difference_type;
	typedef	typename set_type::pointer		pointer;
	typedef	typename set_type::const_pointer	const_pointer;
	typedef	typename set_type::allocator_type	allocator_type;

	typedef	typename set_type::index_type		index_type;
	typedef	typename set_type::key_list_type	key_list_type;
	typedef	typename set_type::key_list_pair_type	key_list_pair_type;
	typedef	typename set_type::key_pair_type	key_pair_type;

	enum { dim = D };
protected:
	/**
		Since a set's value_type is the same as it's key_type, 
		we use the following typedefs to distinguish between the
		logical key and value semantics of the contained elements.  
	 */
	typedef	typename element_type::key_type		impl_key_type;
	typedef	typename element_type::value_type	impl_value_type;

public:
	multikey_set();

	~multikey_set();

	/**
		Wraps around parent set_type's insert.  
	 */
	pair<iterator, bool>
	insert(const value_type& v) {
		return set_type::insert(v);
	}

	/**
		Convenient insertion of element given separate
		key and value.  
		If no value is given, then makes a default value . 
	 */
	pair<iterator, bool>
	insert(const impl_key_type& k, const impl_value_type& v = 
			impl_value_type()) {
		return set_type::insert(value_type(k, v));
	}

#if 0
	/**
		Convenient insertion of element given separate
		key and value.  
		If no value is given, then makes a default value . 
	 */
	pair<iterator, bool>
	insert(const index_type& k, const impl_value_type& v = 
			impl_value_type()) {
		return set_type::insert(value_type(k, v));
	}
#endif

#if 1
	/**
		Searches for an element with just the key part of the element, 
		leaving the value field default constructed.  
		Const-semantics, guaranteeing no creation of blank entries.  
	 */
	const_iterator
	find(const key_type& k) const {
		return set_type::find(value_type(k));
	}

#if 0
	/**
		Non-const version, used by erase() implementations.  
	 */
	iterator
	find(const key_type& k) {
		return set_type::find(value_type(k));
	}
#endif
#else
	using set_type::find;
#endif

	size_type
	erase(const key_type& k) {
		return set_type::erase(value_type(k));
	}

	void
	clean(void);

	// everything else inherited

	ostream&
	dump(ostream& o) const;

};	// end class multikey_set

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
SPECIALIZED_MULTIKEY_SET_TEMPLATE_SIGNATURE
class multikey_set<1, T, S> : public multikey_assoc<1, S<T> > {
private:
	typedef	multikey_set<1, T, S>			this_type;
protected:
	typedef	multikey_assoc<1, S<T> >		set_type;
	typedef	T					element_type;
public:
	typedef	typename set_type::key_type		key_type;
//	typedef	typename set_type::mapped_type		mapped_type;
	typedef	typename set_type::value_type		value_type;
	typedef	typename set_type::key_compare		key_compare;
	typedef	typename set_type::allocator_type	allocator_type;

	typedef	typename set_type::reference		reference;
	typedef	typename set_type::const_reference	const_reference;
	typedef	typename set_type::iterator		iterator;
	typedef	typename set_type::const_iterator	const_iterator;
	typedef	typename set_type::reverse_iterator	reverse_iterator;
	typedef	typename set_type::const_reverse_iterator
							const_reverse_iterator;
	typedef	typename set_type::size_type		size_type;
	typedef	typename set_type::difference_type	difference_type;
	typedef	typename set_type::pointer		pointer;
	typedef	typename set_type::const_pointer	const_pointer;
	typedef	typename set_type::allocator_type	allocator_type;

	typedef	typename set_type::index_type		index_type;
	typedef	typename set_type::key_list_type	key_list_type;
	typedef	typename set_type::key_list_pair_type	key_list_pair_type;
	typedef	typename set_type::key_pair_type	key_pair_type;

	enum { dim = 1 };
protected:
	/**
		Since a set's value_type is the same as it's key_type, 
		we use the following typedefs to distinguish between the
		logical key and value semantics of the contained elements.  
	 */
	typedef	typename element_type::key_type		impl_key_type;
	typedef	typename element_type::value_type	impl_value_type;

public:
	multikey_set();

	~multikey_set();

	/**
		Wraps around parent set_type's insert.  
	 */
	pair<iterator, bool>
	insert(const value_type& v) {
		return set_type::insert(v);
	}

	/**
		Convenient insertion of element given separate
		key and value.  
	 */
	pair<iterator, bool>
	insert(const impl_key_type& k, const impl_value_type& v) {
		return set_type::insert(value_type(k, v));
	}

	/**
		Convenient insertion of element given separate
		key and value.  
	 */
	pair<iterator, bool>
	insert(const index_type& k, const impl_value_type& v) {
		return set_type::insert(value_type(k, v));
	}

#if 1
	/**
		Inserts a default constructed element at the key.  
	 */
	pair<iterator, bool>
	insert(const impl_key_type& k) {
		return set_type::insert(value_type(k));
	}
#else
	/**
		Inserts a default constructed element at the key.  
	 */
	pair<iterator, bool>
	insert(const index_type& k) {
		return set_type::insert(value_type(k));
	}
#endif

	/**
		Searches for an element with just the key part of the element, 
		leaving the value field default constructed.  
		Const-semantics, guaranteeing no creation of blank entries.  
	 */
	const_iterator
	find(const impl_key_type& k) const {
		return set_type::find(value_type(k));
	}

	// everything else inherited

	ostream&
	dump(ostream& o) const;

};	// end class multikey_set
#endif

//=============================================================================

/**
	We provide this as a convenient default class that satisfies the 
	multikey_set containee requirements.  
	\param D is the dimensionality of the key.  
	\param K is the (integer-like) index type, such as size_t.
	\param T is the value_type, as in the second element type of 
		std::pair<const key_type, value_type>.
 */
MULTIKEY_SET_ELEMENT_TEMPLATE_SIGNATURE
class multikey_set_element {
private:
	typedef	multikey_set_element<D,K,T>		this_type;
public:
	typedef	K					index_type;
//	typedef	typename multikey<D,K>::implementation_type
	typedef	multikey<D,K>				key_type;
	typedef	T					value_type;
	/**
		Workaround for multikey_assoc breaking key's constness.
	 */
	typedef	key_type				self_key_type;
	enum { dim = D };
protected:
	/**
		Should this be const like map's value_type?
		Yes, else the set may become unsorted through
		unauthorized tampering.  
		HOWEVER, we need to be able to perform arithmetic
		operations on it, like ++, when evaluating bounds, 
		which prevents us from declaring it as const.
		These are ok situations...
	 */
	const key_type					key;

	/**
		The value contained with the key.
		Publicly accessible for convenience?
	 */
	value_type					value;
public:
	// when does an element get default-constructed?
	multikey_set_element() : key(), value() { }

#if 0
	/**
		Really a special case constructor intended for D = 1.
	 */
	multikey_set_element(const index_type k, const value_type& v =
			value_type()) : key(k), value(v) { }
#endif

	explicit
	multikey_set_element(const key_type& k, const value_type& v =
			value_type()) : key(k), value(v) { }

	// default copy-constructor

	// default destructor

	const key_type&
	self_key(void) const { return key; }

#if 1
	value_type&
	get_value(void) { return value; }

	const value_type&
	get_value(void) const { return value; }
#endif

	/**
		WARNING: abusing implicit conversion operator!
	 */
	operator value_type& () { return value; }

	operator const value_type& () { return value; }

#if 0
	// make it usable by find...
	operator const key_type& () { return key; }
#endif

	const index_type&
	operator [] (const size_t i) const { return key[i]; }

	/**
		ALERT! confusing operator overload combination ahead!
		When comparing order, compare key.  
		This operator is used to sort the set.  
	 */
	bool
	operator < (const this_type& p) const {
		return key < p.key;
	}

	/**
		ALERT! confusing operator overload combination ahead!
		When comparing equality, compare value only!
		This is useful in determining whether or not
		elements contained at a position are equal, 
		by ignoring their sorting "keys".  
		See where this applies in "multikey_assoc.tcc".
	 */
	bool
	operator == (const this_type& p) const {
		return value == p.value;
	}

	bool
	operator != (const this_type& p) const {
		return !(value == p.value);
	}

};	// end class multikey_set_element

//=============================================================================
}	// end namespace util

#endif	// __UTIL_MULTIKEY_SET_H__

