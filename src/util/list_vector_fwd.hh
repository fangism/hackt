/**
	\file "util/list_vector_fwd.hh"
	Forward declaration of list_vector class.

	$Id: list_vector_fwd.hh,v 1.5 2006/10/24 07:27:42 fang Exp $
 */

#ifndef	__UTIL_LIST_VECTOR_FWD_H__
#define	__UTIL_LIST_VECTOR_FWD_H__

#include "util/STL/allocator_fwd.hh"
#include "util/STL/vector_fwd.hh"

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

