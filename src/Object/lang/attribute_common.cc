/**
	\file "Object/lang/attribute_common.cc"
	$Id: attribute_common.cc,v 1.5 2011/03/06 21:02:36 fang Exp $
 */

#include <iostream>
#include "Object/expr/expr_fwd.hh"
#include "Object/lang/attribute_common.hh"
#include "Object/expr/const_collection.hh"
#include "Object/expr/pint_const.hh"
#include "Object/expr/preal_const.hh"
#include "Object/expr/string_expr.hh"
#include "Object/expr/const_param_expr_list.hh"
#include "Object/traits/value_traits.hh"
#include "util/memory/count_ptr.hh"

namespace HAC {
namespace entity {
namespace attributes {
#include "util/using_ostream.hh"

// NOTE: pint_const can also come in the form of pint_const_collection, scalar

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
		(v.size() &&
			!v[0].is_a<const pint_const>() &&
			!v[0].is_a<const pint_const_collection>()
			)) {
		cerr << "The \'" << name << "\' attribute requires accepts "
			"one optional pint (integer) argument." << endl;
		return good_bool(false);
	} else  return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
check_single_integer(const char* name, const attribute_values_type& v) {
	if (v.size() != 1 ||
			(!v[0].is_a<const pint_const>() &&
			!v[0].is_a<const pint_const_collection>())
			) {
		cerr << "The \'" << name << "\' attribute requires exactly "
			"one pint (integer) expression argument." << endl;
		return good_bool(false);
	} else  return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
check_single_real(const char* name, const attribute_values_type& v) {
	if (v.size() != 1 ||
			(!v[0].is_a<const preal_const>() &&
			!v[0].is_a<const preal_const_collection>() &&
			!v[0].is_a<const pint_const>() &&
			!v[0].is_a<const pint_const_collection>())
			) {
		cerr << "The \'" << name << "\' attribute requires exactly "
			"one preal (floating-point) expression argument."
			<< endl;
		if (v.size() == 1) {
			v[0]->what(cerr << "got: ") << endl;
			v[0]->dump(cerr << "value: ") << endl;
		}
		return good_bool(false);
	} else  return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Value must be non-negative integer.
	TODO: accept real-values
	FIXME: body should also allow scalar pint_const_collection
 */
good_bool
check_delay_value(const char* name, const attribute_values_type& v) {
	if (v.size() != 1 || 
			(!v[0].is_a<const pint_const>() &&
			!v[0].is_a<const pint_const_collection>())
			) {
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
check_single_string(const char* name, const attribute_values_type& v) {
	if (v.size() != 1 || !v[0].is_a<const string_expr>()) {
		cerr << "The \'" << name << "\' attribute requires exactly "
			"one string expression argument." << endl;
		return good_bool(false);
	} else  return good_bool(true);
}

//=============================================================================
}	// end namespace attributes
}	// end namespace entity
}	// end namespace HAC

