/**
	\file "main/version.cc"
	Converts ART source code to an object file (pre-unrolled).
	This file was born from "art++2obj.cc" in earlier revision history.

	$Id: version.cc,v 1.2.2.2 2005/11/04 23:30:24 fang Exp $
 */

#include <iostream>
#include "main/program_registry.h"
#include "main/version.h"
#include "main/main_funcs.h"
#include "config.h"
#include "cvstag.h"
#include "builddate.h"
#include "cxx_version.h"
#include "lexer/lex_version.h"
#include "parser/yacc_version.h"
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
	TODO: compiled with... cc:, c++:
		yacc:
		lex:
		do this all in configure script
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
	cxx(o) << endl;
	lex(o) << endl;
	yacc(o) << endl;
	build(o) << endl;
	return 0;
}

//-----------------------------------------------------------------------------
ostream&
version::package(ostream& o) {
	return o << "Version: " << PACKAGE_STRING;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
version::cvs(ostream& o) {
	return o << "CVS Tag: " << CVSTAG;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
version::cxx(ostream& o) {
	return o << "c++: " << CXX_VERSION;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
version::lex(ostream& o) {
	return o << "lex: " << LEX_VERSION;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
version::yacc(ostream& o) {
	return o << "yacc: " << YACC_VERSION;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
version::build(ostream& o) {
	return o << "Built: " << BUILDDATE;
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

