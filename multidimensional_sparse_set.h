// "multidimensional_sparse_set.h"
// a fixed depth/dimension tree representing sparsely instantiated indices
// David Fang, Cornell University, 2004

#ifndef	__MULTIDIMENSIONAL_SPARSE_SET_H__
#define	__MULTIDIMENSIONAL_SPARSE_SET_H__

#include <iostream>
#include "sstream.h"		// used by "dump" method

#include "discrete_interval_set.h"
// includes <map> and <iostream>

#include "qmap.h"		// queryable maps
#include "ptrs.h"		// for excl_ptr, not copy-constructable
#include "count_ptr.h"		// copy-constructable reference count pointers
#include "sublist.h"		// list slices

#ifndef	MULTIDIMENSIONAL_SPARSE_SET_NAMESPACE
#define	MULTIDIMENSIONAL_SPARSE_SET_NAMESPACE	fang
#endif

/**
	Namespace containing multidimensional-sparse-set classes.  
 */
namespace MULTIDIMENSIONAL_SPARSE_SET_NAMESPACE {
//=============================================================================
using namespace std;
using namespace DISCRETE_INTERVAL_SET_NAMESPACE;
using namespace SUBLIST_NAMESPACE;
using namespace QMAP_NAMESPACE;
using namespace COUNT_PTR_NAMESPACE;
using namespace PTRS_NAMESPACE;

//=============================================================================
// forward declarations
template <class>	class base_multidimensional_sparse_set;
template <size_t, class>	class multidimensional_sparse_set;

//=============================================================================
/**
	Abstract base-class.  
 */
template <class T>
class base_multidimensional_sparse_set {
public:
	typedef	pair<T, T>			range_type;
	/** format for a list of ranges to be added, list is also acceptable */
	typedef	sublist<range_type>		range_arg_type;
public:
	static const size_t				LIMIT = 4;

public:
virtual	~base_multidimensional_sparse_set() { }

virtual	size_t dimensions(void) const = 0;

virtual	bool add_ranges(const range_arg_type& r) = 0;

protected:
virtual	ostream& dump(ostream& o, const string& pre) const = 0;

public:
virtual	ostream& dump(ostream& o) const = 0;

// static functions
	/** virtually, a virtual constructor */
static	base_multidimensional_sparse_set<T>* 
		make_multidimensional_sparse_set(const size_t d);
};	// end class base_multidimensional_sparse_set

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
template <size_t D, class T>
class multidimensional_sparse_set : public base_multidimensional_sparse_set<T> {
friend class multidimensional_sparse_set<D+1,T>;

protected:
	typedef	base_multidimensional_sparse_set<T>	parent;
	typedef	typename parent::range_arg_type		range_arg_type;
	typedef multidimensional_sparse_set<D-1,T>	child_type;
	/** need count_ptr to be copy-constructable */
	typedef	count_ptr<child_type>			map_value_type;
	typedef	qmap<T, map_value_type>			map_type;

protected:
	map_type					index_map;

public:
	/** default empty constructor */
	multidimensional_sparse_set() : parent(), index_map() { }

	/** default copy constructor */
	multidimensional_sparse_set(const multidimensional_sparse_set<D,T>& s) :
		parent(), index_map(s.index_map) { }

	/** default destructor */
	~multidimensional_sparse_set() { }

	size_t dimensions(void) const { return D; }

	bool add_ranges(const range_arg_type& r) {
		assert(r.size() == D);
		bool overlap = false;
		typename range_arg_type::const_iterator i = r.begin();
		range_arg_type sub_range(r);
		sub_range.behead();
		T j = i->first;
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

};	// end class multidimensional_sparse_set

//-----------------------------------------------------------------------------
/**
	Specialization of a one-dimensional array.  
 */
template <class T>
class multidimensional_sparse_set<1,T> :
		public base_multidimensional_sparse_set<T> {
friend class multidimensional_sparse_set<2,T>;

protected:
	typedef	base_multidimensional_sparse_set<T>	parent;
	typedef	typename parent::range_arg_type		range_arg_type;
	typedef	discrete_interval_set<T>		map_type;

protected:
	/**
		Storing with this an interval map for efficiency.  
	 */
	map_type					index_map;

public:
	/** default empty constructor */
	multidimensional_sparse_set() : parent(), index_map() { }

	/** default copy constructor */
	multidimensional_sparse_set(const multidimensional_sparse_set<1,T>& s) :
		parent(), index_map(s.index_map) { }

	/** default destructor */
	~multidimensional_sparse_set() { }

	size_t dimensions(void) const { return 1; }

	bool add_ranges(const range_arg_type& r) {
		assert(r.size() == 1);
		typename range_arg_type::const_iterator i = r.begin();
		return index_map.add_range(i->first, i->second);
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
			T j = i->first;
			for ( ; j <= i->second; j++) {
				o << pre << "[" << j << "]" << endl;
			}
		}
		return o;
	}

public:
	ostream& dump(ostream& o) const { return dump(o, ""); }

};	// end class multidimensional_sparse_set

//=============================================================================
// method definitions

template <class T>
base_multidimensional_sparse_set<T>* 
base_multidimensional_sparse_set<T>::
make_multidimensional_sparse_set(const size_t d) {
	// slow switch-case, but we need constants
	assert(d > 0 && d <= LIMIT);			// HARD LIMIT
	// there may be some clever way to make a call table to 
	// the various constructors, but this is a rare operation: who cares?
	switch(d) {
		case 1:	return new multidimensional_sparse_set<1,T>();
		case 2:	return new multidimensional_sparse_set<2,T>();
		case 3:	return new multidimensional_sparse_set<3,T>();
		case 4:	return new multidimensional_sparse_set<4,T>();
		// add more cases if LIMIT is ever extended.
		default: return NULL;
	}
}

//-----------------------------------------------------------------------------

//=============================================================================
}	// end namespace MULTIDIMENSIONAL_SPARSE_SET_NAMESPACE

#endif	// __MULTIDIMENSIONAL_SPARSE_SET_H__

