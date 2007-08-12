/**
	\file "libchpfn/assert.h"
	Assert function.
	$Id: assert.h,v 1.1.2.1 2007/08/12 19:44:07 fang Exp $
 */

#ifndef	__HAC_LIBCHPFN_ASSERT_H__
#define	__HAC_LIBCHPFN_ASSERT_H__

#include "Object/expr/types.h"

namespace HAC {
namespace CHP {
using entity::bool_value_type;

extern
void
assert(const bool_value_type);

}	// end namespace CHP
}	// end namespace HAC

#endif	// __HAC_LIBCHPFN_ASSERT_H__

