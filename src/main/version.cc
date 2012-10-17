/**
	\file "main/version.cc"
	Prints configuration information, everything a maintainer
	would want to know about another's installation configuration.  
	$Id: version.cc,v 1.11 2009/10/16 20:38:47 fang Exp $
 */

#include <iostream>
#include "main/program_registry.hh"
#include "main/version.hh"
#include "main/main_funcs.hh"
#include "common/config.hh"

// #include "util/getopt_portable.h"

namespace HAC {
#include "util/using_ostream.hh"

//=============================================================================

class version::options {
};	// end class options

//=============================================================================
// class version static initializers

const char
version::name[] = "version";

const char
version::brief_str[] = "Shows the HACKT version and configuration info.";

const size_t
version::program_id = register_hackt_program_class<version>();

//=============================================================================
version::version() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The main program just prints the version information.  
	TODO: configure params, paths, etc...
	TODO: if/when we decide to support certain environment variables
		make sure we can dump all the relevant variables.  
	TODO: optional packages, like readline, etc...
 */
int
version::main(const int argc, char*[], const global_options&) {
	if (argc > 1) {
		usage();
		// don't bother returning
	}
	config::dump_all(cout);
	return 0;
}

//-----------------------------------------------------------------------------
#if 0
/**
	\return 0 if is ok to continue, anything else will signal early
		termination, an error will cause exit(1).
	NOTE: the getopt option string begins with '+' to enforce
		POSIXLY correct termination at the first non-option argument.  
 */
int
version::parse_command_options(const int argc, char* argv[], options& opt) {
	return 0;
}
#endif

//-----------------------------------------------------------------------------
void
version::usage(void) {
	cerr << "version: prints HACKT version and configuration" << endl;
	cerr << "usage: " << name << endl;
}

//=============================================================================

}	// end namespace HAC

