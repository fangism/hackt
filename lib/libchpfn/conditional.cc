/**
	\file "libchpfn/conditional.cc"
	$Id: conditional.cc,v 1.2.2.1 2007/08/23 21:36:06 fang Exp $
 */

#include "libchpfn/conditional.h"
#include "Object/expr/const_param_expr_list.h"
#include "Object/expr/const_param.h"
#include "Object/expr/dlfunction.h"
#include "util/memory/count_ptr.h"

namespace HAC {
namespace CHP {
//=============================================================================

template <class T>
static
T
_cond(const bool_value_type z,
		const T a, const T b) {
	return z ? a : b;
}

/**
@texinfo fn/bcond.texi
@deffn Function bcond z a b
Conditional expression, for boolean rvalues.  
If @var{z} is true, return @var{a}, else return @var{b}.
NOTE: both expressions @var{a} and @var{b} 
are evaluated @emph{unconditionally}.
@end deffn
@end texinfo
 */
bool_value_type
bcond(const bool_value_type z,
		const bool_value_type a, const bool_value_type b) {
	return _cond(z, a, b);
}

/**
@texinfo fn/zcond.texi
@deffn Function zcond z a b
Conditional expression, for integer rvalues.  
If @var{z} is true, return @var{a}, else return @var{b}.
NOTE: both expressions @var{a} and @var{b} 
are evaluated @emph{unconditionally}.
@end deffn
@end texinfo
 */
int_value_type
zcond(const bool_value_type z,
		const int_value_type a, const int_value_type b) {
	return _cond(z, a, b);
}

/**
@texinfo fn/rcond.texi
@deffn Function rcond z a b
Conditional expression, for floating-point (real) rvalues.  
If @var{z} is true, return @var{a}, else return @var{b}.
NOTE: both expressions @var{a} and @var{b} 
are evaluated @emph{unconditionally}.
@end deffn
@end texinfo
 */
real_value_type
rcond(const bool_value_type z,
		const real_value_type a, const real_value_type b) {
	return _cond(z, a, b);
}

/***
@texinfo fn/select.texi
@deffn Function select index args...
Returns expression @i{args[index]}, where @var{index} is 0-based.  
Throws run-time exception if @var{index} is out-of-range.  
@end deffn
@end texinfo
***/
chp_function_return_type
select(const chp_function_argument_list_type& args) {
	const int_value_type index = extract_int(*args[0]);
	return args[index -1];
}

//=============================================================================
}	// end namespace CHP
}	// end namespace HAC

