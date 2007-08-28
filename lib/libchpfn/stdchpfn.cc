/**
	\file "libchpfn/stdchpfn.cc"
	This exports the normal library functions into a chpsim module
	suitable for run-time loading (dlopen).
	$Id: stdchpfn.cc,v 1.3 2007/08/28 04:53:49 fang Exp $
 */

#include "libchpfn/assert.h"
#include "libchpfn/conditional.h"
#include "libchpfn/io.h"
#include "libchpfn/OS.h"
#include "libchpfn/string.h"
#include "sim/chpsim/chpsim_dlfunction.h"

namespace HAC {
namespace CHP {
USING_CHPSIM_DLFUNCTION_PROLOGUE

//=============================================================================
// This just loads the named functions using the same name.
// Should you wish to create a library with remapped names, 
// just copy-modify this file.
CHP_DLFUNCTION_LOAD_DEFAULT("assert", Assert)

// NOTE: all functions referenced are form the HAC::CHP namespace
// (not the global C namespace) unless otherwise noted.  

// from "libchpfn/conditional.h"
CHP_DLFUNCTION_LOAD_DEFAULT("bcond", bcond)
CHP_DLFUNCTION_LOAD_DEFAULT("zcond", zcond)
CHP_DLFUNCTION_LOAD_DEFAULT("rcond", rcond)
REGISTER_DLFUNCTION_RAW("select", select)

// from "libchpfn/io.h"
REGISTER_DLFUNCTION_RAW("echo", echo)
REGISTER_DLFUNCTION_RAW("cout", echo)
REGISTER_DLFUNCTION_RAW("print", print)
REGISTER_DLFUNCTION_RAW("printerr", printerr_nl)
REGISTER_DLFUNCTION_RAW("cerr", printerr)
CHP_DLFUNCTION_LOAD_DEFAULT("zscan", zscan)
CHP_DLFUNCTION_LOAD_DEFAULT("zscan_prompt", zscan_prompt)
CHP_DLFUNCTION_LOAD_DEFAULT("bscan", bscan)
CHP_DLFUNCTION_LOAD_DEFAULT("bscan_prompt", bscan_prompt)
// CHP_DLFUNCTION_LOAD_DEFAULT("rscan", rscan)
// CHP_DLFUNCTION_LOAD_DEFAULT("rscan_prompt", rscan_prompt)
CHP_DLFUNCTION_LOAD_DEFAULT("sscan", sscan)
CHP_DLFUNCTION_LOAD_DEFAULT("sscan_prompt", sscan_prompt)
REGISTER_DLFUNCTION_RAW("fprint", fprint)
CHP_DLFUNCTION_LOAD_DEFAULT("fopen", fopen)
CHP_DLFUNCTION_LOAD_DEFAULT("fappend", fappend)
CHP_DLFUNCTION_LOAD_DEFAULT("fclose", fclose)
CHP_DLFUNCTION_LOAD_DEFAULT("fflush", fflush)
CHP_DLFUNCTION_LOAD_DEFAULT("fzscan", fzscan)
CHP_DLFUNCTION_LOAD_DEFAULT("fbscan", fbscan)
CHP_DLFUNCTION_LOAD_DEFAULT("fsscan", fsscan)
// CHP_DLFUNCTION_LOAD_DEFAULT("frscan", fsscan)
CHP_DLFUNCTION_LOAD_DEFAULT("fzscan_loop", fzscan_loop)
CHP_DLFUNCTION_LOAD_DEFAULT("fbscan_loop", fbscan_loop)
CHP_DLFUNCTION_LOAD_DEFAULT("fsscan_loop", fsscan_loop)
// CHP_DLFUNCTION_LOAD_DEFAULT("frscan_loop", frscan_loop)

// from "libchpfn/OS.h"
CHP_DLFUNCTION_LOAD_DEFAULT("system", System)

// from "libchpfn/string.h"
CHP_DLFUNCTION_LOAD_DEFAULT("strcat", strcat)
CHP_DLFUNCTION_LOAD_DEFAULT("sprint", strcat)
CHP_DLFUNCTION_LOAD_DEFAULT("tostring", strcat)
CHP_DLFUNCTION_LOAD_DEFAULT("strtoz", strtoz)
CHP_DLFUNCTION_LOAD_DEFAULT("strtob", strtob)
CHP_DLFUNCTION_LOAD_DEFAULT("strtor", strtor)

//=============================================================================
}	// end namespace CHP
}	// end namespace HAC

