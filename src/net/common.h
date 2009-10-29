/**
	\file "net/common.h"
	For useful declarations and typedefs.  
	$Id: common.h,v 1.8 2009/10/29 17:45:48 fang Exp $
 */

#ifndef	__HAC_NET_COMMON_H__
#define	__HAC_NET_COMMON_H__

#include <iosfwd>
#include "common/status.h"

// Old perm'd flags:
// #define	CACHE_LOGICAL_NODE_NAMES		1
// #define	CACHE_INTERNAL_NODE_NAMES		1
// #define	CACHE_ALL_NODE_NAMES	(CACHE_LOGICAL_NODE_NAMES || CACHE_INTERNAL_NODE_NAMES)

namespace HAC {
namespace NET {
using std::ostream;

//=============================================================================
typedef	size_t		index_type;
typedef	double		real_type;

//=============================================================================

}	// end namespace NET
}	// end namespace HAC

#endif	// __HAC_NET_COMMON_H__

