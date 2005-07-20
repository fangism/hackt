/**
	\file "artobjunroll.cc"
	Unrolls an object file, saves it to another object file.  

	$Id: artobjunroll.cc,v 1.15 2005/07/20 20:59:49 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include <cstring>

#include "util/stacktrace.h"
#include "main/main_funcs.h"
#include "util/persistent_object_manager.h"

using namespace ART;
using util::persistent;
using util::persistent_object_manager;

#include "util/using_ostream.h"

int
main(int argc, char* argv[]) {
	if (argc != 3) {
		cerr << "Usage: " << argv[0] <<
			" <art-obj-infile> <art-obj-outfile>" << endl;
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

