/**
	\file "Object/lang/PRS_attribute_common.cc"
	$Id: PRS_attribute_common.cc,v 1.12 2010/09/01 22:14:19 fang Exp $
 */

#include <iostream>
#include "Object/lang/PRS_attribute_common.h"
#include "Object/lang/attribute_common.h"
// #include "Object/expr/pint_const.h"
// #include "Object/expr/preal_const.h"
// #include "Object/expr/const_param_expr_list.h"
#include "common/TODO.h"
#include "util/memory/count_ptr.h"

namespace HAC {
namespace entity {
namespace PRS {
namespace rule_attributes {
#include "util/using_ostream.h"
using namespace entity::attributes;

//=============================================================================
/**
	Requires a single integer-valued delay.  
	TODO: why not use real-valued delay?
 */
good_bool
After::__check_vals(const char* name, const values_type& v) {
        return check_delay_value(name, v);
}

good_bool
AfterMin::__check_vals(const char* name, const values_type& v) {
        return check_delay_value(name, v);
}

good_bool
AfterMax::__check_vals(const char* name, const values_type& v) {
        return check_delay_value(name, v);
}

//=============================================================================
good_bool
Width::__check_vals(const char* name, const values_type& v) {
	return check_single_real(name, v);
}

good_bool
Length::__check_vals(const char* name, const values_type& v) {
	return check_single_real(name, v);
}

//=============================================================================
good_bool
HVT::__check_vals(const char* name, const values_type& v) {
	return check_optional_integer(name, v);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
SVT::__check_vals(const char* name, const values_type& v) {
	return check_optional_integer(name, v);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
LVT::__check_vals(const char* name, const values_type& v) {
	return check_optional_integer(name, v);
}

//=============================================================================
/**
	Takes a single integer value.  1 or 0.
*/
good_bool
Always_Random::__check_vals(const char* name, const values_type& v) {
        return check_optional_integer(name, v);
}

//=============================================================================
/**
	Takes a single integer value.  1 or 0.  
 */
good_bool
Weak::__check_vals(const char* name, const values_type& v) {
	return check_optional_integer(name, v);
}

//=============================================================================
/**
	Takes a single integer value.  1 or 0.  
 */
good_bool
Unstab::__check_vals(const char* name, const values_type& v) {
	return check_optional_integer(name, v);
}

//=============================================================================
good_bool
Comb::__check_vals(const char* name, const values_type& v) {
	return check_single_integer(name, v);
}

//=============================================================================
good_bool
Keeper::__check_vals(const char* name, const values_type& v) {
	return check_single_integer(name, v);
}

//=============================================================================
good_bool
IsKeeper::__check_vals(const char* name, const values_type& v) {
	return check_optional_integer(name, v);
}

//=============================================================================
good_bool
IsCKeeper::__check_vals(const char* name, const values_type& v) {
	return check_optional_integer(name, v);
}

//=============================================================================
good_bool
Output::__check_vals(const char* name, const values_type& v) {
	FINISH_ME(Fang);
	return good_bool(false);
}

//=============================================================================
good_bool
LoadCap::__check_vals(const char* name, const values_type& v) {
	return check_single_real(name, v);
}

//=============================================================================
good_bool
N_reff::__check_vals(const char* name, const values_type& v) {
	return check_single_real(name, v);
}

good_bool
P_reff::__check_vals(const char* name, const values_type& v) {
	return check_single_real(name, v);
}

//=============================================================================
}	// end namespace rule_attributes
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

