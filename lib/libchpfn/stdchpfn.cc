/**
	\file "libchpfn/stdchpfn.cc"
	This exports the normal library functions into a chpsim module
	suitable for run-time loading (dlopen).
	$Id: stdchpfn.cc,v 1.2 2007/08/15 02:48:36 fang Exp $
 */

#include "libchpfn/assert.h"
#include "libchpfn/conditional.h"
#include "sim/chpsim/chpsim_dlfunction.h"

namespace HAC {
namespace CHP {
USING_CHPSIM_DLFUNCTION_PROLOGUE

// This just loads the named functions using the same name.
// Should you wish to create a library with remapped names, 
// just copy-modify this file.
CHP_DLFUNCTION_LOAD_DEFAULT("assert", Assert)
CHP_DLFUNCTION_LOAD_DEFAULT("bcond", bcond)
CHP_DLFUNCTION_LOAD_DEFAULT("zcond", zcond)
CHP_DLFUNCTION_LOAD_DEFAULT("rcond", rcond)

}	// end namespace CHP
}	// end namespace HAC

