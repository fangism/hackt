/**
	\file "Object/lang/PRS_fwd.h"
	Forward declarations and typedefs for PRS-related classes;
	$Id: PRS_fwd.h,v 1.5.18.1 2009/09/02 00:22:52 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_PRS_FWD_H__
#define	__HAC_OBJECT_LANG_PRS_FWD_H__

#include "util/utypes.h"

/**
	Define to 1 to use short for count type instead of char.
	I can't believe *someone* actually wrote prs with more than
	255 terms in a monolithic AND...
 */
#define	HUGE_MAX_EXPR_COUNT			1

namespace HAC {
namespace entity {
class const_param_expr_list;
typedef	const_param_expr_list		resolved_attribute_values_type;
typedef	resolved_attribute_values_type	bool_attribute_values_type;

namespace PRS {
typedef	resolved_attribute_values_type	rule_attribute_values_type;
typedef	resolved_attribute_values_type	literal_attribute_values_type;

#if HUGE_MAX_EXPR_COUNT
typedef	ushort				expr_count_type;
#else
typedef	uchar				expr_count_type;
#endif

}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

// keep this macro local
#undef	HUGE_MAX_EXPR_COUNT

#endif	// __HAC_OBJECT_LANG_PRS_FWD_H__

