/**
	\file "main/version.cc"
	Converts ART source code to an object file (pre-unrolled).
	This file was born from "art++2obj.cc" in earlier revision history.

	$Id: version.cc,v 1.2 2005/11/03 07:52:07 fang Exp $
 */

#include <iostream>
#include "main/program_registry.h"
#include "main/version.h"
#include "main/main_funcs.h"
#include "config.h"
#include "cvstag.h"
#include "builddate.h"
// #include "util/getopt_portable.h"

namespace ART {
#include "util/using_ostream.h"

//=============================================================================

class version::options {
};	// end class options

//=============================================================================
// class version static initializers

const char
version::name[] = "version";

const char
version::brief_str[] = "Shows the HACKT version info.";

const size_t
version::program_id = register_hackt_program_class<version>();

//=============================================================================
version::version() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The main program just prints the version information.  
 */
int
version::main(const int argc, char* argv[], const global_options&) {
	if (argc > 1) {
		usage();
	}
	cout << "Version: " << PACKAGE_STRING << endl;
	cout << "CVS Tag: " << CVSTAG << endl;
	cout << "Built: " << BUILDDATE << endl;
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
	cerr << "version: prints HACKT version" << endl;
	cerr << "usage: version" << endl;
}

//=============================================================================

}	// end namespace ART

