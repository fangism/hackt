/**
	\file "multidimensional_qmap_fwd.h"
	Forward declarations for a fixed depth/dimension sparse tree.
	$Id: multidimensional_qmap_fwd.h,v 1.5.24.1 2005/02/07 01:11:15 fang Exp $
 */
// David Fang, Cornell University, 2004

#ifndef	__UTIL_MULTIDIMENSIONAL_QMAP_FWD_H__
#define	__UTIL_MULTIDIMENSIONAL_QMAP_FWD_H__

#if 0
#include <list>
#else
#include "STL/list_fwd.h"
#endif

#ifndef	MULTIDIMENSIONAL_QMAP_NAMESPACE
#define	MULTIDIMENSIONAL_QMAP_NAMESPACE		util
#endif

#define	WANT_BASE_MULTIDIMENSIONAL_QMAP		0

// full description in "multidimensional_qmap.h"
namespace MULTIDIMENSIONAL_QMAP_NAMESPACE {
//=============================================================================
USING_LIST

//=============================================================================
// forward declarations
// default to indexing by int, and containing bare char* strings

#if WANT_BASE_MULTIDIMENSIONAL_QMAP
template <class K = int, class T = char*,
		template <class> class L = list >
	class base_multidimensional_qmap;
#endif

template <size_t, class K = int, class T = char*,
		template <class> class L = list >
	class multidimensional_qmap;

#if WANT_BASE_MULTIDIMENSIONAL_QMAP
#define	BASE_MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE			\
	template <class K, class T, template <class> class L>
#endif

#define	MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE			\
	template <size_t D, class K, class T, template <class> class L>

#define	SPECIALIZED_MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE		\
	template <class K, class T, template <class> class L>

//=============================================================================
}	// end namespace MULTIDIMENSIONAL_QMAP_NAMESPACE

#endif	// __UTIL_MULTIDIMENSIONAL_QMAP_FWD_H__

