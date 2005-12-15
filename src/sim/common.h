/**
	\file "sim/common.h"
	Definitions useful for ALL simulations.  
	$Id: common.h,v 1.1.2.1 2005/12/15 04:46:03 fang Exp $
 */

#ifndef	__HAC_SIM_COMMON_H__
#define	__HAC_SIM_COMMON_H__

#include "util/size_t.h"

namespace HAC {
namespace SIM {
//=============================================================================
/**
	This is the integer type used to identify unique objects.  
 */
typedef	size_t		index_type;
typedef	index_type	node_index_type;
typedef	index_type	expr_index_type;
typedef	index_type	event_index_type;

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_COMMON_H__

