/**
	\file "main/instdump.cc"
	Just dumps complete instance hierarchy expanded, 
	along with type information.  
	Useful for testing object file integrity.  
	Copy modified from "main/objdump.cc".

	$Id: instdump.cc,v 1.1 2008/12/13 00:27:56 fang Exp $
 */

#include <iostream>
#include "main/main_funcs.h"
#include "util/using_ostream.h"
#include "Object/module.h"

using namespace HAC;

//=============================================================================
static
void
usage(void) {
	cerr << "usage: hacinstdump <hackt-obj-file>" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
main(int argc, char* argv[]) {
	if (argc != 2) {
		usage();
		return 1;
	}
	if (!check_object_loadable(argv[1]).good)
		return 1;
	const count_ptr<module> the_module(load_module(argv[1]));
	if (!the_module)
		return 1;

	if (!the_module->is_allocated()) {
		if (!the_module->allocate_unique().good) {
			static const char alloc_errstr[] = 
				"ERROR in allocating global state.  Aborting.";
			cerr << alloc_errstr << endl;
			return 1;
		}
	}

	the_module->dump_instance_map(cout);
	return 0;
}
//=============================================================================

