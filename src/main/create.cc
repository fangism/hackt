/**
	\file "main/create.cc"
	Unrolls an object file, saves it to another object file.  

	$Id: create.cc,v 1.6.92.1 2007/03/10 07:29:46 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include <cstring>

#include "main/create.h"
#include "main/program_registry.h"
#include "util/stacktrace.h"
#include "main/main_funcs.h"
#include "main/global_options.h"
#include "util/persistent_object_manager.h"

namespace HAC {

using util::persistent;
using util::persistent_object_manager;

#include "util/using_ostream.h"

//=============================================================================
class create::options {
	// none
};	// end class options

//=============================================================================
// class create static initializers

const char
create::name[] = "create";

const char
create::brief_str[] = "Creates unique state for aliased objects for simulation";

#ifndef	WITH_MAIN
const size_t
create::program_id = register_hackt_program_class<create>();
#endif

//=============================================================================
create::create() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
create::main(const int argc, char* argv[], const global_options&) {
	options opt();
	if (argc != 3) {
		usage();
		return 1;
	}
	if (!strcmp(argv[1], argv[2])) {
		cerr << "The output objfile should not be the same as the input."
			<< endl;
		return 1;
	}
	if (!check_object_loadable(argv[1]).good)
		return 1;
	if (!check_file_writeable(argv[2]).good)
		return 1;

	// TODO: move this into the options class
	persistent_object_manager::dump_reconstruction_table = false;
	persistent::warn_unimplemented = true;	// for verbosity

	excl_ptr<module> the_module = load_module(argv[1]);
	if (!the_module)
		return 1;

//	the_module->dump(cerr);
	if (the_module->is_created()) {
		cerr << "Module is already created, skipping..." << endl;
	} else {
		STACKTRACE("main: try createing.");
		if (!the_module->create_unique().good) {
			cerr << "ERROR in creating.  Aborting." << endl;
			return 1;
		}
//		the_module->dump(cerr);
	}

	persistent_object_manager::dump_reconstruction_table = false;
	save_module(*the_module, argv[2]);
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
create::usage(void) {
	cerr << "usage: " << name <<
		" <hackt-obj-infile> <hackt-obj-outfile>" << endl;
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
	return HAC::create::main(argc, argv, g);
}
#endif	// WITH_MAIN

