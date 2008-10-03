/**
	\file "Object/lang/PRS_attribute_common.cc"
	$Id: PRS_attribute_common.cc,v 1.7 2008/10/03 02:04:27 fang Exp $
 */

#include <iostream>
#include "Object/lang/PRS_attribute_common.h"
#include "Object/lang/attribute_common.h"
#include "Object/expr/pint_const.h"
// #include "Object/expr/preal_const.h"
#include "Object/expr/const_param_expr_list.h"
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
	if (v.size() != 1 || !v[0].is_a<const pint_const>()) {
		cerr << "The \'" << name << "\' attribute requires exactly "
			"one pint (integer) expression argument." << endl;
		return good_bool(false);
	} else {
		const pint_value_type d =
			v[0].is_a<const pint_const>()->static_constant_value();
		if (d < 0) {
			cerr << "Delay value must be >= 0, but got: " << d
				<< endl;
			return good_bool(false);
		} else {
			return good_bool(true);
		}
	}
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
}	// end namespace rule_attributes
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

