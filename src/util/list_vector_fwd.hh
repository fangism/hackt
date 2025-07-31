/**
	\file "util/list_vector_fwd.hh"
	Forward declaration of list_vector class.

	$Id: list_vector_fwd.hh,v 1.5 2006/10/24 07:27:42 fang Exp $
 */

#ifndef	__UTIL_LIST_VECTOR_FWD_H__
#define	__UTIL_LIST_VECTOR_FWD_H__

#include <vector>

#define LIST_VECTOR_TEMPLATE_SIGNATURE					\
template <class T, class ValAlloc, class VecAlloc>

namespace util {

LIST_VECTOR_TEMPLATE_SIGNATURE
class list_vector;

// re-forward-declaration with default arguments
template <class T, class ValAlloc = std::allocator<T>,
	class VecAlloc = std::allocator<std::vector<T> > >
class list_vector;

}	// end namespace util

#endif	// __UTIL_LIST_VECTOR_FWD_H__

