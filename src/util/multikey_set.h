/**
	\file "multikey_set.h"
	Multidimensional set class, using multikey_assoc as base interface. 
	$Id: multikey_set.h,v 1.1.4.2 2005/02/17 00:10:21 fang Exp $
 */

#ifndef	__UTIL_MULTIKEY_SET_H__
#define	__UTIL_MULTIKEY_SET_H__

#include <iosfwd>
#include "multikey_set_fwd.h"
#include "maplikeset.h"
#include "multikey_assoc.h"
#include "multikey.h"

namespace util {
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
class multikey_set : public multikey_assoc<D, maplikeset<S<T> > > {
private:
	typedef	multikey_set<D, T, S>			this_type;
	typedef	maplikeset<S<T> >			maplikeset_type;
protected:
	typedef	multikey_assoc<D, maplikeset_type>	set_type;
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

	// EVERYTHING needed is inherited

	void
	clean(void);

	std::ostream&
	dump(std::ostream& o) const;

};	// end class multikey_set

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
class multikey_set_element :
	public maplikeset_element<typename multikey<D,K>::simple_type, T>
{
private:
	typedef	multikey_set_element<D,K,T>		this_type;
protected:
	typedef	maplikeset_element<typename multikey<D,K>::simple_type, T>
							parent_type;
public:
	typedef	K					index_type;
	typedef	typename parent_type::key_type		key_type;
	typedef	typename parent_type::value_type	value_type;
	enum { dim = D };
public:
	multikey_set_element() : parent_type() { }

	explicit
	multikey_set_element(const key_type& k, const value_type& v =
			value_type()) : parent_type(k, v) { }

	// default copy-constructor

	// default destructor

#if 0
	const index_type&
	operator [] (const size_t i) const { return key[i]; }
#endif

	const this_type&
	operator = (const value_type& v) const {
		parent_type::operator = (v);
		return *this;
	}

};	// end class multikey_set_element

//-----------------------------------------------------------------------------
/**
	Variation of multikey_set element derived from a derivable
	maplike_set_element base.  
 */
MULTIKEY_SET_ELEMENT_TEMPLATE_SIGNATURE
class multikey_set_element_derived :
	public maplikeset_element_derived<
		typename multikey<D,K>::simple_type, T> {
private:
	typedef	multikey_set_element_derived<D,K,T>	this_type;
protected:
	typedef	maplikeset_element_derived<
		typename multikey<D,K>::simple_type, T>
							parent_type;
public:
	typedef	K					index_type;
	typedef	typename parent_type::key_type		key_type;
	typedef	typename parent_type::value_type	value_type;
	typedef	typename parent_type::first_type	first_type;
	typedef	typename parent_type::second_type	second_type;
	enum { dim = D };
public:
	multikey_set_element_derived() : parent_type() { }

	explicit
	multikey_set_element_derived(const key_type& k, const value_type& v =
			value_type()) : parent_type(k, v) { }

	// default copy-constructor

	// default destructor

#if 0
	const index_type&
	operator [] (const size_t i) const { return key[i]; }
#endif

	const this_type&
	operator = (const value_type& v) const {
		parent_type::operator = (v);
		return *this;
	}
};	// end class multikey_set_element_derived

//=============================================================================
}	// end namespace util

namespace std {
using util::multikey_set_element_derived;

MULTIKEY_SET_ELEMENT_TEMPLATE_SIGNATURE
struct _Select1st<multikey_set_element_derived<D,K,T> > :
	public unary_function<multikey_set_element_derived<D,K,T>,
		typename multikey_set_element_derived<D,K,T>::first_type> {
	typedef multikey_set_element_derived<D,K,T>	pair_type;
	typedef typename pair_type::key_type		first_type;

	first_type&
	operator () (pair_type& p) const { return p.key; }

	const first_type&
	operator () (const pair_type& p) const { return p.key; }
};      // end struct _Select1st

MULTIKEY_SET_ELEMENT_TEMPLATE_SIGNATURE
struct _Select2nd<multikey_set_element_derived<D,K,T> > :
	public unary_function<multikey_set_element_derived<D,K,T>,
		typename multikey_set_element_derived<D,K,T>::second_type> {
	typedef multikey_set_element_derived<D,K,T>	pair_type;
	typedef typename pair_type::value_type		second_type;

	second_type&
	operator () (pair_type& p) const {
		return static_cast<second_type&>(p);
	}

	const second_type&
	operator () (const pair_type& p) const {
		return static_cast<const second_type&>(p);
	}
};      // end struct _Select2nd

}	// end namespace std

#endif	// __UTIL_MULTIKEY_SET_H__

