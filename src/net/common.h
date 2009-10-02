/**
	\file "net/common.h"
	For useful declarations and typedefs.  
	$Id: common.h,v 1.4 2009/10/02 01:57:28 fang Exp $
 */

#ifndef	__HAC_NET_COMMON_H__
#define	__HAC_NET_COMMON_H__

#include <iosfwd>

/**
	If set to 1, cache logical nodes' names in the node::name field, 
	instead of re-evaluating each time.  
	Tradeoff: use more string memory, but never have to re-evaluate
	(or re-mangle) the same name more than once.  
	Goal: 1 (I think)
	Status: Both values of switches tested.
 */
#define	CACHE_LOGICAL_NODE_NAMES		1
#define	CACHE_INTERNAL_NODE_NAMES		1

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

