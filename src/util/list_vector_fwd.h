/**
	\file "list_vector_fwd.h"
	Forward declaration of list_vector class.

	$Id: list_vector_fwd.h,v 1.1.52.1 2005/03/05 01:26:39 fang Exp $
 */

#ifndef	__UTIL_LIST_VECTOR_FWD_H__
#define	__UTIL_LIST_VECTOR_FWD_H__

#include <memory>		// for std::allocator
#include <vector>

#define LIST_VECTOR_TEMPLATE_SIGNATURE					\
template <class T, class ValAlloc, class VecAlloc>

namespace util {
using std::allocator;
using std::vector;

template <class T, class ValAlloc = allocator<T>,
	class VecAlloc = allocator<vector<T, ValAlloc> > >
class list_vector;

}	// end namespace util

#endif	// __UTIL_LIST_VECTOR_FWD_H__

