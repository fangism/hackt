/**
	\file "cond.cc"
	$Id: cond.cc,v 1.1.2.2 2007/08/10 06:50:11 fang Exp $
	Conditional operator.
	Unfortunately, all arguments are evaluated unconditionally
		before one is selected.  To evaluate only one would require
		a special form.  
	And selection operator (one of many), eventually.
 */

#include "sim/chpsim/chpsim_dlfunction.h"
// #include "libhackt/sim/chpsim/chpsim_dlfunction.h"

USING_CHPSIM_DLFUNCTION_PROLOGUE

template <class T>
static
T
_cond(const bool_value_type z, 
		const T a, const T b) {
	return z ? a : b;
}

static
bool_value_type
bcond(const bool_value_type z, 
		const bool_value_type a, const bool_value_type b) {
	return _cond(z, a, b);
}

static
int_value_type
zcond(const bool_value_type z, 
		const int_value_type a, const int_value_type b) {
	return _cond(z, a, b);
}

static
real_value_type
rcond(const bool_value_type z, 
		const real_value_type a, const real_value_type b) {
	return _cond(z, a, b);
}

CHP_DLFUNCTION_LOAD_DEFAULT("bcond", bcond)
CHP_DLFUNCTION_LOAD_DEFAULT("zcond", zcond)
CHP_DLFUNCTION_LOAD_DEFAULT("rcond", rcond)

/**
	Generalized conditional expression, where arguments
	can be of heterogenous type.  
	Very rare use of this...
	\param a first argument is the index to the rest of the list,
		so a[0] (must be int) refers to the first of a[1...].
 */
chp_function_return_type
gcond(const const_param_expr_list& a) {
	size_t index = extract_chp_value<int_value_type>(a[0]);
	return a[index +1];
}

REGISTER_DLFUNCTION_RAW("gcond", gcond)

