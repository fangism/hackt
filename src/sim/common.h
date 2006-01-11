/**
	\file "sim/common.h"
	Definitions useful for ALL simulations.  
	$Id: common.h,v 1.1.2.2 2006/01/11 00:07:33 fang Exp $
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
enum {
	INVALID_NODE_INDEX = 0,
	INVALID_EXPR_INDEX = 0
};

//-----------------------------------------------------------------------------
static
inline
bool
is_valid_node_index(const node_index_type& i) {
	return i != INVALID_NODE_INDEX;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
inline
bool
is_valid_expr_index(const expr_index_type& i) {
	return i != INVALID_EXPR_INDEX;
}

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_COMMON_H__

