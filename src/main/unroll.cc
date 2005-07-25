/**
	\file "artobjunroll.cc"
	Unrolls an object file, saves it to another object file.  

	$Id: unroll.cc,v 1.1 2005/07/25 02:10:09 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include <cstring>

#include "main/unroll.h"
#include "main/program_registry.h"
#include "util/stacktrace.h"
#include "main/main_funcs.h"
#include "util/persistent_object_manager.h"

namespace ART {

using util::persistent;
using util::persistent_object_manager;

#include "util/using_ostream.h"

//=============================================================================
class unroll::options {
	// none
};	// end class options

//=============================================================================
// class unroll static initializers

const char
unroll::name[] = "unroll";

const char
unroll::brief_str[] = "Unrolls an object file, saving to another object file";

const size_t
unroll::program_id =
	register_hackt_program(unroll::name, unroll::main, unroll::brief_str);

//=============================================================================
unroll::unroll() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
unroll::main(const int argc, char* argv[], const global_options&) {
	options opt;
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
	if (the_module->is_unrolled()) {
		cerr << "Module is already unrolled, skipping..." << endl;
	} else {
		STACKTRACE("main: try unrolling.");
		if (!the_module->unroll_module().good) {
			cerr << "ERROR in unrolling.  Aborting." << endl;
			// although an empty unroll file was already created
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
unroll::usage(void) {
	cerr << "Usage: " << name <<
		" <art-obj-infile> <art-obj-outfile>" << endl;
}

//=============================================================================
}	// end namespace ART

