/**
	\file "multikey_assoc.h"
	Multidimensional map implemented as plain map with 
	multidimensional key.  
	$Id: multikey_assoc.h,v 1.1.2.3 2005/02/08 22:45:12 fang Exp $
 */

#ifndef	__UTIL_MULTIKEY_ASSOC_H__
#define	__UTIL_MULTIKEY_ASSOC_H__

// #include <iosfwd>
#include "macros.h"
#include "multikey_assoc_fwd.h"
#include "assoc_traits.h"		// for set_element_traits
#include "STL/list_fwd.h"
#include "STL/pair_fwd.h"

// #include "multikey_fwd.h"
#include "array_traits.h"

namespace util {
// using std::ostream;
// using std::istream;
using std::pair;
USING_LIST
// using MULTIKEY_NAMESPACE::multikey;

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
#if 1
	typedef	typename assoc_type::key_type		key_type;
	typedef	typename assoc_type::mapped_type	mapped_type;
#else
	typedef	typename set_element_traits<value_type>::key_type
							key_type;
	typedef	typename set_element_traits<value_type>::mapped_type
							mapped_type;
#endif
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
	typedef	typename assoc_type::allocator_type	allocator_type;

	// this only works for maps... set::key_type == set::value_type :S
#if 1
	typedef	typename key_type::value_type		index_type;
#else
	/**
		Dirty hack from "assoc_traits.h".
		The key_type corresponds to a multikey(-like) class.  
		The key_type type has a value_type member which corresponds
		(most likely) to some built-in integer type.
	 */
	typedef	typename set_element_traits<value_type>::key_type::value_type
							index_type;
#endif
	typedef	list<index_type>			key_list_type;
	typedef	pair<key_list_type, key_list_type >	key_list_pair_type;
	typedef	pair<key_type, key_type>		key_pair_type;

protected:
	typedef	typename assoc_type::key_type		find_arg_type;

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

#if 0
	/**
		General method for removing default values.  
	 */
	void
	clean(void);
#endif

	using assoc_type::begin;
	using assoc_type::end;
	using assoc_type::rbegin;
	using assoc_type::rend;

	// weird: using "find_arg_type"
	iterator
	find(const find_arg_type& k) {
		return assoc_type::find(k);
	}

	const_iterator
	find(const find_arg_type& k) const {
		return assoc_type::find(k);
	}

#if 0
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
#else
	iterator
	lower_bound(const find_arg_type& k) {
		return assoc_type::lower_bound(k);
	}

	const_iterator
	lower_bound(const find_arg_type& k) const {
		return assoc_type::lower_bound(k);
	}

	iterator
	upper_bound(const find_arg_type& k) {
		return assoc_type::upper_bound(k);
	}

	const_iterator
	upper_bound(const find_arg_type& k) const {
		return assoc_type::upper_bound(k);
	}
#endif

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

#if 1
	/** specialization for D2 == 1 */
	size_type
	count(const index_type i) const;
#endif

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

#if 1
	/** specialization of erase() for only 1 dimension specified */
	size_type
	erase(const index_type i);
#endif

#if 0
	T&
	operator [] (const key_type& k) {
		return assoc_type::operator[](k);
	}

	T
	operator [] (const key_type& k) const {
		return assoc_type::operator[](k);
	}

	/**
		Check length of list?
	 */
	T&
	operator [] (const list<K>& k);

	/**
		Check length of list?
	 */
	T
	operator [] (const list<K>& k) const;
#endif

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
		This version queries one set of indices only.  
		Is self-recursive.  
		\return pair of index lists representing explicit
			indices of a dense slice, or empty if failure.  
	 */
	key_list_pair_type
	is_compact_slice(const key_list_type& l) const;

	/**
		"Is the entire set compact?"
		\return explicit indices covering the entire set if it
			is indeed compact, otherwise empty.  
	 */
	key_list_pair_type
	is_compact(void) const;

#if 0
	ostream&
	dump(ostream& o) const;
#endif

	/**
		Returns the extremities of the indicies in each dimension.
		If empty, returns empty lists.  
	 */
	key_list_pair_type
	index_extremities(void) const;

public:
#if 0
	// IO methods
	ostream&
	write(ostream& f) const;

	istream&
	read(istream& f);
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
#if 0
	typedef	typename assoc_type::key_type		key_type;
#else
	typedef	typename set_element_traits<value_type>::key_type
							key_type;
	typedef	typename set_element_traits<value_type>::mapped_type
							mapped_type;
#endif
//	typedef	typename assoc_type::mapped_type	mapped_type;
//	typedef	typename assoc_type::key_compare	key_compare;
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
	typedef	typename assoc_type::allocator_type	allocator_type;

	// only works for map's type:
#if 1
	typedef	key_type				index_type;
#else
	typedef	typename set_element_traits<value_type>::key_type
							index_type;
#endif
	typedef	list<key_type>				key_list_type;
	typedef	pair<key_list_type, key_list_type >	key_list_pair_type;
	typedef	pair<key_type, key_type>		key_pair_type;

	// for array_traits<>
	enum { dim = 1 };
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

#if 0
	void
	clean(void);
#endif

	using assoc_type::begin;
	using assoc_type::end;
	using assoc_type::rbegin;
	using assoc_type::rend;
	using assoc_type::find;

#if 0
	T&
	operator [] (const key_type& k) {
		return assoc_type::operator[](k);
	}

	T
	operator [] (const key_type& k) const {
		return assoc_type::operator[](k);
	}

	T&
	operator [] (const key_list_type& k);

	T
	operator [] (const key_list_type& k) const;

	T&
	operator [] (const multikey<1,K>& k) {
		return assoc_type::operator[](k[0]);
	}

	T
	operator [] (const multikey<1,K>& k) const {
		return assoc_type::operator[](k[0]);
	}
#endif

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

#if 0
	ostream&
	dump(ostream& o) const;

	ostream&
	write(ostream& f) const;

	istream&
	read(istream& f);
#endif

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

