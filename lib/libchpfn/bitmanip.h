/**
	\file "libchpfn/bitmanip.h"
	$Id: bitmanip.h,v 1.1 2007/08/29 04:45:41 fang Exp $
 */

#ifndef	__HAC_LIBCHPFN_BITMANIP_H__
#define	__HAC_LIBCHPFN_BITMANIP_H__

#include "Object/expr/types.h"

namespace HAC {
namespace CHP {
using entity::bool_value_type;
using entity::int_value_type;

extern
int_value_type
popcount(const int_value_type);

extern
bool_value_type
parity(const int_value_type);

extern
int_value_type
clz32(const int_value_type);

extern
int_value_type
ctz(const int_value_type);

extern
int_value_type
ffs(const int_value_type);

extern
int_value_type
msb(const int_value_type);

extern
int_value_type
lsb(const int_value_type);

}	// end namespace CHP
}	// end namespace HAC

#endif	// __HAC_LIBCHPFN_BITMANIP_H__

