/**
	\file "util/multikey_map.hh"
	Multidimensional map implemented as plain map with 
	multidimensional key.  
	$Id: multikey_map.hh,v 1.19 2006/04/27 00:17:07 fang Exp $
 */

#ifndef	__UTIL_MULTIKEY_MAP_H__
#define	__UTIL_MULTIKEY_MAP_H__

#include "util/macros.h"
#include "util/STL/list_fwd.hh"
#include "util/STL/pair_fwd.hh"

#include "util/multikey_fwd.hh"
#include "util/multikey_map_fwd.hh"
#include "util/array_traits.hh"
#include "util/multikey_assoc.hh"
#include "util/type_traits.hh"
#include "util/static_assert.hh"

namespace util {
using std::ostream;
using std::istream;
using std::pair;
using std::list;

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
class multikey_map : public multikey_assoc<D, M> {
private:
	typedef	multikey_map<D,K,T,M>			this_type;
protected:
	/** this is the representation-type */
	typedef	multikey_assoc<D, M>			map_type;
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
	template <class Alloc>
	T&
	operator [] (const list<K,Alloc>& k);

	/**
		Check length of list?
	 */
	template <class Alloc>
	T
	operator [] (const list<K,Alloc>& k) const;

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

