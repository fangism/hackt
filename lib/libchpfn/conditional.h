/**
	\file "libchpfn/conditional.h"
	Interface to conditional expressions.
	$Id: conditional.h,v 1.3 2007/08/28 04:53:47 fang Exp $
 */

#ifndef	__HAC_LIBCHPFN_CONDITIONAL_H__
#define	__HAC_LIBCHPFN_CONDITIONAL_H__

#include "Object/expr/types.h"
#include "Object/expr/dlfunction_fwd.h"

namespace HAC {
namespace CHP {
using entity::int_value_type;
using entity::bool_value_type;
using entity::real_value_type;
using entity::chp_function_return_type;
using entity::chp_function_argument_list_type;

extern
bool_value_type
bcond(const bool_value_type, const bool_value_type, const bool_value_type);

extern
int_value_type
zcond(const bool_value_type, const int_value_type, const int_value_type);

extern
real_value_type
rcond(const bool_value_type, const real_value_type, const real_value_type);

extern
chp_function_return_type
select(const chp_function_argument_list_type&);

}	// end namespace CHP
}	// end namespace HAC

#endif	// __HAC_LIBCHPFN_CONDITIONAL_H__

