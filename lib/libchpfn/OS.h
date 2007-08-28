/**
	\file "OS.h"
	Operating-system related library functions.
	$Id: OS.h,v 1.2 2007/08/28 04:53:47 fang Exp $
 */

#ifndef	__HAC_LIBCHPFN_OS_H__
#define	__HAC_LIBCHPFN_OS_H__

#include "Object/expr/types.h"

namespace HAC {
namespace CHP {
using entity::int_value_type;
using entity::string_value_type;

extern
int_value_type
System(const string_value_type&);

}	// end namespace CHP
}	// end namespace HAC

#endif	// __HAC_LIBCHPFN_OS_H__

