/**
	\file "maplikeset.h"
	Converts a set of special elements into a map-like interface.  
	$Id: maplikeset.h,v 1.1.4.1.2.2 2005/02/13 02:39:01 fang Exp $
 */

#ifndef	__UTIL_MAPLIKESET_H__
#define	__UTIL_MAPLIKESET_H__

// whether or not maplikeset_element is derived from std::pair
#define	MAPLIKESET_ELEMENT_PAIR		1

#if MAPLIKESET_ELEMENT_PAIR
#include <utility>		// for std::pair
#endif

namespace util {
//=============================================================================
/**
	This template class adapter handles a certain aspect of associative
	container design.  
	Maps contain tuples (usually pairs) of keys and values.  
	The values, however, are not aware of the keys with which they are 
	associated.  
	Sets contain only elements whose ordering is defined by 
	a StrictlyWeakOrdering (less).  
	The problem with sets is that the key types ARE the value types.  

	A maplikeset is an associative array whose contained elements are
	like those of a set, but whose interface is map-like in that 
	it performs lookups with a key that is NOT the whole value.

	Constraints: the key/value type contained by the set must have
	distinguishable key-value components.  
	The value component must be default constructible.  

	TODO: provide a maplikeset_element template.  

	\param S is a set container.
 */
template <class S>
class maplikeset : public S {
private:
	typedef	S					set_type;
	typedef	maplikeset<S>				this_type;
public:
	typedef	typename S::value_type			value_type;
	/**
		The value_type must have a key component used for ordering.  
	 */
	typedef	typename value_type::key_type		key_type;
	/**
		The value_type IS the mapped_type.
	 */
	typedef	typename value_type::value_type		mapped_type;
//	typedef	value_type				mapped_type;

	/**
		The key comparater IS the value comparator of sets.  
	 */
	typedef	typename set_type::key_compare		key_compare;
	typedef	typename set_type::allocator_type	allocator_type;

	/**
		Note that all of set's pointer/reference/iterator types
		are actually const!  
		However, since only the keys matter in sorting, 
		the value_types should allow the "mapped_type"
		component to be mutable without violating ordering.  
	 */
#if 0
	typedef	value_type&				reference;
#else
	typedef	typename set_type::reference		reference;
#endif
	typedef	typename set_type::const_reference	const_reference;
	/**
		Set type's iterator is actually const!
		FYI: iterator::iterator is non-const.  
	 */
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
protected:
//	typedef	typename value_type::value_type		impl_value_type;

#if 0
protected:
	/**
		Explicit conversion...
	 */
	static
	inline
	value_type
	key_to_value(const key_type& k) {
		return value_type(k);
	}
#endif

public:
	maplikeset() : set_type() { }

	~maplikeset() { }

	// we provide map-like lookup methods that use the key component
	// of the value type.  
	// everything else that doesn't deal with keys directly is inherited.  

	iterator
	find(const key_type& k) {
		return set_type::find(value_type(k));
	}

	const_iterator
	find(const key_type& k) const {
		return set_type::find(value_type(k));
	}

	iterator
	lower_bound(const key_type& k) {
		return set_type::lower_bound(value_type(k));
	}

	const_iterator
	lower_bound(const key_type& k) const {
		return set_type::lower_bound(value_type(k));
	}

	iterator
	upper_bound(const key_type& k) {
		return set_type::upper_bound(value_type(k));
	}

	const_iterator
	upper_bound(const key_type& k) const {
		return set_type::upper_bound(value_type(k));
	}

	size_type
	count(const key_type& k) const {
		return set_type::count(value_type(k));
	}

	void
	erase(iterator i) {
		set_type::erase(i);
	}

	void
	erase(iterator l, iterator u) {
		set_type::erase(l, u);
	}

	size_type
	erase(const key_type& k) {
		return set_type::erase(value_type(k));
	}

	iterator
	insert(const value_type& p) {
		return set_type::insert(p).first;
	}
	
	iterator
	insert(const key_type& k, const mapped_type& v = mapped_type()) {
		// returns pair<iterator, bool>
		return set_type::insert(value_type(k, v)).first;
	}

	iterator
	insert(iterator i, const key_type& k) {
		return set_type::insert(i, value_type(k));
	}

	/**
		Returns a reference to a whole element, a key-value pair.  
		Note that unlike map, this returns the whole element, 
		not just the mapped_type component.  
		If not found, this will create an entry with a default value.  
	 */
	reference
	operator [] (const key_type& k) {
		iterator i = lower_bound(k);
		if (i == set_type::end() || key_comp()(value_type(k), *i))
			i = insert(i, k);
		// else lower bound points to first element equal
		return *i;
	}

#if 0
	/**
		For convenience, we provide a const-query lookup operator.  
	 */
	value_type
	operator [] (const key_type& k) const {
		const const_iterator i = find(k);
		return (i != this->end()) ? *i : value_type();
	}
#endif

};	// end class maplikeset

//=============================================================================
/**
	We provide a template key-value type adapter for suitable use
	with maplikeset.  
 */
template <class K, class V>
class maplikeset_element
#if MAPLIKESET_ELEMENT_PAIR
	: public std::pair<const K, V>
#endif
{
private:
	typedef	maplikeset_element<K,V>			this_type;
#if MAPLIKESET_ELEMENT_PAIR
	typedef std::pair<const K, V>			parent_type;
#endif
public:
	typedef	K					key_type;
	typedef	V					mapped_type;
	typedef	V					value_type;
protected:
#if !MAPLIKESET_ELEMENT_PAIR
	const key_type					key;
#if 1
public:
#endif
	/**
		Kludge:
		Sets only return const pointer/reference/iterators, 
		and thus the elements may not be modified!
		However, the only part of the element that MUST remain
		const is the key which is used for sort-ordering.  
		The value component should be free to change, 
		hence, the value field is mutable.  
	 */
	mutable value_type				value;
#endif
public:
	maplikeset_element() :
#if MAPLIKESET_ELEMENT_PAIR
		parent_type() { }
#else
		key(), value() { }
#endif

	explicit
	maplikeset_element(const key_type& k, 
		const value_type& v = value_type()) :
#if MAPLIKESET_ELEMENT_PAIR
		parent_type(k, v) { }
#else
		key(k), value(v) { }
#endif

#if !MAPLIKESET_ELEMENT_PAIR
	const key_type&
	get_key(void) const { return key; }
#endif

#if MAPLIKESET_ELEMENT_PAIR
	value_type&
	value(void) const { return const_cast<this_type&>(*this).second; }

	const value_type&
	const_value(void) const { return this->second; }
#endif

#if 0
	/**
		WARNING: abusing implicit conversion operator!
	 */
	operator value_type& () { return value; }

	operator const value_type& () { return value; }
#endif

	/**
		This requires mutability of value, see its note.  
	 */
	const this_type&
	operator = (const value_type& v) const {
#if MAPLIKESET_ELEMENT_PAIR
		const_cast<this_type&>(*this).second = v;
#else
		value = v;
#endif
		return *this;
	}

	const this_type&
	operator = (const this_type& k) const {
#if MAPLIKESET_ELEMENT_PAIR
		const_cast<this_type&>(*this).second = k.second;
#else
		value = k.value;
#endif
		return *this;
	}

	/**
		ALERT! confusing operator overload combination ahead!
		When comparing order, compare key.  
		This operator is used to sort the set.  
	 */
	bool
	operator < (const this_type& p) const {
#if MAPLIKESET_ELEMENT_PAIR
		return first < p.first;
#else
		return key < p.key;
#endif
	}

#if 1
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
#if MAPLIKESET_ELEMENT_PAIR
		return second == p.second;
#else
		return value == p.value;
#endif
	}

	bool
	operator != (const this_type& p) const {
#if MAPLIKESET_ELEMENT_PAIR
		return !(second == p.second);
#else
		return !(value == p.value);
#endif
	}
#endif

};	// end class maplikeset_element

//=============================================================================
}	// end namespace util

#endif	// __UTIL_MAPLIKESET_H__

