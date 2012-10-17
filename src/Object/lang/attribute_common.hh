/**
	\file "Object/lang/attribute_common.hh"
	Helper functions for common attribute value checking.  
	$Id: attribute_common.hh,v 1.4 2009/10/05 23:09:27 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_ATTRIBUTE_COMMON_H__
#define	__HAC_OBJECT_LANG_ATTRIBUTE_COMMON_H__

#include <iosfwd>
#include "util/boolean_types.hh"

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

extern
good_bool
check_delay_value(const char*, const attribute_values_type&);

extern
good_bool
check_single_string(const char*, const attribute_values_type&);


//=============================================================================
}	// end namespace attributes
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_ATTRIBUTE_COMMON_H__

