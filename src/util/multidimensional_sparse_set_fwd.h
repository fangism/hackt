/**
	\file "multidimensional_sparse_set_fwd.h"
	Forward declarations for a fixed depth/dimension tree-set.
	$Id: multidimensional_sparse_set_fwd.h,v 1.6.24.1 2005/02/07 01:11:16 fang Exp $
 */
// David Fang, Cornell University, 2004

#ifndef	__UTIL_MULTIDIMENSIONAL_SPARSE_SET_FWD_H__
#define	__UTIL_MULTIDIMENSIONAL_SPARSE_SET_FWD_H__

#include "STL/pair_fwd.h"
#include "STL/list_fwd.h"

#ifndef	MULTIDIMENSIONAL_SPARSE_SET_NAMESPACE
#define	MULTIDIMENSIONAL_SPARSE_SET_NAMESPACE	util
#endif


#define	WANT_BASE_MULTIDIMENSIONAL_SPARSE_SET		0


#if WANT_BASE_MULTIDIMENSIONAL_SPARSE_SET
#define BASE_MULTIDIMENSIONAL_SPARSE_SET_TEMPLATE_SIGNATURE		\
template <class T, class R, template <class> class L>
#endif

#define MULTIDIMENSIONAL_SPARSE_SET_TEMPLATE_SIGNATURE			\
template <size_t D, class T, class R, template <class> class L>

#define SPECIALIZED_MULTIDIMENSIONAL_SPARSE_SET_TEMPLATE_SIGNATURE	\
template <class T, class R, template <class> class L>

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

#if WANT_BASE_MULTIDIMENSIONAL_SPARSE_SET
template <class T = int,
		class R = pair<T,T>, 
		template <class> class L = list >
	class base_multidimensional_sparse_set;
#endif

template <size_t, class T = int,
		class R = pair<T,T>,
		template <class> class L = list >
	class multidimensional_sparse_set;

//=============================================================================
}	// end namespace MULTIDIMENSIONAL_SPARSE_SET_NAMESPACE

#endif	// __UTIL_MULTIDIMENSIONAL_SPARSE_SET_FWD_H__

