/**
	\file "list_vector_fwd.h"
	Forward declaration of list_vector class.

	$Id: list_vector_fwd.h,v 1.1 2004/11/26 23:24:16 fang Exp $
 */

#ifndef	__LIST_VECTOR_FWD__
#define	__LIST_VECTOR_FWD__

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

#endif	// __LIST_VECTOR_FWD__

