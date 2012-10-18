/**
	\file "util/copy_if.hh"
	This missing copy_if, and transform_if algorithm 
	from the standard library.  
	$Id: copy_if.hh,v 1.4 2009/04/29 05:33:46 fang Exp $
 */

#ifndef	__UTIL_COPY_IF_H__
#define	__UTIL_COPY_IF_H__

#include "config.h"
#ifdef	HAVE_STL_COPY_IF
#include <algorithm>
#define	USING_COPY_IF		using std::copy_if;
#else
#define	USING_COPY_IF		using util::copy_if;
namespace util {
//=============================================================================
// declaration
template <class In, class Out, class Pred>
Out
copy_if(In, In, Out, Pred);

/**
	The copy_if algorithm copies an element to the output iterator
	if the predicate is satisfied.  
 */
template <class In, class Out, class Pred>
Out
copy_if(In first, In last, Out res, Pred p) {
	while (first != last) {
		if (p(*first)) {
			*res++ = *first;
		}
		++first;
	}
	return res;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The transform_if algorithm conditionally writes to the output
	iterator, but unconditionally advances the output iterator.
	This is extremely useful for copy-filtering with transforming.
 */
template<class In, class Out, class Pred, class Op>
Out
transform_if(In first, In last, Out res, Pred p, Op op) {
	while (first != last) {
		if (p(*first)) {
			*res = op(*first);
		}
		++first;
		++res;
	}
	return res;
}

/**
	Use a binary predicate and binary functor.
 */
template<class In, class In2, class Out, class Pred, class Op>
Out
transform_if(In first, In last, In2 first2, Out res, Pred p, Op op) {
	while (first != last) {
		if (p(*first, *first2)) {
			*res = op(*first, *first2);
		}
		++first;
		++first2;
		++res;
	}
	return res;
}

//=============================================================================
}	// end namespace util

#endif	// HAVE_STL_COPY_IF
#endif	// __UTIL_COPY_IF_H__

