/**
	\file "Object/lang/attribute_common.cc"
	Helper functions for common attribute value checking.  
	$Id: attribute_common.h,v 1.1 2008/10/03 02:04:29 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_ATTRIBUTE_COMMON_H__
#define	__HAC_OBJECT_LANG_ATTRIBUTE_COMMON_H__

#include <iosfwd>
// #include "Object/expr/const_param_expr_list.h"
#include "util/boolean_types.h"

namespace HAC {
namespace entity {
class const_param_expr_list;

namespace attributes {
using util::good_bool;
typedef	const_param_expr_list	attribute_values_type;

//=============================================================================
// define re-usable, common checking functions here

extern
good_bool
check_no_value(const char*, const attribute_values_type&);

extern
good_bool
check_optional_integer(const char*, const attribute_values_type&);

extern
good_bool
check_single_integer(const char*, const attribute_values_type&);

extern
good_bool
check_single_real(const char*, const attribute_values_type&);

//=============================================================================
}	// end namespace attributes
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_ATTRIBUTE_COMMON_H__

