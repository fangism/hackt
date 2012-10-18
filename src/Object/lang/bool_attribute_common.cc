/**
	\file "Object/lang/bool_attribute_common.cc"
	$Id: bool_attribute_common.cc,v 1.6 2010/08/24 22:52:03 fang Exp $
 */

#include <iostream>
#include "Object/lang/bool_attribute_common.hh"
#include "Object/lang/attribute_common.hh"
#include "Object/expr/pint_const.hh"
#include "Object/expr/preal_const.hh"
#include "Object/expr/const_param_expr_list.hh"
#include "common/TODO.hh"
#include "util/memory/count_ptr.hh"

namespace HAC {
namespace entity {
namespace bool_attributes {
#include "util/using_ostream.hh"
using namespace entity::attributes;

//=============================================================================
good_bool
AllowInterference::__check_vals(const char* name, const values_type& v) {
	return check_optional_integer(name, v);
}

good_bool
AllowWeakInterference::__check_vals(const char* name, const values_type& v) {
	return check_optional_integer(name, v);
}

//=============================================================================
/**
	Takes a single integer value.  1 or 0.  
 */
good_bool
PseudoStatic::__check_vals(const char* name, const values_type& v) {
	return check_single_integer(name, v);
}

//=============================================================================
/**
	Takes a single integer value.  1 or 0.  
	When unspecified, value is assumed to be 1, which is non-default.  
 */
good_bool
IsComb::__check_vals(const char* name, const values_type& v) {
	return check_optional_integer(name, v);
}

//=============================================================================
/**
	When unspecified, value is assumed to be 1, which is default.  
 */
good_bool
AutoKeeper::__check_vals(const char* name, const values_type& v) {
	return check_optional_integer(name, v);
}

//=============================================================================
good_bool
Supply::__check_vals(const char* name, const values_type& v) {
	return check_single_integer(name, v);
}

good_bool
Reset::__check_vals(const char* name, const values_type& v) {
	return check_single_integer(name, v);
}

//=============================================================================
good_bool
IsRVC1::__check_vals(const char* name, const values_type& v) {
	return check_optional_integer(name, v);
}

good_bool
IsRVC2::__check_vals(const char* name, const values_type& v) {
	return check_optional_integer(name, v);
}

good_bool
IsRVC3::__check_vals(const char* name, const values_type& v) {
	return check_optional_integer(name, v);
}

//=============================================================================
}	// end namespace bool_attributes
}	// end namespace entity
}	// end namespace HAC

