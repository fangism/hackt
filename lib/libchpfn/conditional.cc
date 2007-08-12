/**
	\file "libchpfn/conditional.cc"
	$Id: conditional.cc,v 1.1.2.1 2007/08/12 19:44:10 fang Exp $
 */

#include "libchpfn/conditional.h"

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

//=============================================================================
}	// end namespace CHP
}	// end namespace HAC

