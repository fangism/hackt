/**
	\file "libchpfn/OS.cc"
	$Id: OS.cc,v 1.2 2007/08/28 04:53:46 fang Exp $
 */

#include "libchpfn/OS.h"
#include <cstdlib>
#include <string>

namespace HAC {
namespace CHP {
//=============================================================================
/***
@texinfo fn/system.texi
@deffn Function system cmd
Execute the command @var{cmd} in the parent shell.
Returns the exit status.  
@end deffn
@end texinfo
***/
int_value_type
System(const string_value_type& cmd) {
	return std::system(cmd.c_str());
}

//=============================================================================
}	// end namespace CHP
}	// end namespace HAC


