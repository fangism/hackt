/**
	\file "util/multidimensional_sparse_set.h"
	Fixed depth/dimension tree representing sparsely instantiated indices.
	$Id: multidimensional_sparse_set.h,v 1.8 2005/05/10 04:51:26 fang Exp $
 */
// David Fang, Cornell University, 2004

#ifndef	__UTIL_MULTIDIMENSIONAL_SPARSE_SET_H__
#define	__UTIL_MULTIDIMENSIONAL_SPARSE_SET_H__

#include <iosfwd>
#include "util/macros.h"

#include "util/array_traits.h"
#include "util/multidimensional_sparse_set_fwd.h"	// forward declarations
#include "util/discrete_interval_set.h"
// includes <map> and <iostream>

#include "util/qmap.h"		// queryable maps
#include "util/memory/count_ptr.h"

/**
	Namespace containing multidimensional-sparse-set classes.  
 */
namespace util {
//=============================================================================
using std::ostream;
using std::string;
using util::discrete_interval_set;
using util::qmap;
using namespace util::memory;

//=============================================================================
// forward declarations in "multidimensional_sparse_set_fwd.h"

SPECIALIZED_MULTIDIMENSIONAL_SPARSE_SET_TEMPLATE_SIGNATURE
struct multidimensional_sparse_set_traits {
	typedef	R				range_type;
	/** format for a list of ranges to be added, list is also acceptable */
	typedef	L<range_type>			range_list_type;

	static
	bool
	match_range_list(const range_list_type& s, const range_list_type& t);
};	// end struct multidimensional_sparse_set_traits

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

	\param T is the key type, like an integer.  
	\param R is the range-type which must be like a pair of integers, 
		and have the same interface as std::pair<T,T>, 
		namely, first and second members.  
	\param L must be like a list or list-slice with partial list interface,
		such as sublist.  Only need to be able to pop_front, pop_back.  
		Difference lies in whether or not a deep copy is desired. 
	\todo employ nested_iterators.... oooooh!
 */
MULTIDIMENSIONAL_SPARSE_SET_TEMPLATE_SIGNATURE
class multidimensional_sparse_set {
friend class multidimensional_sparse_set<D+1,T,R,L>;

protected:
	typedef	multidimensional_sparse_set<D,T,R,L>	this_type;
	typedef multidimensional_sparse_set<D-1,T,R,L>	child_type;
	/** need count_ptr to be copy-constructable */
	typedef	count_ptr<child_type>			map_value_type;
	typedef	qmap<T, map_value_type>			map_type;
	typedef	multidimensional_sparse_set_traits<T,R,L>	traits_type;
public:
	typedef	typename traits_type::range_type	range_type;
	typedef	typename traits_type::range_list_type	range_list_type;

public:
	// for array_traits<>
	enum { dim = D };

protected:
	map_type					index_map;

public:
	/** default empty constructor */
	multidimensional_sparse_set();

	/** default copy constructor */
	multidimensional_sparse_set(
		const multidimensional_sparse_set<D,T,R,L>& s);

	/** default destructor */
	~multidimensional_sparse_set();

	size_t
	dimensions(void) const { return D; }

	/** iteratively check empty maps, purge empty entries? */
	bool
	empty(void) const { return index_map.empty(); }

	/** clears entire set */
	void
	clear(void);

	/**
		Queries whether this set contains the entire range
		specified by the argument.  
		\param r may be under-specified list of dimensions, 
			i.e. can query with lesser dimension range list.  
	 */
	bool
	contains(const range_list_type& r) const;

	/**
		Consider templating to allow any standard container
		of intervals, instead of restricting to list<pair<T> >.  
			template <template class C<class>, class P>
				bool add_ranges(const C<P>& r);
			class P has pair interface.  
		\return true if there was any overlap.
	 */
	bool
	add_ranges(const range_list_type& r);

	/**
		Removes the indices specified by the argument from the set.  
		\return true if anything was deleted.  
	 */
	bool
	delete_ranges(const range_list_type& r);

	/**
		Filters this set as the intersection with the argument.  
		Untested.
		\return true if anything was deleted as a result. 
	 */
	bool
	meet(const this_type& s);

	/** alias for meet(). */
	bool
	intersect(const this_type& s) { return meet(s); }

	/**
		Merges argument into this set as union.  
		Should make deep copies or pointer copies?
		Or let user decide in template argument?  Yeah...
		\return true if anything was added as a result.  
	 */
	bool
	join(const this_type& s);

	/** alias for join(). */
	bool
	unite(const this_type& s) { return join(s); }

	/**
		Subtracts the elements from the argument from this set.  
		\return true if anything was actually removed.  
	 */
	bool
	subtract(const this_type& s);

	// write a generic walker for this bad boy?

protected:
	/**
		Prints out list of all members recursively.  
		\param o the output stream.  
		\param pre is a prefix string passed by the caller.  
		\return the output stream.
	 */
	ostream&
	dump(ostream& o, const string& pre) const; 

public:
	ostream&
	dump(ostream& o) const { return dump(o, ""); }

	/**
		Bottom-up tries to construct a packed dense array
		index representation of the entire tree-map collection.  
		If collection is not "dense" returns empty list.  
		\return valid range list for compact bounds or empty.
	 */
	range_list_type
	compact_dimensions(void) const;

	/**
		Determines whether or not the sub-array indexed
		is dense/compact.  
		\param r the multidimensional range list to check.  
		\return a multidimensional range list representing
			the dense sub-array, if it is indeed dense, 
			otherwise just returns an empty list.  
	 */
	range_list_type
	query_compact_dimensions(const range_list_type& r) const;

};	// end class multidimensional_sparse_set

//-----------------------------------------------------------------------------
/**
	Specialization of a one-dimensional array.  
 */
SPECIALIZED_MULTIDIMENSIONAL_SPARSE_SET_TEMPLATE_SIGNATURE
class multidimensional_sparse_set<1,T,R,L> {
friend class multidimensional_sparse_set<2,T,R,L>;

protected:
	typedef	multidimensional_sparse_set<1,T,R,L>	this_type;
	typedef	discrete_interval_set<T>		map_type;
	typedef	multidimensional_sparse_set_traits<T,R,L>	traits_type;
public:
	typedef	typename traits_type::range_type	range_type;
	typedef	typename traits_type::range_list_type	range_list_type;

public:
	// for array_traits<>
	enum { dim = 1 };

protected:
	/**
		Storing with this an interval map for efficiency.  
	 */
	map_type					index_map;

public:
	/** default empty constructor */
	multidimensional_sparse_set();

	/** default copy constructor */
	multidimensional_sparse_set(
		const multidimensional_sparse_set<1,T,R,L>& s);

	/** default destructor */
	~multidimensional_sparse_set();

	size_t
	dimensions(void) const { return 1; }

	bool
	empty(void) const { return index_map.empty(); }

	/** clears entire set */
	void
	clear(void); 

	bool
	add_ranges(const range_list_type& r);

	/**
		Queries whether this set contains the entire range
		specified by the argument.  
		\param r may be under-specified list of dimensions, 
			i.e. can query with lesser dimension range list.  
	 */
	bool
	contains(const range_list_type& r) const;

	bool
	delete_ranges(const range_list_type& r);

	bool join(const this_type& s) {
		return index_map.join(s.index_map);
	}

	bool meet(const this_type& s) {
		return index_map.meet(s.index_map);
	}

	bool subtract(const this_type& s) {
		return index_map.subtract(s.index_map);
	}

protected:
	/**
		Prints out list of all members recursively.  
		\param o the output stream.  
		\param pre is a prefix string passed by the caller.  
		\return the output stream.
	 */
	ostream&
	dump(ostream& o, const string& pre) const;

public:
	ostream&
	dump(ostream& o) const { return dump(o, ""); }

	/**
		1-D sparse set is dense if there is only one element
		in the discrete_interval_set, that represents the 
		entire set. 
		\return single range in list if this is indeed dense, 
			otherwise, returns an empty range list. 
	 */
	range_list_type
	compact_dimensions(void) const;

	/**
		Returns the queried range if it is contained entirely
		in the 1D sparse set.  
	 */
	range_list_type
	query_compact_dimensions(const range_list_type& r) const;

};	// end class multidimensional_sparse_set

//=============================================================================
}	// end namespace util

namespace util {

MULTIDIMENSIONAL_SPARSE_SET_TEMPLATE_SIGNATURE
struct array_traits<util::multidimensional_sparse_set<D,T,R,L> > {
	typedef	util::multidimensional_sparse_set<D,T,R,L>	array_type;
	enum { dimensions = array_type::dim };
};	// end struct array_traits

}	// end namespace util

#endif	// __UTIL_MULTIDIMENSIONAL_SPARSE_SET_H__

