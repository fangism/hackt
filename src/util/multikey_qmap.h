// "multikey_qmap.h"

#ifndef	__MULTIKEY_QMAP_H__
#define	__MULTIKEY_QMAP_H__

#include "qmap.h"
#include "multikey_map.h"

namespace MULTIKEY_MAP_NAMESPACE {
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

	ostream&
	dump(ostream& o) const { return map_type::dump(o); }

	// everything else plain inherited

};	// end class multikey_qmap

//=============================================================================
// static function definitions

template <class K, class T>
multikey_qmap_base<K,T>*
multikey_qmap_base<K,T>::make_multikey_qmap(const size_t d) {
	// slow switch-case, but we need constants
	assert(d > 0 && d <= LIMIT);
	// there may be some clever way to make a call table to
	// the various constructors, but this is a rare operation: who cares?
	switch(d) {
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

