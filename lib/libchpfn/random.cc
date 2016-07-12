/**
	\file "libchpfn/bitmanip.cc"
	$Id: bitmanip.cc,v 1.1 2007/08/29 04:45:41 fang Exp $
 */

#include <cstdlib>
#include "libchpfn/random.hh"

namespace HAC {
namespace CHP {

//=============================================================================

/***
@texinfo fn/srand48.texi
@deffn Function srand48 int
Sets random number generator seed in C stdlib.
@end deffn
@end texinfo
***/
void
srand48(const int_value_type v) {
	::srand48(v);
}

/***
@texinfo fn/lrand48.texi
@deffn Function lrand48
Returns a random unsigned integer value in [0, 2^31).
@end deffn
@end texinfo
***/
int_value_type
lrand48(void) {
	return ::lrand48();
}

/***
@texinfo fn/mrand48.texi
@deffn Function mrand48
Returns a random signed integer value in (-2^31, 2^31).
@end deffn
@end texinfo
***/
int_value_type
mrand48(void) {
	return ::mrand48();
}

//=============================================================================
}	// end namespace HAC
}	// end namespace CHP

