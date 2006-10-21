/**
	\file "util/multikey_assoc.h"
	Multidimensional map implemented as plain map with 
	multidimensional key.  
	$Id: multikey_assoc.h,v 1.5.58.1 2006/10/21 02:50:10 fang Exp $
 */

#ifndef	__UTIL_MULTIKEY_ASSOC_H__
#define	__UTIL_MULTIKEY_ASSOC_H__

#include "util/macros.h"
#include "util/multikey_assoc_fwd.h"
#include "util/STL/list_fwd.h"
#include "util/STL/pair_fwd.h"
#include "util/array_traits.h"

namespace util {
using std::pair;
using std::list;
using std::default_list;

template <size_t, class>
class multikey;

//=============================================================================
/**
	The purpose of this class is to export the is_compact
	and is_compact_slice functionality to other multikey-associative
	(ordered) containers.  
	The nice thing about this is that it doesn't care what the 
	value type of the container is, just the key.  
 */
template <size_t D, class K>
struct multikey_assoc_compact_helper {
	typedef	multikey<D,K>				key_type;
	typedef	typename key_type::value_type		index_type;
	typedef	typename default_list<index_type>::type	key_list_type;
	typedef	pair<key_list_type, key_list_type >	key_list_pair_type;
	typedef	pair<key_type, key_type>		key_pair_type;

	template <class A>
	static
	key_list_pair_type
	is_compact_slice(const A&, 
		const key_list_type& l, const key_list_type& u);

	template <class A>
	static
	key_list_pair_type
	is_compact(const A&);

	template <class A>
	static
	key_list_pair_type
	index_extremities(const A&);

private:
	template <class A>
	static
	key_list_pair_type
	__is_dense_subslice(const A&, const key_list_type&);

};	// end class multikey_assoc_compact_helper

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Partial specialization.
 */
template <class K>
struct multikey_assoc_compact_helper<1,K> {
	typedef	K					key_type;
	typedef	key_type				index_type;
//	typedef	typename key_type::simple_type		index_type;
	typedef	typename default_list<index_type>::type	key_list_type;
	typedef	pair<key_list_type, key_list_type >	key_list_pair_type;
	typedef	pair<key_type, key_type>		key_pair_type;

	template <class A>
	static
	key_list_pair_type
	is_compact_slice(const A&, 
		const key_list_type& l, const key_list_type& u);

	template <class A>
	static
	key_list_pair_type
	is_compact(const A&);

};	// end class multikey_assoc_compact_helper

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// template functions, using template argument deduction

//=============================================================================
/**
	Adapter class for pseudo-multidimensional containers, such as
		maps and sets.  
	This class provides facilities for querying
	the compactness and existence of multidimensional slices.  

	\param D is the dimension.  Technically, this icould be inferred
		from deep within the container's class typedefs, 
		but we also use it to partially specialize.  
	\param C is the container of multidimensionally indexed elements.  
		Inherit publicly or privately?
 */
MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
class multikey_assoc : public C {
	typedef	multikey_assoc<D,C>			this_type;
protected:
	/** this is the representation-type */
	typedef	C					assoc_type;
public:
	typedef	typename assoc_type::value_type		value_type;
	/**
		Using the "simple_type" of the multikey, automatically
		specializes associative structures for the case where
		dimension = 1.
		The removes the burden of having to specialize
		multikey_map and multikey_set separately.  
	 */
	typedef	typename assoc_type::key_type::simple_type
							key_type;
	typedef	typename assoc_type::mapped_type	mapped_type;
	typedef	typename assoc_type::key_compare	key_compare;
	typedef	typename assoc_type::allocator_type	allocator_type;

	typedef	typename assoc_type::reference		reference;
	typedef	typename assoc_type::const_reference	const_reference;
	typedef	typename assoc_type::iterator		iterator;
	typedef	typename assoc_type::const_iterator	const_iterator;
	typedef	typename assoc_type::reverse_iterator	reverse_iterator;
	typedef	typename assoc_type::const_reverse_iterator
							const_reverse_iterator;
	typedef	typename assoc_type::size_type		size_type;
	typedef	typename assoc_type::difference_type	difference_type;
	typedef	typename assoc_type::pointer		pointer;
	typedef	typename assoc_type::const_pointer	const_pointer;

	// this only works for maps... set::key_type == set::value_type :S
	typedef	typename key_type::value_type		index_type;
	typedef	typename default_list<index_type>::type	key_list_type;
	typedef	pair<key_list_type, key_list_type >	key_list_pair_type;
	typedef	pair<key_type, key_type>		key_pair_type;

protected:
	typedef	multikey_assoc_compact_helper<D, index_type>
							compact_helper_type;
public:
	// for array_traits<> interface
	enum { dim = D };

public:
	/**
		Default empty constructor.  
	 */
	multikey_assoc();

	/**
		Default destructor.
	 */
	~multikey_assoc();

	using assoc_type::empty;
	using assoc_type::size;
	using assoc_type::clear;

	/**
		Number of dimensions.
	 */
	size_t
	dimensions(void) const { return D; }

	/**
		\return The number of elements (leaves) in map.  
	 */
	size_t
	population(void) const { return this->size(); }

	using assoc_type::begin;
	using assoc_type::end;
	using assoc_type::rbegin;
	using assoc_type::rend;

	iterator
	find(const key_type& k) {
		return assoc_type::find(k);
	}

	const_iterator
	find(const key_type& k) const {
		return assoc_type::find(k);
	}

	iterator
	lower_bound(const key_type& k) {
		return assoc_type::lower_bound(k);
	}

	const_iterator
	lower_bound(const key_type& k) const {
		return assoc_type::lower_bound(k);
	}

	iterator
	upper_bound(const key_type& k) {
		return assoc_type::upper_bound(k);
	}

	const_iterator
	upper_bound(const key_type& k) const {
		return assoc_type::upper_bound(k);
	}

	/**
		\param K may be underspecified key (fewer dimensions).
		\param k The key of the (key, value) pair to find.  
		\return First element >= key k, or end().  
	 */
	template <class K>
	iterator
	lower_bound(const K& k);

	template <class K>
	const_iterator
	lower_bound(const K& k) const;

	/**
		\param K may be underspecified key (fewer dimensions).
		\param k The key of the (key, value) pair to find.  
		\return First element > key k, or end().  
	 */
	template <class K>
	iterator
	upper_bound(const K& k);

	template <class K>
	const_iterator
	upper_bound(const K& k) const;

	// specializations to allow dimension-1 (POD) indices
	iterator
	lower_bound(const index_type& k);

	const_iterator
	lower_bound(const index_type& k) const;

	iterator
	upper_bound(const index_type& k);

	const_iterator
	upper_bound(const index_type& k) const;

	/**
		How many members match the given prefix of sub-dimensions?
	 */
	template <class K>
	size_type
	count(const K& k) const;

	/** specialization for D2 == 1 */
	size_type
	count(const index_type i) const;

	size_type
	erase(const key_type& k) {
		return assoc_type::erase(k);
	}

	void
	erase(iterator i) {
		assoc_type::erase(i);
	}

	template <class K>
	size_type
	erase(const K& k);

	/** specialization of erase() for only 1 dimension specified */
	size_type
	erase(const index_type i);

	/**
		Recursive routine to determine implicit desnsely 
		packed subslice.  
		\param r list of upper bound and lower bounds.  
		\return explicit indices for the subarray, or pair of empty
			lists if sub-array is not densely packed.
	 */
	key_list_pair_type
	is_compact_slice(const key_list_type& l, const key_list_type& u) const;

	/**
		"Is the entire set compact?"
		\return explicit indices covering the entire set if it
			is indeed compact, otherwise empty.  
	 */
	key_list_pair_type
	is_compact(void) const;

	/**
		Returns the extremities of the indicies in each dimension.
		If empty, returns empty lists.  
	 */
	key_list_pair_type
	index_extremities(void) const;

#if 0
private:
	key_list_pair_type
	__is_dense_subslice(const key_list_type&) const;
#endif

};	// end class multikey_assoc

//-----------------------------------------------------------------------------
/**
	Specialization for one-dimension: just use base map type.  
	\param C the underlying associaltive container type (map, set,...)
 */
SPECIALIZED_MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
class multikey_assoc<1,C> : public C {
protected:
	typedef	C					assoc_type;
public:
	typedef	typename assoc_type::value_type		value_type;
	typedef	typename assoc_type::key_type		key_type;
	typedef	typename assoc_type::mapped_type	mapped_type;
	typedef	typename assoc_type::key_compare	key_compare;
	typedef	typename assoc_type::allocator_type	allocator_type;

	typedef	typename assoc_type::reference		reference;
	typedef	typename assoc_type::const_reference	const_reference;
	typedef	typename assoc_type::iterator		iterator;
	typedef	typename assoc_type::const_iterator	const_iterator;
	typedef	typename assoc_type::reverse_iterator	reverse_iterator;
	typedef	typename assoc_type::const_reverse_iterator
							const_reverse_iterator;
	typedef	typename assoc_type::size_type		size_type;
	typedef	typename assoc_type::difference_type	difference_type;
	typedef	typename assoc_type::pointer		pointer;
	typedef	typename assoc_type::const_pointer	const_pointer;

	// only works for map's type:
	typedef	key_type				index_type;
//	typedef	typename key_type::simple_type		index_type;
	typedef	typename default_list<index_type>::type	key_list_type;
	typedef	pair<key_list_type, key_list_type >	key_list_pair_type;
	typedef	pair<key_type, key_type>		key_pair_type;

	// for array_traits<>
	enum { dim = 1 };
private:
	typedef	multikey_assoc_compact_helper<1,index_type>
						compact_helper_type;
public:
	multikey_assoc();

	~multikey_assoc();

	using assoc_type::empty;
	using assoc_type::clear;
	using assoc_type::size;

	size_t
	dimensions(void) const { return 1; }

	size_t
	population(void) const { return this->size(); }

	using assoc_type::begin;
	using assoc_type::end;
	using assoc_type::rbegin;
	using assoc_type::rend;
	using assoc_type::find;

	key_list_pair_type
	is_compact_slice(const key_list_type& l, const key_list_type& u) const;

	/**
		With one argument, is always true.
	 */
	key_list_pair_type
	is_compact_slice(const key_list_type& l) const {
		typedef	key_list_pair_type	return_type;
		return return_type(l,l);
	}

	key_list_pair_type
	is_compact(void) const;

	// all other methods are the same as general template class

};	// end class multikey_assoc (specialization)

//=============================================================================
}	// end namespace util

namespace util {

MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
struct array_traits<util::multikey_assoc<D,C> > {
	typedef	util::multikey_assoc<D,C>
				array_type;
	enum { dimensions = array_type::dim };
};	// end struct array_traits

}	// end namespace util

#endif	// __UTIL_MULTIKEY_ASSOC_H__

