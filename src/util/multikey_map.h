/**
	\file "multikey_map.h"
	Multidimensional map implemented as plain map with 
	multidimensional key.  
	$Id: multikey_map.h,v 1.14.16.2 2005/02/17 00:10:20 fang Exp $
 */

#ifndef	__UTIL_MULTIKEY_MAP_H__
#define	__UTIL_MULTIKEY_MAP_H__

#include "macros.h"
#include "STL/list_fwd.h"
#include "STL/pair_fwd.h"

#include "multikey_fwd.h"
#include "multikey_map_fwd.h"
#include "array_traits.h"
#include "multikey_assoc.h"

namespace util {
using std::ostream;
using std::istream;
using std::pair;
USING_LIST
using util::multikey;

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
	public multikey_assoc<D, M<typename multikey<D,K>::simple_type, T> >
{
private:
	typedef	multikey_map<D,K,T,M>			this_type;
protected:
	/** this is the representation-type */
	typedef	multikey_assoc<D, M<typename multikey<D,K>::simple_type, T> >
							map_type;
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

public:
	ostream&
	dump(ostream& o) const;

	// IO methods
	ostream&
	write(ostream& f) const;

	istream&
	read(istream& f);

};	// end class multikey_map

//=============================================================================
}	// end namespace util

namespace util {

MULTIKEY_MAP_TEMPLATE_SIGNATURE
struct array_traits<util::multikey_map<D,K,T,M> > {
	typedef	util::multikey_map<D,K,T,M>	array_type;
	enum { dimensions = array_type::dim };
};	// end struct array_traits

}	// end namespace util

#endif	// __UTIL_MULTIKEY_MAP_H__

