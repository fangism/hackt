/**
	\file "sim/prsim/enums.h"
	$Id: enums.h,v 1.2.4.1 2008/11/26 05:16:31 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_ENUMS_H__
#define	__HAC_SIM_PRSIM_ENUMS_H__

#include "sim/prsim/devel_switches.h"

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
	PULL_WEAK = 0x02
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
	LOGIC_MASK = 0x03
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
/**
	Policy enumeration for determining simulation behavior
	in the event of a delay-insensitivity violation.  
	Absolute values matter, in increasing order of severity.
 */
enum error_policy_enum {
	ERROR_IGNORE = 0,
	ERROR_NONE = ERROR_IGNORE,
	ERROR_WARN = 1,
	ERROR_NOTIFY = ERROR_WARN,
	ERROR_BREAK = 2,
	/// return control to user temporarily before resuming script
	ERROR_INTERACTIVE = 3,
	/// halt the simulation immediately
	ERROR_FATAL = 4,
	ERROR_INVALID
};

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_ENUMS_H__

