/**
	\file "artobjdump.cc"
	Just dumps an object file to human-readable (?) output.  
	Useful for testing object file integrity.  

	$Id: artobjdump.cc,v 1.6 2005/01/28 19:58:31 fang Exp $
 */

#include <iostream>
#include <fstream>
#include "art++.h"			// has everything you need

using namespace std;

int
main(int argc, char* argv[]) {
	if (argc != 2) {
		cerr << "Usage: " << argv[0] << " art-obj-file" << endl;
		exit(0);
	}
	{
		// test if file is valid
		ifstream f(argv[1], ios_base::binary);
		if (!f.good()) {
			cerr << "Error opening object file \"" << argv[1]
				<< "\"." << endl;
			exit(1);
		}
		f.close();
	}
	string fname(argv[1]);

//	persistent_object_manager::dump_registered_type_map(cerr);
/***
	The following line is required to make Appls's gcc think
	that linking in the majority of libart++.la is required 
	in the executable.  
	Without it the only reference to the module type is through
	a pointer-class, which is insufficient.  
***/
	{ entity::module bogus("please link modules from libart++.la"); }

	persistent_object_manager::dump_reconstruction_table = true;
	persistent::warn_unimplemented = true;	// for verbosity

	excl_ptr<entity::module> the_module;
try {
	the_module = persistent_object_manager::load_object_from_file
			<entity::module>(fname);
}
catch (...) {
	return 1;
}
	the_module->dump(cerr);

	// global will delete itself (recursively)
	return 0;
}

