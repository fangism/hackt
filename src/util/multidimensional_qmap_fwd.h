// "multidimensional_qmap_fwd.h"
// forward declarations for
// a fixed depth/dimension tree representing sparsely instantiated indices
// David Fang, Cornell University, 2004

#ifndef	__MULTIDIMENSIONAL_QMAP_FWD_H__
#define	__MULTIDIMENSIONAL_QMAP_FWD_H__

#include <list>

#ifndef	MULTIDIMENSIONAL_QMAP_NAMESPACE
#define	MULTIDIMENSIONAL_QMAP_NAMESPACE		fang
#endif

// full description in "multidimensional_qmap.h"
namespace MULTIDIMENSIONAL_QMAP_NAMESPACE {
//=============================================================================
using namespace std;

//=============================================================================
// forward declarations
// default to indexing by int, and containing bare char* strings

template <class K = int, class T = char*,
		template <class> class L = list >
	class base_multidimensional_qmap;

template <size_t, class K = int, class T = char*,
		template <class> class L = list >
	class multidimensional_qmap;

#define	BASE_MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE		\
	template <class K, class T, template <class> class L>

#define	MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE		\
	template <size_t D, class K, class T, template <class> class L>

//=============================================================================
}	// end namespace MULTIDIMENSIONAL_QMAP_NAMESPACE

#endif	// __MULTIDIMENSIONAL_QMAP_FWD_H__

