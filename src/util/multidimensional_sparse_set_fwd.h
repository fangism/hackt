/**
	\file "multidimensional_sparse_set_fwd.h"
	Forward declarations for a fixed depth/dimension tree-set.
	$Id: multidimensional_sparse_set_fwd.h,v 1.6 2004/12/20 23:21:15 fang Exp $
 */
// David Fang, Cornell University, 2004

#ifndef	__MULTIDIMENSIONAL_SPARSE_SET_FWD_H__
#define	__MULTIDIMENSIONAL_SPARSE_SET_FWD_H__

#include <utility>		// for pair<>
#include "STL/list_fwd.h"

#ifndef	MULTIDIMENSIONAL_SPARSE_SET_NAMESPACE
#define	MULTIDIMENSIONAL_SPARSE_SET_NAMESPACE	util
#endif

#define BASE_MULTIDIMENSIONAL_SPARSE_SET_TEMPLATE_SIGNATURE		\
template <class T, class R, template <class> class L>

#define MULTIDIMENSIONAL_SPARSE_SET_TEMPLATE_SIGNATURE			\
template <size_t D, class T, class R, template <class> class L>


// full description in "multidimensional_sparse_set.h"
namespace MULTIDIMENSIONAL_SPARSE_SET_NAMESPACE {
//=============================================================================
using std::pair;
USING_LIST

//=============================================================================
// forward declarations

// should param R be a template <class, class>?
// or just any class that satisfies the pair interface, 
// not necessarily template?

template <class T = int,
		class R = pair<T,T>, 
		template <class> class L = list >
	class base_multidimensional_sparse_set;

template <size_t, class T = int,
		class R = pair<T,T>,
		template <class> class L = list >
	class multidimensional_sparse_set;

//=============================================================================
}	// end namespace MULTIDIMENSIONAL_SPARSE_SET_NAMESPACE

#endif	// __MULTIDIMENSIONAL_SPARSE_SET_FWD_H__

