/**
	\file "Object/lang/PRS_attribute_common.h"
	Contains base classes for all tool-independent implementations
	of PRS rule attribute classes.  
	$Id: PRS_attribute_common.h,v 1.5 2007/10/08 01:21:20 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_PRS_ATTRIBUTE_COMMON_H__
#define	__HAC_OBJECT_LANG_PRS_ATTRIBUTE_COMMON_H__

#include "Object/lang/PRS_fwd.h"
#include "util/boolean_types.h"

namespace HAC {
namespace entity {
namespace PRS {

/**
	Parent namespace of tool-independent classes.  
	These base classes are not registered.  
 */
namespace rule_attributes {
using util::good_bool;
//=============================================================================
/**
	Convenience macro for repetitive definitions.  
	Consider using type tags to name these base classes?
	Contains an argument-checking member function.  
 */
#define	DECLARE_PRS_ATTRIBUTE_COMMON_STRUCT(class_name)			\
struct class_name {							\
	typedef	rule_attribute_values_type		values_type;	\
	static								\
	good_bool							\
	__check_vals(const char*, const values_type&);			\
};	// end struct class_name


/**
	Delay specification for a single rule.  
 */
DECLARE_PRS_ATTRIBUTE_COMMON_STRUCT(After)

//=============================================================================
/**
	Weak attribute.  
 */
DECLARE_PRS_ATTRIBUTE_COMMON_STRUCT(Weak)

/**
	Unstable attribute.  
 */
DECLARE_PRS_ATTRIBUTE_COMMON_STRUCT(Unstab)

//-----------------------------------------------------------------------------
// inherited attributes from ACT toolchain
// source: http://vlsi.cornell.edu/wiki/doku.php?id=designtools:netgen

/// for automatic combinational-feedback configuration
DECLARE_PRS_ATTRIBUTE_COMMON_STRUCT(Comb)

/// for explicit staticizer
DECLARE_PRS_ATTRIBUTE_COMMON_STRUCT(Keeper)

/// label terminals of pass transistors as output
DECLARE_PRS_ATTRIBUTE_COMMON_STRUCT(Output)

/// specify load capacitance, overriding default assumptions
DECLARE_PRS_ATTRIBUTE_COMMON_STRUCT(LoadCap)

//=============================================================================
}	// end namespace attributes
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_PRS_ATTRIBUTE_COMMON_H__

