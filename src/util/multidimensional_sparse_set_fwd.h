// "multidimensional_sparse_set_fwd.h"
// forward declarations only
// a fixed depth/dimension tree representing sparsely instantiated indices
// David Fang, Cornell University, 2004

#ifndef	__MULTIDIMENSIONAL_SPARSE_SET_FWD_H__
#define	__MULTIDIMENSIONAL_SPARSE_SET_FWD_H__

#include <map>		// for pair<>
#include <list>

#ifndef	MULTIDIMENSIONAL_SPARSE_SET_NAMESPACE
#define	MULTIDIMENSIONAL_SPARSE_SET_NAMESPACE	fang
#endif

// full description in "multidimensional_sparse_set.h"
namespace MULTIDIMENSIONAL_SPARSE_SET_NAMESPACE {
//=============================================================================
using namespace std;

//=============================================================================
// forward declarations
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

