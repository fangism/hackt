/**
	\file "libchpfn/bitmanip.cc"
	$Id: bitmanip.cc,v 1.1 2007/08/29 04:45:41 fang Exp $
 */

#include "libchpfn/bitmanip.hh"
#include "util/bitmanip.hh"
#include "util/numeric/sign_traits.hh"

namespace HAC {
namespace CHP {
using util::numeric::to_unsigned_type;

//=============================================================================

/***
@texinfo fn/popcount.texi
@deffn Function popcount int
Returns the number of set bits in the binary representation of @var{int}.  
@end deffn
@end texinfo
***/
int_value_type
popcount(const int_value_type v) {
	return util::numeric::popcount(to_unsigned_type(v));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo fn/parity.texi
@deffn Function parity int
Returns parity of @var{int}, true if odd-parity, false if even-parity.
@end deffn
@end texinfo
***/
bool_value_type
parity(const int_value_type v) {
	return util::numeric::parity(to_unsigned_type(v));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo fn/clz32.texi
@deffn Function clz32 int
Returns the number of leading 0s in the 32b binary representation of @var{int}.
(Leading 0s are in the more significant bit positions.)  
Result is undefined if @var{int} is 0.  
@end deffn
@end texinfo
***/
int_value_type
clz32(const int_value_type v) {
	return util::numeric::clz(to_unsigned_type(v));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo fn/ctz.texi
@deffn Function ctz int
Returns the number of trailing 0s in the binary representation of @var{int}.
(Trailing 0s are in the less significant bit positions.)  
Result is undefined if @var{int} is 0.  
@end deffn
@end texinfo
***/
int_value_type
ctz(const int_value_type v) {
	return util::numeric::ctz(to_unsigned_type(v));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo fn/ffs.texi
@deffn Function ffs int
Returns the 1-indexed position of the first set bit in the 
binary representation of @var{int}.  
Position is counted from the least significant bits.  
Returns 0 if @var{int} is 0.  
@end deffn
@end texinfo
***/
int_value_type
ffs(const int_value_type v) {
	return util::numeric::ffs(to_unsigned_type(v));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo fn/msb.texi
@deffn Function msb int
Return the 0-indexed position of the most significant bit.
Undefined for @var{int} 0.  
@end deffn
@end texinfo
***/
int_value_type
msb(const int_value_type v) {
	return util::numeric::msb(to_unsigned_type(v));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo fn/lsb.texi
@deffn Function lsb int
Return the 0-indexed position of the least significant bit.
Undefined for @var{int} 0.  
@end deffn
@end texinfo
***/
int_value_type
lsb(const int_value_type v) {
	return util::numeric::lsb(to_unsigned_type(v));
}

//=============================================================================
}	// end namespace HAC
}	// end namespace CHP

