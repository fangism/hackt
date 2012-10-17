/**
	\file "main/prsim-main.cc"
	Traditional production rule simulator. 
	$Id: prsim.cc,v 1.31 2011/05/03 19:20:53 fang Exp $
 */

#include "util/static_trace.hh"
DEFAULT_STATIC_TRACE_BEGIN

#include "main/prsim-main.hh"
#include "main/program_registry.hh"

#ifndef	WITH_MAIN
namespace HAC {
const size_t
prsim::program_id = register_hackt_program_class<prsim>();
}
#endif

#ifdef	WITH_MAIN
/**
	Assumes no global hackt options.  
 */
int
main(const int argc, char* argv[]) {
	const HAC::global_options g;
	return HAC::prsim::main(argc, argv, g);
}
#endif

DEFAULT_STATIC_TRACE_END
