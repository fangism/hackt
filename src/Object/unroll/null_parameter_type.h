/**
	\file "Object/unroll/null_parameter_type.h"
	$Id: null_parameter_type.h,v 1.2 2006/03/15 04:38:24 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_NULL_PARAMETER_TYPE_H__
#define	__HAC_OBJECT_UNROLL_NULL_PARAMETER_TYPE_H__

namespace HAC {
namespace entity {
class unroll_context;

//=============================================================================
// for classes without parameter (bool)
struct null_parameter_type {
	/// type is never null, added since canonical_type was used
	operator bool () const { return true; }

	unroll_context
	make_unroll_context(void) const;
};      // end struct null_parameter_type

//=============================================================================
}	// end namespace entity
}	// end namespace HAC


#endif	// __HAC_OBJECT_UNROLL_NULL_PARAMETER_TYPE_H__

