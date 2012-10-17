/**
	\file "sim/prsim/enums.hh"
	$Id: enums.hh,v 1.7 2010/06/29 01:55:05 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_ENUMS_H__
#define	__HAC_SIM_PRSIM_ENUMS_H__

#include "sim/common.hh"
#include "sim/prsim/devel_switches.hh"

namespace HAC {
namespace SIM {
namespace PRSIM {
//=============================================================================
/**
	These values are special, they correspond to 
	LOGIC_LOW, LOGIC_HIGH, LOGIC_OTHER.  
	Consider re-enumerating so that 2-x can be used
	to invert.  (Will need to recode some tables in this case.)
 */
enum pull_enum {
	PULL_OFF = 0x00,
	PULL_ON = 0x01,
	PULL_WEAK = 0x02,
	PULL_INVALID = 0xFF
};

//-----------------------------------------------------------------------------
/**
	Values that a node can take.  
 */
enum value_enum {
	LOGIC_LOW = 0x00,		// 0
	LOGIC_HIGH = 0x01,		// 1
	LOGIC_VALUE = 0x01,		// value mask
	LOGIC_OTHER = 0x02,		// 2
	LOGIC_MASK = 0x03,
	LOGIC_INVALID = 0xFF		// to signal error
};

//-----------------------------------------------------------------------------
#if PRSIM_WEAK_RULES
/**
	Enumeration of rule strengths.  
 */
enum rule_strength {
	NORMAL_RULE = 0,
	WEAK_RULE = 1
};
#define	STR_INDEX(w)	[w]
#else
#define	STR_INDEX(w)
#endif

//-----------------------------------------------------------------------------
enum {
	/// index of the first valid global node
	FIRST_VALID_GLOBAL_NODE = SIM::INVALID_NODE_INDEX +1,
	/// index of the first valid global expr/expr_graph_node
	FIRST_VALID_GLOBAL_EXPR = SIM::INVALID_EXPR_INDEX +1,
	/// index of the first valid local node
	FIRST_VALID_LOCAL_NODE = 0,
	/// index of the first valid local expr/expr_graph_node
	FIRST_VALID_LOCAL_EXPR = 0,
	/// index of first valid process, 0 is the top-level process
	FIRST_VALID_PROCESS = 0,
	/// index of the first valid event
	FIRST_VALID_EVENT = SIM::INVALID_EVENT_INDEX +1
};

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_ENUMS_H__

