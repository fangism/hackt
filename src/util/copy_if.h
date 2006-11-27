/**
	\file "util/copy_if.h"
	This missing copy_if algorithm from the standard library.  
	$Id: copy_if.h,v 1.1 2006/11/27 08:29:20 fang Exp $
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

//=============================================================================
}	// end namespace util

#endif	// __UTIL_COPY_IF_H__

