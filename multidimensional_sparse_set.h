// "multidimensional_sparse_set.h"
// a fixed depth/dimension tree representing sparsely instantiated indices
// David Fang, Cornell University, 2004

#ifndef	__MULTIDIMENSIONAL_SPARSE_SET_H__
#define	__MULTIDIMENSIONAL_SPARSE_SET_H__

#include <iostream>
#include "sstream.h"		// used by "dump" method

#include "multidimensional_sparse_set_fwd.h"		// forward declarations
#include "discrete_interval_set.h"
// includes <map> and <iostream>

#include "qmap.h"		// queryable maps
#include "ptrs.h"		// for excl_ptr, not copy-constructable
#include "count_ptr.h"		// copy-constructable reference count pointers
#include "sublist.h"		// list slices

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
// forward declarations in "multidimensional_sparse_set_fwd.h"

//=============================================================================
/**
	Abstract base-class, for multidimensional sparse set.  
	T is the key type, like an integer.  
	R is the range-type which must be like a pair of integers, 
		and have the same interface as std::pair<T,T>, 
		namely, first and second members.  
	L must be like a list or list-slice with partial list interface,
		such as sublist.  Only need to be able to pop_front, pop_back.  
		Difference lies in whether or not a deep copy is desired. 
 */
template <class T, class R, template <class> class L >
class base_multidimensional_sparse_set {
public:
	typedef	R				range_type;
	/** format for a list of ranges to be added, list is also acceptable */
	typedef	L<range_type>			range_list_type;
	typedef	base_multidimensional_sparse_set<T,R,L>
						this_type;
public:
	static const size_t			LIMIT = 4;

public:
virtual	~base_multidimensional_sparse_set() { }

virtual	size_t dimensions(void) const = 0;

virtual	bool empty(void) const = 0;
virtual	void clear(void) = 0;
virtual range_list_type query_compact_dimensions(
		const range_list_type& r) const = 0;
virtual range_list_type compact_dimensions(void) const = 0;
virtual	bool contains(const range_list_type& r) const = 0;
virtual	bool add_ranges(const range_list_type& r) = 0;
virtual	bool delete_ranges(const range_list_type& r) = 0;

virtual	bool join_sparse_set(const this_type& s) = 0;
virtual	bool meet_sparse_set(const this_type& s) = 0;
virtual	bool subtract_sparse_set(const this_type& s) = 0;
#if 0
	// expensive but sometimes necessary
	deep_copy();
	// to add dimension
	promote();
	// to lose a dimension
	demote();
#endif

protected:
virtual	ostream& dump(ostream& o, const string& pre) const = 0;

public:
virtual	ostream& dump(ostream& o) const = 0;

// static functions
	/** virtually, a virtual constructor */
static	this_type* 
		make_multidimensional_sparse_set(const size_t d);
#if 0
static	this_type* 
		make_multidimensional_sparse_set(const range_list_type& r);
#endif
static	bool	match_range_list(const range_list_type& s,
			const range_list_type& t) {
		assert(s.size() == t.size());
		typename range_list_type::const_iterator si = s.begin();
		typename range_list_type::const_iterator ti = t.begin();
		for ( ; si!=s.end(); si++, ti++) {
			if (si->first != ti->first ||
					si->second != ti->second)
				return false;
			// else range matches, continue
		}
		return true;
	}
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
	Derive this from qmap<>?
	Is 4 enough?

	consider:
	template <size_t D, class T,
		class R = pair<T,T>,	// or pair interface (first, second)
		template <class U> class L = sublist<R>, 
		template <class> class V = count_ptr<child_type>, 
		template <class, class> class map_type = qmap<T, V>
		> class multidimensional_sparse_set;
 */
template <size_t D, class T, class R, template <class> class L>
class multidimensional_sparse_set :
		public base_multidimensional_sparse_set<T, R, L> {
friend class multidimensional_sparse_set<D+1,T,R,L>;

protected:
	typedef	base_multidimensional_sparse_set<T,R,L>	parent;
	typedef	multidimensional_sparse_set<D,T,R,L>	this_type;
	typedef	typename parent::range_list_type		range_list_type;
	typedef multidimensional_sparse_set<D-1,T,R,L>	child_type;
	/** need count_ptr to be copy-constructable */
	typedef	count_ptr<child_type>			map_value_type;
	typedef	qmap<T, map_value_type>			map_type;

protected:
	map_type					index_map;

public:
	/** default empty constructor */
	multidimensional_sparse_set() : parent(), index_map() { }

	/** default copy constructor */
	multidimensional_sparse_set(
		const multidimensional_sparse_set<D,T,R,L>& s) :
		parent(), index_map(s.index_map) { }

	/** default destructor */
	~multidimensional_sparse_set() { }

	size_t dimensions(void) const { return D; }

	/** iteratively check empty maps, purge empty entries? */
	bool empty(void) const {
		return index_map.empty();
	}

	/** clears entire set */
	void clear(void) {
		index_map.clear();
	}

	/**
		Queries whether this set contains the entire range
		specified by the argument.  
		\param r may be under-specified list of dimensions, 
			i.e. can query with lesser dimension range list.  
	 */
	bool contains(const range_list_type& r) const {
		assert(r.size() <= D);
		if (r.empty()) return true;
		range_list_type sub_range(r);
		sub_range.pop_front();
		const T min = r.begin()->first;
		const T max = r.begin()->second;
		T j = min;
		for ( ; j<=max; j++) {
			const map_value_type probe = index_map[j];
			if (probe) {
				if (!probe->contains(sub_range))
					return false;
			} else {
				return false;
			}
		}
		// if this point reached, all recursive calls returned true
		return true;
	}

	/**
		Consider templating to allow any standard container
		of intervals, instead of restricting to list<pair<T> >.  
			template <template class C<class>, class P>
				bool add_ranges(const C<P>& r);
			class P has pair interface.  
		\return true if there was any overlap.
	 */
	bool add_ranges(const range_list_type& r) {
		assert(r.size() == D);
		bool overlap = false;
		typename range_list_type::const_iterator i = r.begin();
		range_list_type sub_range(r);
		sub_range.pop_front();
		T j = i->first;
		for ( ; j <= i->second; j++) {
			// non-modifying probe first
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

	/**
		Removes the indices specified by the argument from the set.  
		\return true if anything was deleted.  
	 */
	bool delete_ranges(const range_list_type& r) {
		assert(r.size() <= D);
		bool overlap = false;
		typename range_list_type::const_iterator i = r.begin();
		range_list_type sub_range(r);
		sub_range.pop_front();
		T j = i->first;
		for ( ; j <= i->second; j++) {
			// non-modifying probe first
			const map_value_type probe = 
				static_cast<const map_type&>(index_map)[j];
			if (probe) {
				map_value_type sub = index_map[j];
				assert(sub);
				if (sub->delete_ranges(sub_range))
					overlap = true;
				if (sub->empty())
					index_map.erase(j);
			}
		}
		return overlap;
	}

	/**
		Filters this set as the intersection with the argument.  
		Untested.
		\return true if anything was deleted as a result. 
	 */
	bool meet(const this_type& s) {
		// at this dimension, find indices common to both maps
		bool overlap = false;
		typename map_type::iterator i = index_map.begin();
		for ( ; i!=index_map.end(); i++) {
			// recall map_value_type is pointer-class
				// needs to be non-modifying query
			const map_value_type probe = s.index_map[i->first];
			if (probe) {
				// recursive meet
				// now can use modifing lookup
				if (i->second->meet(*probe)) {
					overlap = true;
				}
			}
		}
		return overlap;
	}

	/** alias for meet(). */
	bool intersect(const this_type& s) { return meet(s); }

	/**
		Merges argument into this set as union.  
		Should make deep copies or pointer copies?
		Or let user decide in template argument?  Yeah...
		\return true if anything was added as a result.  
	 */
	bool join(const this_type& s) {
		bool overlap = false;
		typename map_type::const_iterator s_i = s.index_map.begin();
		for ( ; s_i!=s.index_map.end(); s_i++) {
			// recall map_value_type is pointer-class
			// modifying lookup is ok
			map_value_type probe = index_map[s_i->first];
			if (probe) {
				// recursive join subtree
				// now can use modifing lookup
				if (probe->join(*s_i->second)) {
					overlap = true;
				}
			} else {
				// add entire subtree
				index_map[s_i->first] = s_i->second;
				overlap = true;
			}
		}
		return overlap;
	}

	/** alias for join(). */
	bool unite(const this_type& s) { return join(s); }

	/**
		Subtracts the elements from the argument from this set.  
		\return true if anything was actually removed.  
	 */
	bool subtract(const this_type& s) {
		bool overlap = false;
		typename map_type::const_iterator i = s.index_map.begin();
		for ( ; i!=s.index_map.end(); i++) {
			// recall map_value_type is pointer-class
				// needs to be non-modifying query
			const map_value_type probe = 
				static_cast<const map_type&>(index_map)
					[i->first];
			if (probe) {
				// recursive meet
				// now can use modifing lookup
				if (probe->subtract(*(i->second))) {
					overlap = true;
					// clean-up
					if (probe->empty())
						index_map.erase(i->first);
				}
			}
		}
		return overlap;
	}

	/**
		Join operator with run-type type-check.  
	 */
	bool join_sparse_set(const parent& s) {
		const this_type* t = dynamic_cast<const this_type*>(&s);
		assert(t);
		return join(*t);
	}

	/**
		Meet operator with run-type type-check.  
	 */
	bool meet_sparse_set(const parent& s) {
		const this_type* t = dynamic_cast<const this_type*>(&s);
		assert(t);
		return meet(*t);
	}

	/**
		Subtract operator with run-type type-check.  
	 */
	bool subtract_sparse_set(const parent& s) {
		const this_type* t = dynamic_cast<const this_type*>(&s);
		assert(t);
		return subtract(*t);
	}

	// write a generic walker for this bad boy?

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

	/**
		Bottom-up tries to construct a packed dense array
		index representation of the entire tree-map collection.  
		If collection is not "dense" returns empty list.  
	 */
	range_list_type compact_dimensions(void) const {
		if (index_map.empty())
			return range_list_type();
		// check if this dimension is dense
		typename map_type::const_iterator i = index_map.begin();
		const typename map_type::const_reverse_iterator e =
			index_map.rbegin();
		const T min = i->first;
		const T max = e->first;
		const T diff = index_map.size() -1;
		if (max -min != diff)
			return range_list_type();	// not dense
		// else if sub-dimensions match
		range_list_type s = i->second->compact_dimensions();
		// should empty case be acceptable? no, depends on return value
		if (s.empty())
			return range_list_type();
		// match against s
		for (i++; i!=index_map.end(); i++) {
			range_list_type t = i->second->compact_dimensions();
			if (t.empty())
				return range_list_type();
			else if (!match_range_list(s,t))
				return range_list_type();
			// else continue checking...
		}
		// if this point is reached, all subdimensions match
		// prepend with this dimension's range, and return it.  
		typename parent::range_type r(min, max);
		s.push_front(r);
		return s;
	}

	/**
		Determines whether or not the sub-array indexed
		is dense/compact.  
		\param r the multidimensional range list to check.  
		\return a multidimensional range list representing
			the dense sub-array, if it is indeed dense, 
			otherwise just returns an empty list.  
	 */
	range_list_type query_compact_dimensions(
			const range_list_type& r) const {
		assert(r.size() <= D);
		if (r.empty())
			return compact_dimensions();
		// else proceed
		range_list_type sub(r);
		typename range_list_type::const_iterator t = sub.begin();
		const T min = t->first;
		const T max = t->second;
		assert(min <= max);
		sub.pop_front();
		T i = min;
		map_value_type probe =
			static_cast<const map_type&>(index_map)[i];
		if (!probe)	// referencing un-instantiated index
			return range_list_type();
		range_list_type s = probe->query_compact_dimensions(sub);
		if (s.empty())
			return range_list_type();
		for (i++; i<=max; i++) {
			map_value_type probe =	// shadows
				static_cast<const map_type&>(index_map)[i];
			if (!probe)	// referencing un-instantiated index
				return range_list_type();
			range_list_type t =
				probe->query_compact_dimensions(sub);
			if (t.empty())
				return range_list_type();
			else if (!match_range_list(s,t))
				return range_list_type();
			// else continue checking...
		}
		// if this point is reached, all subdimensions match
		// prepend with this dimension's range, and return it.  
		typename parent::range_type nr(min, max);
		s.push_front(nr);
		return s;
	}

};	// end class multidimensional_sparse_set

//-----------------------------------------------------------------------------
/**
	Specialization of a one-dimensional array.  
 */
template <class T, class R, template <class> class L>
class multidimensional_sparse_set<1,T,R,L> :
		public base_multidimensional_sparse_set<T,R,L> {
friend class multidimensional_sparse_set<2,T,R,L>;

protected:
	typedef	base_multidimensional_sparse_set<T,R,L>	parent;
	typedef	multidimensional_sparse_set<1,T,R,L>	this_type;
	typedef	typename parent::range_list_type	range_list_type;
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
	multidimensional_sparse_set(
		const multidimensional_sparse_set<1,T,R,L>& s) :
		parent(), index_map(s.index_map) { }

	/** default destructor */
	~multidimensional_sparse_set() { }

	size_t dimensions(void) const { return 1; }

	bool empty(void) const {
		return index_map.empty();
	}

	/** clears entire set */
	void clear(void) {
		index_map.clear();
	}

	bool add_ranges(const range_list_type& r) {
		assert(r.size() == 1);
		typename range_list_type::const_iterator i = r.begin();
		return index_map.add_range(i->first, i->second);
	}

	/**
		Queries whether this set contains the entire range
		specified by the argument.  
		\param r may be under-specified list of dimensions, 
			i.e. can query with lesser dimension range list.  
	 */
	bool contains(const range_list_type& r) const {
		assert(r.size() <= 1);
		if (r.empty()) return true;
		T min = r.begin()->first;
		T max = r.begin()->second;
		typename map_type::const_iterator ret =
			index_map.contains_entirely(min, max);
		return (ret != index_map.end());
	}

	bool delete_ranges(const range_list_type& r) {
		assert(r.size() <= 1);
		if (r.empty()) return false;
		typename range_list_type::const_iterator i = r.begin();
		return index_map.delete_range(i->first, i->second);
	}

	bool join(const this_type& s) {
		return index_map.join(s.index_map);
	}

	bool meet(const this_type& s) {
		return index_map.meet(s.index_map);
	}

	bool subtract(const this_type& s) {
		return index_map.subtract(s.index_map);
	}

	/**
		Join operator with run-type type-check.  
	 */
	bool join_sparse_set(const parent& s) {
		const this_type* t = dynamic_cast<const this_type*>(&s);
		assert(t);
		return join(*t);
	}

	/**
		Meet operator with run-type type-check.  
	 */
	bool meet_sparse_set(const parent& s) {
		const this_type* t = dynamic_cast<const this_type*>(&s);
		assert(t);
		return meet(*t);
	}

	/**
		Subtract operator with run-type type-check.  
	 */
	bool subtract_sparse_set(const parent& s) {
		const this_type* t = dynamic_cast<const this_type*>(&s);
		assert(t);
		return subtract(*t);
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

	/**
		1-D sparse set is dense if there is only one element
		in the discrete_interval_set, that represents the 
		entire set. 
		\return single range in list if this is indeed dense, 
			otherwise, returns an empty range list. 
	 */
	range_list_type compact_dimensions(void) const {
		range_list_type ret;
		if (index_map.size() == 1) {
			const typename map_type::const_iterator b =
				index_map.begin();
			typename parent::range_type r(b->first, b->second);
			ret.push_back(r);
		}
		return ret;
	}

	/**
		Returns the queried range if it is contained entirely
		in the 1D sparse set.  
	 */
	range_list_type query_compact_dimensions(
			const range_list_type& r) const {
		assert(r.size() <= 1);
		range_list_type ret;
		if (!r.empty()) {
			// only one element
			typename range_list_type::const_iterator f = r.begin();
			const T min = f->first;
			const T max = f->second;
			if (index_map.contains_entirely(min, max) !=
					index_map.end()) {
				typename parent::range_type r(min, max);
				ret.push_back(r);
			}
		}
		return ret;
	}

};	// end class multidimensional_sparse_set

//=============================================================================
// method definitions

template <class T, class R, template <class> class L>
base_multidimensional_sparse_set<T,R,L>* 
base_multidimensional_sparse_set<T,R,L>::
make_multidimensional_sparse_set(const size_t d) {
	// slow switch-case, but we need constants
	assert(d > 0 && d <= LIMIT);			// HARD LIMIT
	// there may be some clever way to make a call table to 
	// the various constructors, but this is a rare operation: who cares?
	switch(d) {
		case 1:	return new multidimensional_sparse_set<1,T,R,L>();
		case 2:	return new multidimensional_sparse_set<2,T,R,L>();
		case 3:	return new multidimensional_sparse_set<3,T,R,L>();
		case 4:	return new multidimensional_sparse_set<4,T,R,L>();
		// add more cases if LIMIT is ever extended.
		default: return NULL;
	}
}

//-----------------------------------------------------------------------------

//=============================================================================
}	// end namespace MULTIDIMENSIONAL_SPARSE_SET_NAMESPACE

#endif	// __MULTIDIMENSIONAL_SPARSE_SET_H__

