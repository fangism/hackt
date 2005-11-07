/**
	\file "main/objdump.cc"
	Just dumps an object file to human-readable (?) output.  
	Useful for testing object file integrity.  
	This file came from "artobjdump.cc" in a previous life.  

	$Id: objdump.cc,v 1.3.14.1 2005/11/07 08:55:11 fang Exp $
 */

#include <iostream>
#include "main/objdump.h"
#include "main/program_registry.h"
#include "main/main_funcs.h"
#include "util/using_ostream.h"

namespace ART {
//=============================================================================
class objdump::options {
};	// end class options

//=============================================================================
// class objdump static initializers

const char
objdump::name[] = "objdump";

const char
objdump::brief_str[] = "Dumps HACKT object semi-human-readably to stdout.";

const size_t
objdump::program_id = register_hackt_program_class<objdump>();

//=============================================================================
// class objdump member definitions

objdump::objdump() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
objdump::main(int argc, char* argv[], const global_options&) {
	if (argc != 2) {
		usage();
		return 0;
	}
	if (!check_object_loadable(argv[1]).good)
		return 1;
	excl_ptr<module> the_module =
		load_module_debug(argv[1]);
		// load_module(argv[1]);
	if (!the_module)
		return 1;

	the_module->dump(cerr);
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
objdump::usage(void) {
	cerr << "usage: " << name << " <hackt-obj-file>" << endl;
}

//=============================================================================

}	// end namespace ART

