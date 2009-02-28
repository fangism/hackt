/**
	\file "util/copy_if.h"
	This missing copy_if, and transform_ig algorithm 
	from the standard library.  
	$Id: copy_if.h,v 1.3 2009/02/28 01:20:47 fang Exp $
 */

#ifndef	__UTIL_COPY_IF_H__
#define	__UTIL_COPY_IF_H__

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

#endif	// __UTIL_COPY_IF_H__

