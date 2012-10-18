/**
	\file "libchpfn/stdchpfn.cc"
	This exports the normal library functions into a chpsim module
	suitable for run-time loading (dlopen).
	$Id: stdchpfn.cc,v 1.6 2010/10/14 17:33:45 fang Exp $
 */

#include "libchpfn/assert.hh"
#include "libchpfn/conditional.hh"
#include "libchpfn/io.hh"
#include "libchpfn/OS.hh"
#include "libchpfn/string.hh"
#include "libchpfn/bitmanip.hh"
#include "libchpfn/numeric.hh"
#include "sim/chpsim/chpsim_dlfunction.hh"

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

// from "libchpfn/conditional.hh"
CHP_DLFUNCTION_LOAD_DEFAULT("bcond", bcond)
CHP_DLFUNCTION_LOAD_DEFAULT("zcond", zcond)
CHP_DLFUNCTION_LOAD_DEFAULT("rcond", rcond)
REGISTER_DLFUNCTION_RAW("select", select)

// from "libchpfn/io.hh"
REGISTER_DLFUNCTION_RAW("echo", echo)
REGISTER_DLFUNCTION_RAW("cout", echo)
REGISTER_DLFUNCTION_RAW("print", print)
REGISTER_DLFUNCTION_RAW("printerr", printerr_nl)
REGISTER_DLFUNCTION_RAW("cerr", printerr)
CHP_DLFUNCTION_LOAD_DEFAULT("zscan", zscan)
CHP_DLFUNCTION_LOAD_DEFAULT("dzscan", zscan)
CHP_DLFUNCTION_LOAD_DEFAULT("bzscan", bzscan)
CHP_DLFUNCTION_LOAD_DEFAULT("xzscan", xzscan)
CHP_DLFUNCTION_LOAD_DEFAULT("zscan_prompt", zscan_prompt)
CHP_DLFUNCTION_LOAD_DEFAULT("dzscan_prompt", zscan_prompt)
CHP_DLFUNCTION_LOAD_DEFAULT("bzscan_prompt", bzscan_prompt)
CHP_DLFUNCTION_LOAD_DEFAULT("xzscan_prompt", xzscan_prompt)
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
CHP_DLFUNCTION_LOAD_DEFAULT("fdzscan", fzscan)
CHP_DLFUNCTION_LOAD_DEFAULT("fbzscan", fbzscan)
CHP_DLFUNCTION_LOAD_DEFAULT("fxzscan", fxzscan)
CHP_DLFUNCTION_LOAD_DEFAULT("fbscan", fbscan)
CHP_DLFUNCTION_LOAD_DEFAULT("fsscan", fsscan)
// CHP_DLFUNCTION_LOAD_DEFAULT("frscan", fsscan)
CHP_DLFUNCTION_LOAD_DEFAULT("fzscan_loop", fzscan_loop)
CHP_DLFUNCTION_LOAD_DEFAULT("fdzscan_loop", fzscan_loop)
CHP_DLFUNCTION_LOAD_DEFAULT("fbzscan_loop", fbzscan_loop)
CHP_DLFUNCTION_LOAD_DEFAULT("fxzscan_loop", fxzscan_loop)
CHP_DLFUNCTION_LOAD_DEFAULT("fbscan_loop", fbscan_loop)
CHP_DLFUNCTION_LOAD_DEFAULT("fsscan_loop", fsscan_loop)
// CHP_DLFUNCTION_LOAD_DEFAULT("frscan_loop", frscan_loop)

// from "libchpfn/OS.hh"
CHP_DLFUNCTION_LOAD_DEFAULT("system", System)

// from "libchpfn/string.hh"
CHP_DLFUNCTION_LOAD_DEFAULT("strcat", strcat)
CHP_DLFUNCTION_LOAD_DEFAULT("sprint", strcat)
CHP_DLFUNCTION_LOAD_DEFAULT("tostring", strcat)
CHP_DLFUNCTION_LOAD_DEFAULT("strtoz", strtoz)
CHP_DLFUNCTION_LOAD_DEFAULT("strtob", strtob)
CHP_DLFUNCTION_LOAD_DEFAULT("strtor", strtor)

// from "libchpfn/bitmanip.hh"
CHP_DLFUNCTION_LOAD_DEFAULT("popcount", popcount)
CHP_DLFUNCTION_LOAD_DEFAULT("parity", parity)
CHP_DLFUNCTION_LOAD_DEFAULT("clz32", clz32)
CHP_DLFUNCTION_LOAD_DEFAULT("ctz", ctz)
CHP_DLFUNCTION_LOAD_DEFAULT("ffs", ffs)
CHP_DLFUNCTION_LOAD_DEFAULT("msb", msb)
CHP_DLFUNCTION_LOAD_DEFAULT("lsb", lsb)

// from "libchpfn/numeric.hh"
CHP_DLFUNCTION_LOAD_DEFAULT("sign_extend", sign_extend)
CHP_DLFUNCTION_LOAD_DEFAULT("extract_bit", extract_bit)
CHP_DLFUNCTION_LOAD_DEFAULT("extract_bits", extract_bits)

//=============================================================================
}	// end namespace CHP
}	// end namespace HAC

