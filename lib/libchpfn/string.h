/**
	\file "libchpfn/string.h"
	String manipulation functions.
	Going to and from strings.  
	$Id: string.h,v 1.2 2007/08/28 04:53:51 fang Exp $
 */

#ifndef	__HAC_LIBCHPFN_STRING_H__
#define	__HAC_LIBCHPFN_STRING_H__

#include "Object/expr/types.h"
#include "Object/expr/dlfunction_fwd.h"

namespace HAC {
namespace CHP {
using entity::int_value_type;
using entity::bool_value_type;
using entity::real_value_type;
using entity::string_value_type;
using entity::chp_function_return_type;
using entity::chp_function_argument_list_type;

extern
string_value_type
strcat(const chp_function_argument_list_type&);

extern
int_value_type
strtoz(const string_value_type&);

extern
bool_value_type
strtob(const string_value_type&);

extern
real_value_type
strtor(const string_value_type&);

}	// end namespace CHP
}	// end namespace HAC

#endif	// __HAC_LIBCHPFN_STRING_H__

