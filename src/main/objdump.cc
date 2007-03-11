/**
	\file "main/objdump.cc"
	Just dumps an object file to human-readable (?) output.  
	Useful for testing object file integrity.  
	This file came from "artobjdump.cc" in a previous life.  

	$Id: objdump.cc,v 1.5.92.1 2007/03/11 05:13:56 fang Exp $
 */

#include <iostream>
#include "main/objdump.h"
#include "main/program_registry.h"
#include "main/main_funcs.h"
#include "main/global_options.h"
#include "util/using_ostream.h"

namespace HAC {
//=============================================================================
class objdump::options {
};	// end class options

//=============================================================================
// class objdump static initializers

const char
objdump::name[] = "objdump";

const char
objdump::brief_str[] = "Dumps HACKT object semi-human-readably to stdout.";

#ifndef	WITH_MAIN
const size_t
objdump::program_id = register_hackt_program_class<objdump>();
#endif

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

}	// end namespace HAC

#ifdef	WITH_MAIN
/**
	Assumes no global hackt options.  
 */
int
main(const int argc, char* argv[]) {
	const HAC::global_options g;
	return HAC::objdump::main(argc, argv, g);
}
#endif	// WITH_MAIN

