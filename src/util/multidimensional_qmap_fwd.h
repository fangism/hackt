/**
	\file "multidimensional_qmap_fwd.h"
	Forward declarations for a fixed depth/dimension sparse tree.
	$Id: multidimensional_qmap_fwd.h,v 1.6 2005/02/27 22:54:22 fang Exp $
 */
// David Fang, Cornell University, 2004

#ifndef	__UTIL_MULTIDIMENSIONAL_QMAP_FWD_H__
#define	__UTIL_MULTIDIMENSIONAL_QMAP_FWD_H__

#if 0
#include <list>
#else
#include "STL/list_fwd.h"
#endif

// full description in "multidimensional_qmap.h"
namespace util {
//=============================================================================
USING_LIST

//=============================================================================
// forward declarations
// default to indexing by int, and containing bare char* strings

template <size_t, class K = int, class T = char*,
		template <class> class L = list >
	class multidimensional_qmap;

#define	MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE			\
	template <size_t D, class K, class T, template <class> class L>

#define	SPECIALIZED_MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE		\
	template <class K, class T, template <class> class L>

//=============================================================================
}	// end namespace util

#endif	// __UTIL_MULTIDIMENSIONAL_QMAP_FWD_H__

