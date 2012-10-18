/**
	\file "libchpfn/assert.hh"
	Assert function.
	$Id: assert.hh,v 1.2 2007/08/15 02:48:35 fang Exp $
 */

#ifndef	__HAC_LIBCHPFN_ASSERT_H__
#define	__HAC_LIBCHPFN_ASSERT_H__

#include "Object/expr/types.hh"

namespace HAC {
namespace CHP {
using entity::bool_value_type;

extern
void
Assert(const bool_value_type);

}	// end namespace CHP
}	// end namespace HAC

#endif	// __HAC_LIBCHPFN_ASSERT_H__

