/**
	\file "discrete_interval_set.h"
	Class for a set of intervals over the set of integers
	or discrete, integer-like values.  
	This class is used to build the "multidimensional_sparse_set" class.  
	$Id: discrete_interval_set.h,v 1.6.28.1 2005/02/15 07:32:06 fang Exp $
 */

// by David Fang, Cornell University, 2004

#ifndef	__UTIL_DISCRETE_INTERVAL_SET_H__
#define	__UTIL_DISCRETE_INTERVAL_SET_H__

#include "discrete_interval_set_fwd.h"

#include <map>
#include <iosfwd>

//=============================================================================
namespace util {
using std::map;		// for map<>
using std::ostream;

//=============================================================================
/**
	Template parameter class T must be discrete, such as integers, 
	and not continuous such as reals, floats, or doubles.  
	Because differences of 1 will be fused.  
	Implemented as a map of pairs.  
	Pairs are automatically fused together as holes are filled.  

	We choose a map for efficient comparison (<, >) operations.  
	The first element of each pair is a minimum of an inclusive
	bracket, and the second element is the maximum, inclusive.
 */
DISCRETE_INTERVAL_SET_TEMPLATE_SIGNATURE
class discrete_interval_set {
private:
	typedef	map<T, T>				parent_type;
	parent_type					parent;
protected:
	typedef	typename parent_type::iterator		iterator;
	typedef	typename parent_type::reverse_iterator	reverse_iterator;

public:
	typedef	typename parent_type::const_iterator	const_iterator;
	typedef	typename parent_type::const_reverse_iterator
							const_reverse_iterator;
	typedef	typename parent_type::size_type		size_type;

public:
	/** Standard default constructor. */
	discrete_interval_set();

	/** Constructor initializing with one interval. */
	discrete_interval_set(const T a, const T b);

	/** Standard copy constructor. */
	discrete_interval_set(const discrete_interval_set<T>& s);

	/** Standard default destructor. */
	~discrete_interval_set();

protected:
	void
	check_integrity(void) const;

	iterator
	contains(const T v);

public:
	const_iterator
	begin(void) const { return parent.begin(); }

	const_iterator
	end(void) const { return parent.end(); }

	size_type
	size(void) const { return parent.size(); }

	bool
	empty(void) const { return parent.empty(); }

	void
	clear(void);

	/**
		Public const version of contains().  
	 */
	const_iterator
	contains(const T v) const;

	/**
		Returns the interval that entirely contains [min,max], 
		else returns the end iterator.  
	 */
	const_iterator
	contains_entirely(const T min, const T max) const;

	/**
		Queries whether or not proposed range will overlap
		at all with existing intervals.  
	 */
	bool
	query_overlap(const T min, const T max) const;

	/**
		Adds an interval to the current set.
		Intelligently fuses ranges together appropriately.  
		\param min the lower bound, inclusive.  
		\param max the upper bound, inclusive.  
		\return true if there was overlap, else false.
	 */
	bool
	add_range(const T min, const T max);

	/**
		Removes a range from the set.
		\param min the lower bound, inclusive.  
		\param max the upper bound, inclusive.  
		\return if anything was actually deleted.
	 */
	bool
	delete_range(const T min, const T max);

	/**
		Removes the range (-INF, m].
		\param m the lower cut-off.
		\return true if anything was deleted.  
	 */
	bool
	behead(const T m);

	/**
		Removes the range [m, +INF).
		\param m the upper cut-off.
		\return true if anything was deleted.  
	 */
	bool
	betail(const T m);

	/**
		Takes intersection.  
		\param b the list with which to take the intersection. 
		\return true if anything was deleted.  
	 */
	bool
	meet(const discrete_interval_set<T>& b);

	bool
	intersect(const discrete_interval_set<T>& b) { return meet(b); }

	/**
		Takes union.  
		\param b the list with which to take the union. 
		\return true if anything was added.  
	 */
	bool
	join(const discrete_interval_set<T>& b);

	bool
	unite(const discrete_interval_set<T>& b) { return join(b); }

	/**
		Subtracts argument's set from this set.  
		\return true if anything was actually deleted.  
	 */
	bool
	subtract(const discrete_interval_set<T>& b);

	// non-member functions
	template <class U>
	friend
	ostream&
	operator << (ostream& o, const discrete_interval_set<U>& r);

};	// end class discrete_interval_set

//=============================================================================
}	// end namespace util

#endif	// __UTIL_DISCRETE_INTERVAL_SET_H__

