/**
	\file "Object/lang/PRS_literal_attribute_common.hh"
	Contains base classes for all tool-independent implementations
	of PRS rule attribute classes.  
	$Id: PRS_literal_attribute_common.hh,v 1.3 2009/10/05 23:09:26 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_PRS_LITERAL_ATTRIBUTE_COMMON_H__
#define	__HAC_OBJECT_LANG_PRS_LITERAL_ATTRIBUTE_COMMON_H__

#include "Object/lang/PRS_fwd.hh"
#include "util/boolean_types.hh"

namespace HAC {
namespace entity {
namespace PRS {

/**
	Parent namespace of tool-independent classes.  
	These base classes are not registered.  
 */
namespace literal_attributes {
using util::good_bool;
//=============================================================================
/**
	Convenience macro for repetitive definitions.  
	Consider using type tags to name these base classes?
	Contains an argument-checking member function.  
 */
#define	DECLARE_PRS_LITERAL_ATTRIBUTE_COMMON_STRUCT(class_name)		\
struct class_name {							\
	typedef	literal_attribute_values_type		values_type;	\
	static								\
	good_bool							\
	__check_vals(const char*, const values_type&);			\
};	// end struct class_name

//=============================================================================
// transistor name overrides
DECLARE_PRS_LITERAL_ATTRIBUTE_COMMON_STRUCT(Label)
/**
	Transistor types.
 */
DECLARE_PRS_LITERAL_ATTRIBUTE_COMMON_STRUCT(LVT)
DECLARE_PRS_LITERAL_ATTRIBUTE_COMMON_STRUCT(SVT)
DECLARE_PRS_LITERAL_ATTRIBUTE_COMMON_STRUCT(HVT)

//=============================================================================
}	// end namespace literal_attributes
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_PRS_LITERAL_ATTRIBUTE_COMMON_H__

