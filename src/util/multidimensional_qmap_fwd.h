/**
	\file "multidimensional_qmap_fwd.h"
	Forward declarations for a fixed depth/dimension sparse tree.
	$Id: multidimensional_qmap_fwd.h,v 1.5 2004/12/16 01:08:53 fang Exp $
 */
// David Fang, Cornell University, 2004

#ifndef	__MULTIDIMENSIONAL_QMAP_FWD_H__
#define	__MULTIDIMENSIONAL_QMAP_FWD_H__

#if 0
#include <list>
#else
#include "STL/list_fwd.h"
#endif

#ifndef	MULTIDIMENSIONAL_QMAP_NAMESPACE
#define	MULTIDIMENSIONAL_QMAP_NAMESPACE		util
#endif

// full description in "multidimensional_qmap.h"
namespace MULTIDIMENSIONAL_QMAP_NAMESPACE {
//=============================================================================
USING_LIST

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

