/**
	\file "main/version.cc"
	Prints configuration information, everything a maintainer
	would want to know about another's installation configuration.  
	$Id: version.cc,v 1.7.60.1 2007/03/28 06:11:53 fang Exp $
 */

#include <iostream>
#include "main/program_registry.h"
#include "main/version.h"
#include "main/main_funcs.h"
#include "common/config.h"

// #include "util/getopt_portable.h"

namespace HAC {
#include "util/using_ostream.h"

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
version::main(const int argc, char* argv[], const global_options&) {
	if (argc > 1) {
		usage();
		// don't bother returning
	}
	ostream& o(cout);
	config::package(o) << endl;
	config::cvs(o) << endl;
	config::configure_params(o) << endl;
	config::buildhost(o) << endl;
	config::cxx(o) << endl;
	config::cxxflags(o) << endl;
	config::lex(o) << endl;
	config::yacc(o) << endl;
	config::readline(o) << endl;
	config::builddate(o) << endl;
	// influential environment variables
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

