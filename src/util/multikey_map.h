/**
	\file "multikey_map.h"
	Multidimensional map implemented as plain map with 
	multidimensional key.  
	$Id: multikey_map.h,v 1.14.24.8 2005/02/08 06:41:24 fang Exp $
 */

#ifndef	__UTIL_MULTIKEY_MAP_H__
#define	__UTIL_MULTIKEY_MAP_H__

#define	USE_MULTIKEY_ASSOC			1

#include "macros.h"
#include "STL/list_fwd.h"
#include "STL/pair_fwd.h"

#include "multikey_fwd.h"
#include "multikey_map_fwd.h"
#include "array_traits.h"

#if USE_MULTIKEY_ASSOC
#include "multikey_assoc.h"
#endif

namespace MULTIKEY_MAP_NAMESPACE {
using std::ostream;
using std::istream;
using std::pair;
USING_LIST
using MULTIKEY_NAMESPACE::multikey;

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
class multikey_map :
#if USE_MULTIKEY_ASSOC
	public multikey_assoc<D, M< multikey<D,K>, T> >
#else
	protected M<multikey<D,K>, T>
#endif
{
private:
	typedef	multikey_map<D,K,T,M>			this_type;
protected:
	/** this is the representation-type */
#if USE_MULTIKEY_ASSOC
	typedef	multikey_assoc<D, M<multikey<D,K>, T> >	map_type;
#else
	typedef	M<multikey<D,K>, T>			map_type;
#endif
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

	typedef	typename mt::key_list_type		key_list_type;
	typedef	typename mt::key_list_pair_type		key_list_pair_type;
	typedef	typename mt::key_pair_type		key_pair_type;

public:
	// for array_traits<> interface
	enum { dim = D };

public:
	/**
		Default empty constructor.  
	 */
	multikey_map();

	/**
		Default destructor.
	 */
	~multikey_map();

	using map_type::empty;
	using map_type::size;
	using map_type::clear;
#if 0
	/**
		Whether or not this map contains any elements.
		Need final overrider here to resolve ambiguity.  
	 */
	bool
	empty(void) const {
		return map_type::empty();
	}
#endif

#if 0
	/**
		Number of dimensions.
	 */
	size_t
	dimensions(void) const { return D; }
#endif

#if 0
	size_t
	size(void) const { return this->population(); }
#endif

#if 0
	/**
		\return The number of elements (leaves) in map.  
	 */
	size_t
	population(void) const { return mt::size(); }
#endif

#if 0
	/**
		Removes all elements.
	 */
	void
	clear(void);
#endif

#if 0
	/**
		General method for removing default values.  
	 */
	void
	clean(void);
#endif

	using map_type::begin;
	using map_type::end;
	using map_type::rbegin;
	using map_type::rend;

#if !USE_MULTIKEY_ASSOC
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
#endif

	T&
	operator [] (const key_type& k) {
		return map_type::operator[](k);
	}

	T
	operator [] (const key_type& k) const {
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

#if !USE_MULTIKEY_ASSOC
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

	/**
		Returns the extremities of the indicies in each dimension.
		If empty, returns empty lists.  
	 */
	key_list_pair_type
	index_extremities(void) const;
#endif

public:
	ostream&
	dump(ostream& o) const;

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
class multikey_map<1,K,T,M> : 
#if USE_MULTIKEY_ASSOC
		public multikey_assoc<1, M<K,T> >
#else
		protected M<K,T>
#endif
{
private:
	typedef	multikey_map<1,K,T,M>			this_type;
protected:
#if USE_MULTIKEY_ASSOC
	typedef	multikey_assoc<1, M<K,T> >		map_type;
#else
	typedef	M<K, T>					map_type;
#endif
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

	typedef	typename mt::key_list_type		key_list_type;
	typedef	typename mt::key_list_pair_type		key_list_pair_type;
	typedef	typename mt::key_pair_type		key_pair_type;

	// for array_traits<>
	enum { dim = 1 };
public:
	multikey_map();

	~multikey_map();

#if 0
	bool
	empty(void) const {
		return map_type::empty();
	}

	void
	clear(void);

	size_t
	size(void) const { return this->population(); }
#else
	using map_type::empty;
	using map_type::clear;
	using map_type::size;
#endif

#if 0
	size_t
	dimensions(void) const { return 1; }

	size_t
	population(void) const { return mt::size(); }
#endif

#if 0
	void
	clean(void);
#endif

	using map_type::begin;
	using map_type::end;
	using map_type::rbegin;
	using map_type::rend;

	T&
	operator [] (const key_type& k) {
		return map_type::operator[](k);
	}

	T
	operator [] (const key_type& k) const {
		return map_type::operator[](k);
	}

	T&
	operator [] (const key_list_type& k);

	T
	operator [] (const key_list_type& k) const;

	T&
	operator [] (const multikey<1,K>& k) {
		return map_type::operator[](k[0]);
	}

	T
	operator [] (const multikey<1,K>& k) const {
		return map_type::operator[](k[0]);
	}

#if !USE_MULTIKEY_ASSOC
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
#endif

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
	enum { dimensions = array_type::dim };
};	// end struct array_traits

}	// end namespace util

#endif	// __UTIL_MULTIKEY_MAP_H__

