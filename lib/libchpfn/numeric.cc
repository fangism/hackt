/**
	\file "libchpfn/numeric.cc"
	$Id: numeric.cc,v 1.1 2010/10/14 17:33:44 fang Exp $
 */

#include "libchpfn/numeric.h"
#include "util/macros.h"
#include "sim/chpsim/chpsim_dlfunction.h"
#include "Object/expr/expr_fwd.h"
#include "Object/expr/const_collection.h"
#include "Object/expr/pint_const.h"
#include "Object/expr/const_param_expr_list.h"

namespace HAC {
namespace CHP {
using entity::make_meta_value;
using entity::extract_meta_value;
using entity::pint_const;
using entity::pbool_const_collection;

//=============================================================================
/***
@texinfo fn/sign_extend.texi
@deffn Function sign_extend int
Extends integer to arbitrary width.  
This is useful for converting between integer results of different widths.
@end deffn
@end texinfo
***/
int_value_type
sign_extend(const int_value_type v) {
	return v;
}

//-----------------------------------------------------------------------------
/***
@texinfo fn/extract_bit.texi
@deffn Function extract_bit int pos
Extracts a single bit from an int, returning a boolean value.
The LSB is indexed at 0.
Function: (@var{int} >> @var{pos} & 1)
@end deffn
@end texinfo
***/
bool_value_type
extract_bit(const int_value_type v, const int_value_type p) {
	return (v >> p) & 0x1;
}

//-----------------------------------------------------------------------------
/***
@texinfo fn/extract_bits.texi
@deffn Function extract_bits int pos1 pos2
Extracts a range of bits from an int, returning an int.
The LSB is indexed at 0.
Function: (@var{int} & (2^@var{pos2} -1)) >> @var{pos1}
@end deffn
@end texinfo
***/
int_value_type
extract_bits(const int_value_type v,
		const int_value_type p1, const int_value_type p2) {
	INVARIANT(p1 <= p2);
	return (v & ((1 << p2) -1)) >> p1;
}

//-----------------------------------------------------------------------------
#if 0
/***
Converts a 1-d array of bools to integer.
***/
chp_function_return_type
bits_to_int(const chp_function_argument_list_type& p) {
	INVARIANT(p.size() == 1);
}
#endif

//=============================================================================
}	// end namespace HAC
}	// end namespace CHP

