/**
	\file "util/list_vector_fwd.h"
	Forward declaration of list_vector class.

	$Id: list_vector_fwd.h,v 1.3.54.1 2006/01/18 06:25:05 fang Exp $
 */

#ifndef	__UTIL_LIST_VECTOR_FWD_H__
#define	__UTIL_LIST_VECTOR_FWD_H__

#if 0
#include <memory>		// for std::allocator
#include <vector>
#else
#include "util/STL/allocator_fwd.h"
#include "util/STL/vector_fwd.h"
#endif

#define LIST_VECTOR_TEMPLATE_SIGNATURE					\
template <class T, class ValAlloc, class VecAlloc>

namespace util {
using std::allocator;
using std::vector;
using std::default_vector;

LIST_VECTOR_TEMPLATE_SIGNATURE
class list_vector;

// re-forward-declaration with default arguments
template <class T, class ValAlloc = std::allocator<T>,
	class VecAlloc = std::allocator<typename default_vector<T>::type > >
class list_vector;

}	// end namespace util

#endif	// __UTIL_LIST_VECTOR_FWD_H__

