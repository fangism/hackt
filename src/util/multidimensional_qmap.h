// "multidimensional_qmap.h"
// a fixed depth/dimension tree representing sparsely instantiated indices
// David Fang, Cornell University, 2004

#ifndef	__MULTIDIMENSIONAL_QMAP_H__
#define	__MULTIDIMENSIONAL_QMAP_H__

#include <iostream>
#include "sstream.h"		// used by "dump" method

#include "qmap.h"		// queryable maps
#include "ptrs.h"		// for excl_ptr, not copy-constructable
#include "count_ptr.h"		// copy-constructable reference count pointers
#include "sublist.h"		// list slices

#ifndef	MULTIDIMENSIONAL_QMAP_NAMESPACE
#define	MULTIDIMENSIONAL_QMAP_NAMESPACE		fang
#endif

/**
	Namespace containing multidimensional-sparse-set classes.  
 */
namespace MULTIDIMENSIONAL_QMAP_NAMESPACE {
//=============================================================================
using namespace std;
using namespace SUBLIST_NAMESPACE;
using namespace QMAP_NAMESPACE;
using namespace COUNT_PTR_NAMESPACE;
using namespace PTRS_NAMESPACE;

//=============================================================================
// forward declarations
template <class, class>		class base_multidimensional_qmap;
template <size_t, class, class>	class multidimensional_qmap;

//=============================================================================
/**
	Abstract base-class.  
 */
template <class K, class T>
class base_multidimensional_qmap {
public:
	typedef	pair<K, K>			range_type;
	/** format for a list of ranges to be added, list is also acceptable */
	typedef	sublist<range_type>		range_arg_type;
public:
	static const size_t				LIMIT = 4;

public:
virtual	~base_multidimensional_qmap() { }

virtual	size_t dimensions(void) const = 0;

virtual	bool add_ranges(const range_arg_type& r) = 0;

protected:
virtual	ostream& dump(ostream& o, const string& pre) const = 0;

public:
virtual	ostream& dump(ostream& o) const = 0;

// static functions
	/** virtually, a virtual constructor */
static	base_multidimensional_qmap<T>* 
		make_multidimensional_qmap(const size_t d);
};	// end class base_multidimensional_qmap

//=============================================================================
/**
	A representation of sparse indices in a multidimensional array.
	Implemented as a fixed depth tree.  
	Parameter class should integer-like.  
	int D in template signature?
	Less memory, for multidimensional storage.  
	fixed dimension type-enforced?
	sparse_set<N> contains sparse_set<N-1>'s?
	problem: limits implementation's template depth.  
	Places limit on dimensionality of arrays...
	Is 4 enough?
 */
template <size_t D, class K, class T>
class multidimensional_qmap : public base_multidimensional_qmap<K, T> {
friend class multidimensional_qmap<D+1, K, T>;

protected:
	typedef	base_multidimensional_qmap<K, T>	parent;
	typedef	typename parent::range_arg_type		range_arg_type;
	typedef multidimensional_qmap<D-1, K, T>	child_type;
	/** need count_ptr to be copy-constructable */
	typedef	count_ptr<child_type>			map_value_type;
	typedef	qmap<K, map_value_type>			map_type;

protected:
	map_type					index_map;

public:
	/** default empty constructor */
	multidimensional_qmap() : parent(), index_map() { }

	/** default copy constructor */
	multidimensional_qmap(const multidimensional_qmap<D,K,T>& s) :
		parent(), index_map(s.index_map) { }

	/** default destructor */
	~multidimensional_qmap() { }

	size_t dimensions(void) const { return D; }

	/**
		\return true if there was overlap.  
	 */
	bool add_ranges(const range_arg_type& r) {
		assert(r.size() == D);
		bool overlap = false;
		typename range_arg_type::const_iterator i = r.begin();
		range_arg_type sub_range(r);
		sub_range.behead();
		K j = i->first;
		for ( ; j <= i->second; j++) {
			const map_value_type probe = 
				static_cast<const map_type&>(index_map)[j];
			if (!probe) {
				index_map[j] = map_value_type(new child_type);
			}
			map_value_type sub = index_map[j];
			assert(sub);
			if (sub->add_ranges(sub_range))
				overlap = true;
		}
		return overlap;
	}

protected:
	/**
		Prints out list of all members recursively.  
		\param o the output stream.  
		\param pre is a prefix string passed by the caller.  
		\return the output stream.
	 */
	ostream& dump(ostream& o, const string& pre) const {
		typename map_type::const_iterator i = index_map.begin();
		for ( ; i!=index_map.end(); i++) {
			ostringstream p;
			p << pre << "[" << i->first << "]";
			i->second->dump(o, p.str());
		}
		return o;
	}

public:
	ostream& dump(ostream& o) const { return dump(o, ""); }

};	// end class multidimensional_qmap

//-----------------------------------------------------------------------------
/**
	Specialization of a one-dimensional array.  
 */
template <class K, class T>
class multidimensional_qmap<1,K,T> :
		public base_multidimensional_qmap<K,T> {
friend class multidimensional_qmap<2,K,T>;

protected:
	typedef	base_multidimensional_qmap<K,T>		parent;
	typedef	typename parent::range_arg_type		range_arg_type;
	typedef	qmap<K,T>				map_type;

protected:
	/**
		Storing with this an interval map for efficiency.  
	 */
	map_type					index_map;

public:
	/** default empty constructor */
	multidimensional_qmap() : parent(), index_map() { }

	/** default copy constructor */
	multidimensional_qmap(const multidimensional_qmap<1,K,T>& s) :
		parent(), index_map(s.index_map) { }

	/** default destructor */
	~multidimensional_qmap() { }

	size_t dimensions(void) const { return 1; }

	/**
		Can only add default values.  
		\return true if there was overlap.  
	 */
	bool add_ranges(const range_arg_type& r) {
		assert(r.size() == 1);
		typename range_arg_type::const_iterator i = r.begin();
		K j = i->first;
		for ( ; i <= i->second; j++) {
			T default;
			// query-probe first
			if (static_cast<const map_type&>(index_map)[j]
					== default) {
				index_map[j] = T();	// assign default
			} else {
				return true;
			}
		}
		return true;
	}

protected:
	/**
		Prints out list of all members recursively.  
		\param o the output stream.  
		\param pre is a prefix string passed by the caller.  
		\return the output stream.
	 */
	ostream& dump(ostream& o, const string& pre) const {
		typename map_type::const_iterator i = index_map.begin();
		for ( ; i!=index_map.end(); i++) {
			o << pre << "[" << i->first << "] = "
				<< i->second << endl;
		}
		return o;
	}

public:
	ostream& dump(ostream& o) const { return dump(o, ""); }

};	// end class multidimensional_qmap

//=============================================================================
// method definitions

template <class K, class T>
base_multidimensional_qmap<K,T>* 
base_multidimensional_qmap<K,T>::
make_multidimensional_qmap(const size_t d) {
	// slow switch-case, but we need constants
	assert(d > 0 && d <= LIMIT);			// HARD LIMIT
	// there may be some clever way to make a call table to 
	// the various constructors, but this is a rare operation: who cares?
	switch(d) {
		case 1:	return new multidimensional_qmap<1,K,T>();
		case 2:	return new multidimensional_qmap<2,K,T>();
		case 3:	return new multidimensional_qmap<3,K,T>();
		case 4:	return new multidimensional_qmap<4,K,T>();
		// add more cases if LIMIT is ever extended.
		default: return NULL;
	}
}

//-----------------------------------------------------------------------------

//=============================================================================
}	// end namespace MULTIDIMENSIONAL_QMAP_NAMESPACE

#endif	// __MULTIDIMENSIONAL_QMAP_H__

