/**
	\file "main/haco.cc"
	Invokes the compiler-driver.  
	$Id: haco.cc,v 1.1 2007/09/13 01:14:16 fang Exp $
 */

// #define	ENABLE_STATIC_TRACE			1
#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include "main/haco.h"
#include "main/compile.h"
#include "main/global_options.h"
#include "main/program_registry.h"

namespace HAC {
#ifndef	WITH_MAIN
/**
	\pre name and function pointer already initialized...
 */
const size_t
compile::program_id = register_hackt_program_class<compile>();
#endif	// WITH_MAIN

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
haco::haco() { }

}	// end namespace HAC

//=============================================================================
#ifdef	WITH_MAIN
/**
	Assumes no global hackt options.  
 */
int
main(const int argc, char* argv[]) {
	const HAC::global_options g;
	return HAC::compile::main(argc, argv, g);
}
#endif

DEFAULT_STATIC_TRACE_END


