/**
	\file "libchpfn/conditional.h"
	Interface to conditional expressions.
	$Id: conditional.h,v 1.2 2007/08/15 02:48:35 fang Exp $
 */

#ifndef	__HAC_LIBCHPFN_CONDITIONAL_H__
#define	__HAC_LIBCHPFN_CONDITIONAL_H__

#include "Object/expr/types.h"

namespace HAC {
namespace CHP {
using entity::int_value_type;
using entity::bool_value_type;
using entity::real_value_type;

extern
bool_value_type
bcond(const bool_value_type, const bool_value_type, const bool_value_type);

extern
int_value_type
zcond(const bool_value_type, const int_value_type, const int_value_type);

extern
real_value_type
rcond(const bool_value_type, const real_value_type, const real_value_type);

}	// end namespace CHP
}	// end namespace HAC

#endif	// __HAC_LIBCHPFN_CONDITIONAL_H__

