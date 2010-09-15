/**
	\file "libchpfn/conditional.cc"
	$Id: conditional.cc,v 1.3.54.1 2010/09/15 00:57:42 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include "libchpfn/conditional.h"
#include "Object/expr/const_param_expr_list.h"
#include "Object/expr/const_param.h"
#include "sim/chpsim/chpsim_dlfunction.h"
#include "util/memory/count_ptr.h"
#include "util/stacktrace.h"

#if ENABLE_STACKTRACE
// debugging only
#include <iostream>
#include "Object/expr/expr_dump_context.h"
#endif

namespace HAC {
namespace CHP {
USING_CHPSIM_DLFUNCTION_PROLOGUE
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

/**
@texinfo fn/strcond.texi
@deffn Function strcond z a b
Conditional expression, for string rvalues.  
If @var{z} is true, return @var{a}, else return @var{b}.
NOTE: both expressions @var{a} and @var{b} 
are evaluated @emph{unconditionally}.
@end deffn
@end texinfo
 */
string_value_type
strcond(const bool_value_type z,
		const string_value_type a, const string_value_type b) {
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
	STACKTRACE_VERBOSE;
	const int_value_type index = extract_int(*args[0]);
	const chp_function_return_type& ret(args[index +1]);
	NEVER_NULL(ret);
#if ENABLE_STACKTRACE
	ret->dump(std::cout << "select: ",
		entity::expr_dump_context::default_value) << std::endl;
#endif
	return ret;
}

//=============================================================================
}	// end namespace CHP
}	// end namespace HAC

