/**
	\file "main/shell.cc"
	Interactive shell for HACKT.  
	$Id: shell.cc,v 1.1.2.1 2005/11/03 02:16:36 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include <cstring>

#include "main/shell.h"
#include "main/program_registry.h"
#include "util/stacktrace.h"
#include "main/main_funcs.h"
#include "util/persistent_object_manager.h"
#include "util/getopt_portable.h"

namespace ART {
using util::persistent;
using util::persistent_object_manager;
#include "util/using_ostream.h"

//=============================================================================
class shell::options {
	// none
};	// end class options

//=============================================================================
// class shell static initializers

const char
shell::name[] = "shell";

const char
shell::brief_str[] = "Interactive environment shell";

const size_t
shell::program_id = register_hackt_program_class<shell>();

//=============================================================================
shell::shell() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The main shell interpreter loop.  
	TODO: be able to source script files, etc...
	TODO: be able to escape to system commands, using !-prefix, like vi.  
	TODO: be able to take command-line arguments
 */
int
shell::main(const int argc, char* argv[], const global_options&) {
	options opt();
	if (argc != 1) {
		usage();
		return 1;
	}
	cerr << "This doesn\'t do anything ... yet." << endl;
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
shell::usage(void) {
	cerr << "Usage: " << name <<
		" (forthcoming...)" << endl;
}

//=============================================================================
}	// end namespace ART

