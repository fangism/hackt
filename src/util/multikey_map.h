/**
	\file "multikey_map.h"
	Multidimensional map implemented as plain map with 
	multidimensional key.  
	$Id: multikey_map.h,v 1.14 2004/12/23 00:07:44 fang Exp $
 */

#ifndef	__MULTIKEY_MAP_H__
#define	__MULTIKEY_MAP_H__

#include "macros.h"
#include "STL/list_fwd.h"
#include "STL/pair_fwd.h"

#include "multikey_fwd.h"
#include "multikey_map_fwd.h"
#include "array_traits.h"

namespace MULTIKEY_MAP_NAMESPACE {
using std::ostream;
using std::istream;
using std::pair;
USING_LIST
using MULTIKEY_NAMESPACE::multikey_base;
using MULTIKEY_NAMESPACE::multikey;

//=============================================================================
/**
	Abstract base class for pseudo-multidimensional map.
	Implementation-independent.  
 */
BASE_MULTIKEY_MAP_TEMPLATE_SIGNATURE
class multikey_map_base {
public:
	typedef	pair<multikey_base<K>, multikey_base<K> >
							multikey_base_pair;
	typedef	multikey_map_base<K,T>			this_type;
	typedef	list<K>					key_list_type;
	typedef	pair<key_list_type, key_list_type >	key_list_pair_type;
public:
	static const size_t				LIMIT = 4;
public:
virtual	~multikey_map_base() { }

virtual	size_t dimensions(void) const = 0;
virtual	size_t population(void) const = 0;
	size_t size(void) const { return this->population(); }
virtual	bool empty(void) const = 0;
virtual	void clear(void) = 0;
virtual	void clean(void) = 0;

virtual	T& operator [] (const list<K>& k) = 0;
virtual	T operator [] (const list<K>& k) const = 0;
virtual	T& operator [] (const multikey_base<K>& k) = 0;
virtual	T operator [] (const multikey_base<K>& k) const = 0;

virtual	key_list_pair_type is_compact_slice(
		const key_list_type& l, const key_list_type& u) const = 0;
virtual	key_list_pair_type is_compact_slice(const key_list_type& l) const = 0;
virtual	key_list_pair_type is_compact(void) const = 0;

virtual	ostream& dump(ostream& o) const = 0;

template <template <class, class> class M>
static	this_type* make_multikey_map(const size_t d);

};	// end class multikey_map_base

//=============================================================================
/**
	Adapter class for pseudo-multidimensional maps.  
	\param D is the dimension.  
	\param K is the key type, such as integer.  
	\param T is the element type.  
	\param M is the map class.  {e.g. map, qmap}

	allocater? comparator?

	\example multikey_qmap_test.cc
 */
MULTIKEY_MAP_TEMPLATE_SIGNATURE
class multikey_map : protected M<multikey<D,K>, T>, 
		public multikey_map_base<K,T> {
protected:
	/** this is the representation-type */
	typedef	multikey_map_base<K,T>			interface_type;
	typedef	M<multikey<D,K>, T>			map_type;
	typedef	map_type				mt;
public:
	typedef	typename mt::key_type			key_type;
	typedef	typename mt::mapped_type		mapped_type;
	typedef	typename mt::value_type			value_type;
	typedef	typename mt::key_compare		key_compare;
	typedef	typename mt::allocator_type		allocator_type;

	typedef	typename mt::reference			reference;
	typedef	typename mt::const_reference		const_reference;
	typedef	typename mt::iterator			iterator;
	typedef	typename mt::const_iterator		const_iterator;
	typedef	typename mt::reverse_iterator		reverse_iterator;
	typedef	typename mt::const_reverse_iterator	const_reverse_iterator;
	typedef	typename mt::size_type			size_type;
	typedef	typename mt::difference_type		difference_type;
	typedef	typename mt::pointer			pointer;
	typedef	typename mt::const_pointer		const_pointer;
	typedef	typename mt::allocator_type		allocator_type;

	typedef	typename interface_type::key_list_type	key_list_type;
	typedef	typename interface_type::key_list_pair_type
							key_list_pair_type;
	typedef	pair<key_type, key_type>		key_pair_type;

public:
	// for array_traits<> interface
	static const size_t dim = D;

public:
	/**
		Default empty constructor.  
	 */
	multikey_map();

	/**
		Default destructor.
	 */
	~multikey_map();

	/**
		Whether or not this map contains any elements.
		Need final overrider here to resolve ambiguity.  
	 */
	bool
	empty(void) const {
		return map_type::empty();
	}

	/**
		Number of dimensions.
	 */
	size_t
	dimensions(void) const { return D; }

	/**
		\return The number of elements (leaves) in map.  
	 */
	size_t
	population(void) const { return mt::size(); }

	/**
		Removes all elements.
	 */
	void
	clear(void);

	/**
		General method for removing default values.  
	 */
	void
	clean(void);

	using map_type::begin;
	using map_type::end;
	using map_type::rbegin;
	using map_type::rend;

	/**
		\param k The key of the (key, value) pair to find.  
		\return First element >= key k, or end().  
	 */
	template <size_t D2>
	iterator
	lower_bound(const multikey<D2,K>& k);

	template <size_t D2>
	const_iterator
	lower_bound(const multikey<D2,K>& k) const;

	/**
		\param k The key of the (key, value) pair to find.  
		\return First element > key k, or end().  
	 */
	template <size_t D2>
	iterator
	upper_bound(const multikey<D2,K>& k);

	template <size_t D2>
	const_iterator
	upper_bound(const multikey<D2,K>& k) const;
	/**
		How many members match the given prefix of sub-dimensions?
	 */
	template <size_t D2>
	size_type
	count(const multikey<D2,K>& k) const;

	/** specialization for D2 == 1 */
	size_type
	count(const K i) const;

	template <size_t D2>
	size_type
	erase(const multikey<D2,K>& k);

	/** specialization of erase() for only 1 dimension specified */
	size_type
	erase(const K i);

	T&
	operator [] (const typename map_type::key_type& k) {
		return map_type::operator[](k);
	}

	T
	operator [] (const typename map_type::key_type& k) const {
		return map_type::operator[](k);
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

	T&
	operator [] (const multikey_base<K>& k);

	T
	operator [] (const multikey_base<K>& k) const;

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

	ostream&
	dump(ostream& o) const;

	/**
		Returns the extremities of the indicies in each dimension.
		If empty, returns empty lists.  
	 */
	key_list_pair_type
	index_extremities(void) const;

public:
	// IO methods
	ostream&
	write(ostream& f) const;

	istream&
	read(istream& f);

};	// end class multikey_map

//-----------------------------------------------------------------------------
/**
	Specialization for one-dimension: just use base map type.  
 */
SPECIALIZED_MULTIKEY_MAP_TEMPLATE_SIGNATURE
class multikey_map<1,K,T,M> : protected M<K,T>, public multikey_map_base<K,T> {
protected:
	typedef	multikey_map_base<K,T>			interface_type;
	typedef	M<K, T>					map_type;
	typedef	map_type				mt;
public:
	typedef	typename mt::key_type			key_type;
	typedef	typename mt::mapped_type		mapped_type;
	typedef	typename mt::value_type			value_type;
	typedef	typename mt::key_compare		key_compare;
	typedef	typename mt::allocator_type		allocator_type;

	typedef	typename mt::reference			reference;
	typedef	typename mt::const_reference		const_reference;
	typedef	typename mt::iterator			iterator;
	typedef	typename mt::const_iterator		const_iterator;
	typedef	typename mt::reverse_iterator		reverse_iterator;
	typedef	typename mt::const_reverse_iterator	const_reverse_iterator;
	typedef	typename mt::size_type			size_type;
	typedef	typename mt::difference_type		difference_type;
	typedef	typename mt::pointer			pointer;
	typedef	typename mt::const_pointer		const_pointer;
	typedef	typename mt::allocator_type		allocator_type;

	typedef	typename interface_type::key_list_type	key_list_type;
	typedef	typename interface_type::key_list_pair_type
							key_list_pair_type;
	typedef	pair<key_type, key_type>		key_pair_type;

	// for array_traits<>
	static const size_t dim = 1;
public:
	multikey_map();
	~multikey_map();

	bool
	empty(void) const {
		return map_type::empty();
	}

	void
	clear(void);

	size_t
	dimensions(void) const { return 1; }

	size_t
	population(void) const { return mt::size(); }

	void
	clean(void);

	using map_type::begin;
	using map_type::end;
	using map_type::rbegin;
	using map_type::rend;

	T&
	operator [] (const typename map_type::key_type& k) {
		return map_type::operator[](k);
	}

	T
	operator [] (const typename map_type::key_type& k) const {
		return map_type::operator[](k);
	}

	T&
	operator [] (const key_list_type& k);

	T
	operator [] (const key_list_type& k) const;

	T&
	operator [] (const multikey_base<K>& k);

	T
	operator [] (const multikey_base<K>& k) const;

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


	ostream&
	dump(ostream& o) const;

	ostream&
	write(ostream& f) const;

	istream&
	read(istream& f);

	// all other methods are the same as general template class

};	// end class multikey_map (specialization)

//=============================================================================
}	// end namespace MULTIKEY_MAP_NAMESPACE

namespace util {

MULTIKEY_MAP_TEMPLATE_SIGNATURE
struct array_traits<MULTIKEY_MAP_NAMESPACE::multikey_map<D,K,T,M> > {
	typedef	MULTIKEY_MAP_NAMESPACE::multikey_map<D,K,T,M>
				array_type;
	static const size_t	dimensions = array_type::dim;
};	// end struct array_traits

}

#endif	//	__MULTIKEY_MAP_H__

