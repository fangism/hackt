/**
	\file "artobjdump.cc"
	Just dumps an object file to human-readable (?) output.  
	Useful for testing object file integrity.  

	$Id: artobjdump.cc,v 1.8 2005/05/20 19:28:31 fang Exp $
 */

#include <iostream>
#include "main/main_funcs.h"
#include "util/using_ostream.h"

using namespace ART;

int
main(int argc, char* argv[]) {
	if (argc != 2) {
		cerr << "Usage: " << argv[0] << " <art-obj-file>" << endl;
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

