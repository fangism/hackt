/**
	\file "multidimensional_sparse_set_fwd.h"
	Forward declarations for a fixed depth/dimension tree-set.
	$Id: multidimensional_sparse_set_fwd.h,v 1.4 2004/11/02 07:52:15 fang Exp $
 */
// David Fang, Cornell University, 2004

#ifndef	__MULTIDIMENSIONAL_SPARSE_SET_FWD_H__
#define	__MULTIDIMENSIONAL_SPARSE_SET_FWD_H__

#include <utility>		// for pair<>
#include <list>

#ifndef	MULTIDIMENSIONAL_SPARSE_SET_NAMESPACE
#define	MULTIDIMENSIONAL_SPARSE_SET_NAMESPACE	multidim_sparse_set_ns
#endif

// full description in "multidimensional_sparse_set.h"
namespace MULTIDIMENSIONAL_SPARSE_SET_NAMESPACE {
//=============================================================================
using namespace std;

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

