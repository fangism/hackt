/**
	\file "Object/lang/PRS_attribute_common.hh"
	Contains base classes for all tool-independent implementations
	of PRS rule attribute classes.  
	$Id: PRS_attribute_common.hh,v 1.14 2010/09/01 22:14:19 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_PRS_RULE_ATTRIBUTE_COMMON_H__
#define	__HAC_OBJECT_LANG_PRS_RULE_ATTRIBUTE_COMMON_H__

#include "Object/lang/PRS_fwd.hh"
#include "util/boolean_types.hh"

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
#define	DECLARE_PRS_RULE_ATTRIBUTE_COMMON_STRUCT(class_name)		\
struct class_name {							\
	typedef	rule_attribute_values_type		values_type;	\
	static								\
	good_bool							\
	__check_vals(const char*, const values_type&);			\
};	// end struct class_name


/**
	Delay specification for a single rule.  
 */
DECLARE_PRS_RULE_ATTRIBUTE_COMMON_STRUCT(After)
DECLARE_PRS_RULE_ATTRIBUTE_COMMON_STRUCT(AfterMin)
DECLARE_PRS_RULE_ATTRIBUTE_COMMON_STRUCT(AfterMax)

/**
	Random Delay Specification for a single rule.
 */
DECLARE_PRS_RULE_ATTRIBUTE_COMMON_STRUCT(Always_Random)

//=============================================================================
/**
	Weak attribute.  
 */
DECLARE_PRS_RULE_ATTRIBUTE_COMMON_STRUCT(Weak)

/**
	Unstable attribute.  
	PRSIM: do not X out the output node in the event of instability,
		and also silence instability diagnostics on the rule.
 */
DECLARE_PRS_RULE_ATTRIBUTE_COMMON_STRUCT(Unstab)

//-----------------------------------------------------------------------------
// transistor sizing attributes
DECLARE_PRS_RULE_ATTRIBUTE_COMMON_STRUCT(Width)
DECLARE_PRS_RULE_ATTRIBUTE_COMMON_STRUCT(Length)

//-----------------------------------------------------------------------------
/**
	Transistor type attributes.  (for netlist generation)
	For convenience, rather than specifying per-literal.
 */
DECLARE_PRS_RULE_ATTRIBUTE_COMMON_STRUCT(HVT)
DECLARE_PRS_RULE_ATTRIBUTE_COMMON_STRUCT(SVT)	// default, anyway
DECLARE_PRS_RULE_ATTRIBUTE_COMMON_STRUCT(LVT)

//-----------------------------------------------------------------------------
// inherited attributes from ACT toolchain
// source: http://vlsi.cornell.edu/wiki/doku.php?id=designtools:netgen

/// for automatic combinational-feedback configuration
DECLARE_PRS_RULE_ATTRIBUTE_COMMON_STRUCT(Comb)

/// for explicit staticizer -- soon to be deprecated in favor of node attribute
DECLARE_PRS_RULE_ATTRIBUTE_COMMON_STRUCT(Keeper)
/// this rule is part of a staticizer, no need to prsim-simulate
DECLARE_PRS_RULE_ATTRIBUTE_COMMON_STRUCT(IsKeeper)
DECLARE_PRS_RULE_ATTRIBUTE_COMMON_STRUCT(IsCKeeper)

/// label terminals of pass transistors as output
DECLARE_PRS_RULE_ATTRIBUTE_COMMON_STRUCT(Output)

/// specify load capacitance, overriding default assumptions
DECLARE_PRS_RULE_ATTRIBUTE_COMMON_STRUCT(LoadCap)

/// dirty hacks... should be deprecated in favor of expression macros (fold)
DECLARE_PRS_RULE_ATTRIBUTE_COMMON_STRUCT(N_reff)
DECLARE_PRS_RULE_ATTRIBUTE_COMMON_STRUCT(P_reff)

/// for declaring that rule is diode-connected transistor to supply (not fanin)
DECLARE_PRS_RULE_ATTRIBUTE_COMMON_STRUCT(Diode)
/// for declaring that rule is fake resistor
DECLARE_PRS_RULE_ATTRIBUTE_COMMON_STRUCT(Res)

//=============================================================================
}	// end namespace rule_attributes
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_PRS_RULE_ATTRIBUTE_COMMON_H__

