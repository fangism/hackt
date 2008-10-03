/**
	\file "Object/lang/attribute_common.cc"
	$Id: attribute_common.cc,v 1.1 2008/10/03 02:04:28 fang Exp $
 */

#include <iostream>
#include "Object/lang/attribute_common.h"
#include "Object/expr/pint_const.h"
#include "Object/expr/preal_const.h"
#include "Object/expr/const_param_expr_list.h"
#include "util/memory/count_ptr.h"

namespace HAC {
namespace entity {
namespace attributes {
#include "util/using_ostream.h"

//=============================================================================
// define re-usable, common checking functions here

good_bool
check_no_value(const char* name, const attribute_values_type& v) {
	if (v.size()) {
		cerr << "The \'" << name << "\' attribute takes "
			"no expression arguments." << endl;
		return good_bool(false);
	} else	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
check_optional_integer(const char* name, const attribute_values_type& v) {
	if (v.size() > 1 ||
		(v.size() && !v[0].is_a<const pint_const>())) {
		cerr << "The \'" << name << "\' attribute requires accepts "
			"one optional pint (integer) argument." << endl;
		return good_bool(false);
	} else  return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
check_single_integer(const char* name, const attribute_values_type& v) {
	if (v.size() != 1 || !v[0].is_a<const pint_const>()) {
		cerr << "The \'" << name << "\' attribute requires exactly "
			"one pint (integer) expression argument." << endl;
		return good_bool(false);
	} else  return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
check_single_real(const char* name, const attribute_values_type& v) {
	if (v.size() != 1 || !v[0].is_a<const preal_const>()) {
		cerr << "The \'" << name << "\' attribute requires exactly "
			"one preal (floating-point) expression argument."
			<< endl;
		return good_bool(false);
	} else  return good_bool(true);
}

//=============================================================================
}	// end namespace attributes
}	// end namespace entity
}	// end namespace HAC

