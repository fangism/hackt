/**
	\file "util/STL/map.h"
	Queryable map with non-modifying (const) lookup method.  
	Non-modifying lookup guarantees that no extraneous empty
	elements are added by lookup.  
	$Id: map.h,v 1.2 2005/05/10 04:51:32 fang Exp $
 */

#ifndef	__UTIL_STL_MAP_H__
#define	__UTIL_STL_MAP_H__

#include "util/STL/map_fwd.h"

#if !USE_STD_MAP
#include <map>

/**
	Experimenting around with code generated: 
	containership vs. inheritance.  
 */
#define	MAP_INHERITANCE			1

#define	MAP_INLINE_LEVEL		0

#if MAP_INHERITANCE
	#define	MAP_PARENT		map_type::
#else
	#define	MAP_PARENT		the_map.
#endif

namespace util {
namespace STL {
using std::pair;

//-----------------------------------------------------------------------------
/**
	Extension of Standard Template Library's map container.  
	Adds an lookup operator with constant semantics for querying
	whether or not something is in the map, but without modifying it.  
	Useful for maps of pointers and pointer classes.  
 */
STL_MAP_TEMPLATE_SIGNATURE
class map
#if MAP_INHERITANCE
: private std::map<K,T,C,A>
#endif
{
private:
	typedef	std::map<K,T,C,A>			map_type;
#if !MAP_INHERITANCE
	map_type					the_map;
#endif
public:
        typedef typename map_type::key_type		key_type;
        typedef typename map_type::mapped_type		mapped_type;   
        typedef typename map_type::value_type		value_type;
        typedef typename map_type::key_compare		key_compare;
        typedef typename map_type::value_compare	value_compare;
	typedef typename map_type::allocator_type	allocator_type;

	/// this is not T&, this depends on implementation
	typedef typename map_type::reference		reference;
	/// this is not const T&, this depends on implementation
	typedef typename map_type::const_reference	const_reference;

	typedef typename map_type::iterator		iterator;
	typedef typename map_type::const_iterator	const_iterator;
	typedef typename map_type::reverse_iterator	reverse_iterator;
	typedef typename map_type::const_reverse_iterator
							const_reverse_iterator; 
	typedef typename map_type::size_type		size_type;
	typedef typename map_type::difference_type	difference_type;
	typedef typename map_type::pointer		pointer;
	typedef typename map_type::const_pointer	const_pointer;
	typedef typename map_type::allocator_type	allocator_type;

public:
	/** Default constructor. */
	map();

	explicit
	map(const key_compare& __comp,
		const allocator_type& __a = allocator_type());

	/** Psuedo copy-constructor, accepting the base type */
	map(const map_type& __x);

	template <typename _InputIterator>
	map(_InputIterator __first, _InputIterator __last);

	template <typename _InputIterator>
	map(_InputIterator __first, _InputIterator __last,
		const key_compare& __comp,
		const allocator_type& __a = allocator_type());

	/** Default destructor. */
	~map();

	/// note: accept and return base type of map
	map_type&
	operator = (const map_type& __x);

	allocator_type
	get_allocator(void) const { return MAP_PARENT get_allocator(); }

	key_compare
	key_comp(void) const { return MAP_PARENT key_comp(); }

	value_compare
	value_comp(void) const { return MAP_PARENT value_comp(); }

	iterator
	begin(void) { return MAP_PARENT begin(); }

	iterator
	end(void) { return MAP_PARENT end(); }

	const_iterator
	begin(void) const { return MAP_PARENT begin(); }

	const_iterator
	end(void) const { return MAP_PARENT end(); }

	reverse_iterator
	rbegin(void) { return MAP_PARENT rbegin(); }

	reverse_iterator
	rend(void) { return MAP_PARENT rend(); }

	const_reverse_iterator
	rbegin(void) const { return MAP_PARENT rbegin(); }

	const_reverse_iterator
	rend(void) const { return MAP_PARENT rend(); }

	bool
	empty(void) const { return MAP_PARENT empty(); }

	size_type
	size(void) const { return MAP_PARENT size(); }

	size_type
	max_size(void) const { return MAP_PARENT max_size(); }

	mapped_type&
	operator [] (const key_type& k);

	pair<iterator,bool>
	insert(const value_type& __x);

	iterator
	insert(iterator position, const value_type& __x);

	template <typename _InputIterator>
	void
	insert(_InputIterator __first, _InputIterator __last);

	void
	erase(iterator __position);

	size_type
	erase(const key_type& __x);

	void
	erase(iterator __first, iterator __last);

	void
	swap(map_type& __x);

#if !LIST_INHERITANCE
	void
	swap(map& __x);
#endif

	void
	clear(void);

	iterator
	find(const key_type& __x);

	const_iterator
	find(const key_type& __x) const;

	size_type
	count(const key_type& __x) const;

	iterator
	lower_bound(const key_type& __x);

	const_iterator
	lower_bound(const key_type& __x) const;

	iterator
	upper_bound(const key_type& __x);

	const_iterator
	upper_bound(const key_type& __x) const;

	pair<iterator,iterator>
	equal_range(const key_type& __x);

	pair<const_iterator,const_iterator>
	equal_range(const key_type& __x) const;

#if !MAP_INHERITANCE
	template <typename _K1, typename _T1, typename _C1, typename _A1>
	friend
	bool
	operator == (const map<_K1,_T1,_C1,_A1>&,
		const map<_K1,_T1,_C1,_A1>&);

	template <typename _K1, typename _T1, typename _C1, typename _A1>
	friend
	bool
	operator < (const map<_K1,_T1,_C1,_A1>&,
		const map<_K1,_T1,_C1,_A1>&);
#endif

};	// end class map

//-----------------------------------------------------------------------------
}	// end namespace STL
}	// end namespace util

#endif	// !USE_STD_MAP

#endif	//	__UTIL_STL_MAP_H__

