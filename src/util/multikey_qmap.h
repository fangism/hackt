/**
	\file "multikey_qmap.h"
	Multidimensional queryable map.  
 */

#ifndef	__MULTIKEY_QMAP_H__
#define	__MULTIKEY_QMAP_H__

#include "qmap.h"
#include "multikey_map.h"

namespace MULTIKEY_MAP_NAMESPACE {
using namespace MULTIKEY_NAMESPACE;
using namespace QMAP_NAMESPACE;

//=============================================================================
/**
        Abstract base class for pseudo-multidimensional map.
        Implementation-independent.
	Resist temptation to derive from multikey_map_base.  
 */
template <class K, class T>
class multikey_qmap_base {
public:
	typedef multikey_qmap_base<K,T>			this_type;
public:
	static const size_t				LIMIT = 4;
public:
virtual ~multikey_qmap_base() { }

virtual size_t dimensions(void) const = 0;
virtual size_t population(void) const = 0;
virtual bool empty(void) const = 0;
virtual void clear(void) = 0;

virtual ostream& dump(ostream& o) const = 0;

virtual T& operator [] (const multikey_base<K>& k) = 0;
virtual T operator [] (const multikey_base<K>& k) const = 0;

static  this_type* make_multikey_qmap(const size_t d);

};      // end class multikey_qmap_base

//=============================================================================
template <size_t D, class K, class T>
class multikey_qmap : public multikey_map<D, K, T, qmap>, 
		public multikey_qmap_base<K,T> {
private:
	/** this is the representation-type */
	typedef multikey_map<D,K,T,qmap>		map_type;
	typedef map_type				mt;
public:
	typedef typename mt::key_type			key_type;
	typedef typename mt::mapped_type		mapped_type;
	typedef typename mt::value_type			value_type;
	typedef typename mt::key_compare		key_compare;
	typedef typename mt::allocator_type		allocator_type;

	typedef typename mt::reference			reference;
	typedef typename mt::const_reference		const_reference;
	typedef typename mt::iterator			iterator;
	typedef typename mt::const_iterator		const_iterator;
	typedef typename mt::reverse_iterator		reverse_iterator;
	typedef typename mt::const_reverse_iterator	const_reverse_iterator;
	typedef typename mt::size_type			size_type;
	typedef typename mt::difference_type		difference_type;
	typedef typename mt::pointer			pointer;
	typedef typename mt::const_pointer		const_pointer;
	typedef typename mt::allocator_type		allocator_type;

public:

	// need final overriders for virtual functions

	size_t
	dimensions(void) const { return map_type::dimensions(); }

	bool
	empty(void) const { return map_type::empty(); }

	void
	clear(void) { map_type::clear(); }

	size_t
	population(void) const { return map_type::population(); }

	// specialization
	void
	clean(void) {
		// same as qmap<multikey<D,K>, T>::clean()
		map_type::map_type::clean();
	}

	T& operator [] (const typename map_type::key_type& k) {
		return map_type::operator[](k);
	}

	T operator [] (const typename map_type::key_type& k) const {
		return map_type::operator[](k);
	}

	T& operator [] (const multikey_base<K>& k) {
		return map_type::operator[](k);
	}

	T operator [] (const multikey_base<K>& k) const {
		return map_type::operator[](k);
	}

	ostream&
	dump(ostream& o) const { return map_type::dump(o); }

	// everything else plain inherited

};	// end class multikey_qmap

//=============================================================================
// no need for 1-D specialization, already done in multikey_map

//=============================================================================
// No, I am NOT crazy, this is indeed a 0-D specialization
// class K is useless, but is kept for interface purposes

template <class K, class T>
class multikey_qmap<0,K,T> : public multikey_qmap_base<K,T> {
protected:
	T			val;

public:
	// only default constructor
	~multikey_qmap() { }

	size_t dimensions(void) const { return 0; }
	size_t population(void) const { return 1; }
	bool empty(void) const { return false; }

	/** writes with default value */
	void clear(void) { val = T(); }

	operator const T& () const { return val; }
	operator T& () { return val; }

	/** should we allow this? */
	T& operator [] (const multikey_base<K>& k) {
		assert(0);
		return val;
	}

	/** should we allow this? */
	T operator [] (const multikey_base<K>& k) const {
		assert(0);
		return val;
	}

	ostream& dump(ostream& o) const {
		return o << val;
	}

};	// end class multikey_qmap_base

//=============================================================================
// static function definitions

/**
	\param d dimensions of collection, may be 0.
 */
template <class K, class T>
multikey_qmap_base<K,T>*
multikey_qmap_base<K,T>::make_multikey_qmap(const size_t d) {
	// slow switch-case, but we need constants
	assert(d <= LIMIT);
	// there may be some clever way to make a call table to
	// the various constructors, but this is a rare operation: who cares?
	switch(d) {
		case 0: return new multikey_qmap<0,K,T>();      
		case 1: return new multikey_qmap<1,K,T>();      
		case 2: return new multikey_qmap<2,K,T>();
		case 3: return new multikey_qmap<3,K,T>();
		case 4: return new multikey_qmap<4,K,T>();
		// add more cases if LIMIT is ever extended.      
		default: return NULL;
	}
}

//=============================================================================
}	// end namespace MULTIKEY_MAP_NAMESPACE

#endif	//	__MULTIKEY_QMAP_H__

