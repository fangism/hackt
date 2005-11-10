/**
	\file "main/version.cc"
	Prints configuration information, everything a maintainer
	would want to know about another's installation configuration.  
	$Id: version.cc,v 1.3 2005/11/10 02:13:07 fang Exp $
 */

#include <iostream>
#include "main/program_registry.h"
#include "main/version.h"
#include "main/main_funcs.h"
#include "config.h"

// various configure-generated and make-generated headers
#include "cvstag.h"
#include "builddate.h"
#include "cxx_version.h"
#include "cxxflags.h"
#include "am_cxxflags.h"
#include "lexer/lex_version.h"
#include "parser/yacc_version.h"
#include "buildhost.h"

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
 */
int
version::main(const int argc, char* argv[], const global_options&) {
	if (argc > 1) {
		usage();
		// don't bother returning
	}
	ostream& o(cout);
	package(o) << endl;
	cvs(o) << endl;
	buildhost(o) << endl;
	cxx(o) << endl;
	cxxflags(o) << endl;
	lex(o) << endl;
	yacc(o) << endl;
	builddate(o) << endl;

	// influential environment variables
	return 0;
}

//-----------------------------------------------------------------------------
ostream&
version::package(ostream& o) {
	return o << "Version: " PACKAGE_STRING;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
version::cvs(ostream& o) {
	return o << "CVS Tag: " CVSTAG;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
version::cxx(ostream& o) {
	return o << "c++: " CXX_VERSION;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
version::cxxflags(ostream& o) {
	o << "AM_CXXFLAGS: " AM_CXXFLAGS;
	return o << endl << "config-CXXFLAGS: " CONFIG_CXXFLAGS;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
version::lex(ostream& o) {
	return o << "lex: " LEX_VERSION;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
version::yacc(ostream& o) {
	return o << "yacc: " YACC_VERSION;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
version::builddate(ostream& o) {
	return o << "build-date: " BUILDDATE;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints the host triplet only if it's different from the build triplet.
 */
ostream&
version::buildhost(ostream& o) {
	o << "build-triplet: " CONFIG_BUILD;
	if (strcmp(CONFIG_HOST, CONFIG_BUILD))
		o << endl << "host-triplet: " CONFIG_HOST;
	return o;
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

}	// end namespace ART

