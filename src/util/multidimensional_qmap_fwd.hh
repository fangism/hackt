/**
	\file "util/multidimensional_qmap_fwd.hh"
	Forward declarations for a fixed depth/dimension sparse tree.
	$Id: multidimensional_qmap_fwd.hh,v 1.10 2006/04/18 18:42:43 fang Exp $
 */
// David Fang, Cornell University, 2004

#ifndef	__UTIL_MULTIDIMENSIONAL_QMAP_FWD_H__
#define	__UTIL_MULTIDIMENSIONAL_QMAP_FWD_H__

#include <list>
#include "util/size_t.h"

// full description in "multidimensional_qmap.hh"
namespace util {
//=============================================================================
// forward declarations
// default to indexing by int, and containing bare char* strings

#define	MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE			\
	template <size_t D, class K, class T, class L>

#define	SPECIALIZED_MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE		\
	template <class K, class T, class L>

MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE
class multidimensional_qmap;

SPECIALIZED_MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE
class multidimensional_qmap<1,K,T,L>;

//=============================================================================
}	// end namespace util

#endif	// __UTIL_MULTIDIMENSIONAL_QMAP_FWD_H__

