// "multikey_map.h"

#ifndef	__MULTIKEY_MAP_H__
#define	__MULTIKEY_MAP_H__

#include <limits>		// for numeric_limits specializations
#include <iostream>
#include "multikey.h"
#include "multikey_map_fwd.h"

namespace MULTIKEY_MAP_NAMESPACE {
using MULTIKEY_NAMESPACE::multikey;

//=============================================================================
/**
	Adapter class for pseudo-multidimensional maps.  
	D is the dimension.  
	K is the key type, such as integer.  
	T is the element type.  
	M is the map class.  {e.g. map, qmap}
	// allocater? comparator?
 */
template <size_t D, class K, class T, template <class, class> class M>
class multikey_map : public M<multikey<D,K>, T> {
private:
	/** this is the representation-type */
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

public:
	multikey_map() : map_type() { }

	~multikey_map() { }

	/**
		\return The number of elements (leaves) in map.  
	 */
	size_t
	population(void) const { return this->size(); }

	/**
		\param k The key of the (key, value) pair to find.  
		\return First element >= key k, or end().  
	 */
	template <size_t D2, K init2>
	iterator
	lower_bound(const multikey<D2,K,init2>& k) {
		key_type x(k, numeric_limits<K>::min());
		return map_type::lower_bound(x);
	}

	template <size_t D2, K init2>
	const_iterator
	lower_bound(const multikey<D2,K,init2>& k) const {
		key_type x(k, numeric_limits<K>::min());
		return map_type::lower_bound(x);
	}

	/**
		\param k The key of the (key, value) pair to find.  
		\return First element > key k, or end().  
	 */
	template <size_t D2, K init2>
	iterator
	upper_bound(const multikey<D2,K,init2>& k) {
		key_type x(k, numeric_limits<K>::min());
		return map_type::lower_bound(x);
	}

	template <size_t D2, K init2>
	const_iterator
	upper_bound(const multikey<D2,K,init2>& k) const {
		key_type x(k, numeric_limits<K>::min());
		return map_type::lower_bound(x);
	}

	/**
		How many members match the given prefix of sub-dimensions?
	 */
	template <size_t D2, K init2>
	size_type
	count(const multikey<D2,K,init2>& k) const {
		key_type l(k);
		l[D2-1]++;
		return distance(lower_bound(k), lower_bound(l));
	}

	/** specialization for D2 == 1 */
	size_type
	count(const K i) const {
		multikey<1,K> l;
		l[0] = i;
		key_type m(l);
		m[0]++;
		return distance(lower_bound(l), lower_bound(m));
	}

	template <size_t D2, K init2>
	size_type
	erase(const multikey<D2,K,init2>& k) {
		if (D2 < D) {
			multikey<D2,K,init2> m(k);
			m[D2-1]++;
			const iterator l(lower_bound(k));
			const iterator u(lower_bound(m));
			size_type ret = distance(l,u);
			if (ret) map_type::erase(l,u);
			return ret;
		} else {	// D2 >= D
			const key_type l(k);
			const iterator f(map_type::find(l));
			if (f != this->end()) {
				map_type::erase(f);
				return 1;
			} else	return 0;
		}
	}

	/** specialization of erase() for only 1 dimension specified */
	size_type
	erase(const K i) {
		multikey<1,K> m, k;
		k[0] = i;
		m[0] = i+1;
		const iterator l(lower_bound(k));
		const iterator u(lower_bound(m));
		size_type ret = distance(l,u);
		if (ret) map_type::erase(l,u);
		return ret;
	}

	ostream&
	dump(ostream& o) const {
		const_iterator i = this->begin();
		const const_iterator e = this->end();
		for ( ; i!=e; i++)
			o << i->first << " = " << i->second << endl;
		return o;
	}
};	// end class multikey_map

//-----------------------------------------------------------------------------
/**
	Specialization for one-dimension: just use base map type.  
 */
template <class K, class T, template <class, class> class M>
class multikey_map<1,K,T,M> : public M<K,T> {
private:
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

public:
	multikey_map() : map_type() { }
	~multikey_map() { }

	size_t
	population(void) const { return this->size(); }

	ostream&
	dump(ostream& o) const {
		const_iterator i = this->begin();
		const const_iterator e = this->end();
		for ( ; i!=e; i++)
			o << '[' << i->first << ']' << " = "
				<< i->second << endl;
		return o;
	}

};	// end class multikey_map specialization

//=============================================================================
}	// end namespace MULTIKEY_MAP_NAMESPACE

#endif	//	__MULTIKEY_MAP_H__

