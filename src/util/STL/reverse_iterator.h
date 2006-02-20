/**
	\file "util/STL/reverse_iterator.h"
	Extensions to the standard reverse_iterator template operations.  
	Ideally this is not needed, but there are always shortcomings...
	Known to be needed before gcc-4.1 fixed this in its headers.  
	$Id: reverse_iterator.h,v 1.5 2006/02/20 20:51:00 fang Exp $
 */

#ifndef	__UTIL_STL_REVERSE_ITERATOR_H__
#define	__UTIL_STL_REVERSE_ITERATOR_H__

#include "config.h"
#include <iterator>

#if !HAVE_STL_REVERSE_ITERATOR_COMPARISONS
namespace std {

/**
	Equality between two reverse iterators of related type.  
	\pre Iter1 and Iter2 must be compatible underlying iterators,
		e.g. one may be const w.r.t. a common base type.  
 */
template <class Iter1, class Iter2>
inline
bool
operator == (const reverse_iterator<Iter1>& x, 
		const reverse_iterator<Iter2>& y) {
	return x.base() == y.base();
}

/**
	Inequality between two reverse iterators of related type.  
	\pre Iter1 and Iter2 must be compatible underlying iterators,
		e.g. one may be const w.r.t. a common base type.  
 */
template <class Iter1, class Iter2>
inline
bool
operator != (const reverse_iterator<Iter1>& x, 
		const reverse_iterator<Iter2>& y) {
	return !(x.base() == y.base());
//	return x.base() != y.base();
}

// later add relative comparison overloads...
// see <bits/stl_iterator.h> for reference

}	// end namespace std
#endif	// HAVE_STL_REVERSE_ITERATOR_COMPARISONS

#endif	// __UTIL_STL_REVERSE_ITERATOR_H__

