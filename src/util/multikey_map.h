// "multikey_map.h"

#ifndef	__MULTIKEY_MAP_H__
#define	__MULTIKEY_MAP_H__

#include <limits>		// for numeric_limits specializations
#include <iostream>
#include "multikey.h"
#include "multikey_map_fwd.h"

namespace MULTIKEY_MAP_NAMESPACE {
using MULTIKEY_NAMESPACE::multikey_base;
using MULTIKEY_NAMESPACE::multikey;

//=============================================================================
/**
	Abstract base class for pseudo-multidimensional map.
	Implementation-independent.  
 */
template <class K, class T>
class multikey_map_base {
public:
	typedef	multikey_map_base<K,T>			this_type;
public:
	static const size_t				LIMIT = 4;
public:
virtual	~multikey_map_base() { }

virtual	size_t dimensions(void) const = 0;
virtual	size_t population(void) const = 0;
virtual	bool empty(void) const = 0;
virtual	void clear(void) = 0;
virtual	void clean(void) = 0;

virtual	T& operator [] (const multikey_base<K>& k) = 0;
virtual	T operator [] (const multikey_base<K>& k) const = 0;

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
template <size_t D, class K, class T, template <class, class> class M>
class multikey_map : public M<multikey<D,K>, T>, multikey_map_base<K,T> {
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

public:
	/**
		Default empty constructor.  
	 */
	multikey_map() : map_type(), interface_type() { }

	/**
		Default destructor.
	 */
	~multikey_map() { }

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
	population(void) const { return this->size(); }

	/**
		Removes all elements.
	 */
	void
	clear(void) {
		map_type::clear();
	}

	/**
		General method for removing default values.  
	 */
	void
	clean(void) {
		const T def;
		iterator i = this->begin();
		const const_iterator e = this->end();
		for ( ; i!=e; ) {
			if (i->second == def) {
				iterator j = i;
				j++;
				map_type::erase(i);
				i = j;
			} else {
				i++;
			}
		}
	}

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

	T& operator [] (const typename map_type::key_type& k) {
		return map_type::operator[](k);
	}

	T operator [] (const typename map_type::key_type& k) const {
		return map_type::operator[](k);
	}

	T& operator [] (const multikey_base<K>& k) {
		// what if initial value is different?
		const multikey<D,K>* dk =
			dynamic_cast<const multikey<D,K>*>(&k);
		assert(dk);
		return map_type::operator[](*dk);
	}

	T operator [] (const multikey_base<K>& k) const {
		// what if initial value is different?
		const multikey<D,K>* dk =
			dynamic_cast<const multikey<D,K>*>(&k);
		assert(dk);
		return map_type::operator[](*dk);
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
class multikey_map<1,K,T,M> : public M<K,T>, public multikey_map_base<K,T> {
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

public:
	multikey_map() : map_type(), interface_type() { }
	~multikey_map() { }

	bool
	empty(void) const {
		return map_type::empty();
	}

	void
	clear(void) {
		map_type::clear();
	}

	size_t
	dimensions(void) const { return 1; }

	size_t
	population(void) const { return this->size(); }

	void
	clean(void) {
		const T def;
		iterator i = this->begin();
		const const_iterator e = this->end();
		for ( ; i!=e; ) {
			if (i->second == def) {
				iterator j = i;
				j++;
				this->erase(i);
				i = j;
			} else {
				i++;
			}
		}
	}

	T& operator [] (const typename map_type::key_type& k) {
		return map_type::operator[](k);
	}

	T operator [] (const typename map_type::key_type& k) const {
		return map_type::operator[](k);
	}

	T& operator [] (const multikey_base<K>& k) {
		// what if initial value is different?
		const multikey<1,K>* dk =
			dynamic_cast<const multikey<1,K>*>(&k);
		assert(dk);
		return map_type::operator[]((*dk)[0]);
	}

	T operator [] (const multikey_base<K>& k) const {
		// what if initial value is different?
		const multikey<1,K>* dk =
			dynamic_cast<const multikey<1,K>*>(&k);
		assert(dk);
		return map_type::operator[]((*dk)[0]);
	}


	ostream&
	dump(ostream& o) const {
		const_iterator i = this->begin();
		const const_iterator e = this->end();
		for ( ; i!=e; i++)
			o << '[' << i->first << ']' << " = "
				<< i->second << endl;
		return o;
	}

	// all other methods are the same as general template class

};	// end class multikey_map specialization

//=============================================================================
// static function definitions

template <class K, class T>
template <template <class, class> class M>
multikey_map_base<K,T>*
multikey_map_base<K,T>::make_multikey_map(const size_t d) {
	// slow switch-case, but we need constants
	assert(d > 0 && d <= LIMIT);
	// there may be some clever way to make a call table to
	// the various constructors, but this is a rare operation: who cares?
	switch(d) {
		case 1: return new multikey_map<1,K,T,M>();
		case 2: return new multikey_map<2,K,T,M>();
		case 3: return new multikey_map<3,K,T,M>();
		case 4: return new multikey_map<4,K,T,M>();
		// add more cases if LIMIT is ever extended.
		default: return NULL;
	}
}

//=============================================================================
}	// end namespace MULTIKEY_MAP_NAMESPACE

#endif	//	__MULTIKEY_MAP_H__

