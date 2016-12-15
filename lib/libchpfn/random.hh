/**
	\file "libchpfn/random.hh"
	$Id: bitmanip.hh,v 1.1 2007/08/29 04:45:41 fang Exp $
 */

#ifndef	__HAC_LIBCHPFN_RANDOM_HH__
#define	__HAC_LIBCHPFN_RANDOM_HH__

#include "Object/expr/types.hh"

namespace HAC {
namespace CHP {
using entity::bool_value_type;
using entity::int_value_type;

extern
void
srand48(const int_value_type);

extern
int_value_type
lrand48(void);

extern
int_value_type
mrand48(void);


}	// end namespace CHP
}	// end namespace HAC

#endif	// __HAC_LIBCHPFN_RANDOM_HH__

