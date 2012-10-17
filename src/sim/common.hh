/**
	\file "sim/common.hh"
	Definitions useful for ALL simulations.  
	$Id: common.hh,v 1.5 2008/11/05 23:03:38 fang Exp $
 */

#ifndef	__HAC_SIM_COMMON_H__
#define	__HAC_SIM_COMMON_H__

#include "util/size_t.h"

namespace HAC {
namespace SIM {
//=============================================================================
/**
	This is the integer type used to identify unique objects.  
	This may change depending on native machine integer size!
 */
typedef	size_t		index_type;
typedef	index_type	node_index_type;
typedef	index_type	expr_index_type;
typedef	index_type	event_index_type;
typedef	index_type	process_index_type;
/**
	The set of rules is just a subset of the set of expressions.  
 */
typedef	expr_index_type	rule_index_type;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
enum {
	INVALID_NODE_INDEX = 0,
	INVALID_EXPR_INDEX = 0,
	INVALID_EVENT_INDEX = 0,
	INVALID_RULE_INDEX = INVALID_EXPR_INDEX
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

