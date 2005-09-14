/**
	\file "main/cflat.cc"
	cflat backwards compability module.  

	$Id: cflat.cc,v 1.1.2.1 2005/09/14 19:20:03 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
// #include <cstring>

#include "main/cflat.h"
#include "main/program_registry.h"
#include "util/stacktrace.h"
#include "main/main_funcs.h"
#include "util/persistent_object_manager.h"

namespace ART {

using util::persistent;
using util::persistent_object_manager;

#include "util/using_ostream.h"

//=============================================================================
class cflat::options {
	// none
};	// end class options

//=============================================================================
// class cflat static initializers

const char
cflat::name[] = "cflat";

const char
cflat::brief_str[] =
	"Flattens instantiations CAST-style (backward compatibility)";

const size_t
cflat::program_id = register_hackt_program_class<cflat>();

//=============================================================================
cflat::cflat() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
cflat::main(const int argc, char* argv[], const global_options&) {
	options opt();
	if (argc != 3) {
		usage();
		return 1;
	}
	if (!check_object_loadable(argv[2]).good)
		return 1;

	excl_ptr<module> the_module = load_module(argv[2]);
	if (!the_module)
		return 1;

	if (!the_module->allocate_unique().good) {
		cerr << "ERROR in allocating global state.  Aborting." << endl;
		return 1;
	}

	// cflat here!!!
	// based on mode, set the options to pass into the module.
	if (!the_module->cflat(cout).good) {
		cerr << "Error during cflat." << endl;
		return 1;
	}

	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
cflat::usage(void) {
	cerr << "Usage: " << name << " <mode> <hackt-obj-infile>" << endl;
	// list modes
}

//=============================================================================
}	// end namespace ART

