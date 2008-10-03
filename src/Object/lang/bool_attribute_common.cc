/**
	\file "Object/lang/bool_attribute_common.cc"
	$Id: bool_attribute_common.cc,v 1.1 2008/10/03 02:04:29 fang Exp $
 */

#include <iostream>
#include "Object/lang/bool_attribute_common.h"
#include "Object/expr/pint_const.h"
#include "Object/expr/preal_const.h"
#include "Object/expr/const_param_expr_list.h"
#include "common/TODO.h"
#include "util/memory/count_ptr.h"

namespace HAC {
namespace entity {
namespace bool_attributes {
#include "util/using_ostream.h"

//=============================================================================
// define re-usable, common checking functions here

/**
	COPIED from PRS_attribute_common.cc
 */
static
good_bool
check_single_integer(const char* name, const bool_attribute_values_type& v) {
	if (v.size() != 1 || !v[0].is_a<const pint_const>()) {
		cerr << "The \'" << name << "\' attribute requires exactly "
			"one pint (integer) expression argument." << endl;
		return good_bool(false);
	} else  return good_bool(true);
}

#if 0
/**
	COPIED from PRS_attribute_common.cc
 */
static
good_bool
check_single_real(const char* name, const bool_attribute_values_type& v) {
	if (v.size() != 1 || !v[0].is_a<const preal_const>()) {
		cerr << "The \'" << name << "\' attribute requires exactly "
			"one preal (floating-point) expression argument."
			<< endl;
		return good_bool(false);
	} else  return good_bool(true);
}
#endif

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
 */
good_bool
IsComb::__check_vals(const char* name, const values_type& v) {
	return check_single_integer(name, v);
}

//=============================================================================
good_bool
AutoKeeper::__check_vals(const char* name, const values_type& v) {
	return check_single_integer(name, v);
}

//=============================================================================
}	// end namespace bool_attributes
}	// end namespace entity
}	// end namespace HAC

