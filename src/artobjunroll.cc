// "artobjunroll.cc"
// unrolls an object file, saves it to another object file

#include <iostream>
#include <fstream>
#include "art++.h"			// has everything you need

using namespace std;

int
main(int argc, char* argv[]) {
	if (argc != 3) {
		cerr << "Usage: " << argv[0] <<
			" art-obj-infile art-obj-outfile" << endl;
		exit(1);
	}
	{
		// test if in file is valid
		ifstream f(argv[1], ios_base::binary);
		if (!f.good()) {
			cerr << "Error opening object file \"" << argv[1]
				<< "\"." << endl;
			exit(1);
		}
		f.close();
	}
	{
		// test if out file is valid (permissions, etc.)
		ofstream f(argv[2], ios_base::binary);
		if (!f.good()) {
			cerr << "Error opening object file \"" << argv[1]
				<< "\"." << endl;
			exit(1);
		}
		f.close();
	}
	const string ifname(argv[1]);
	const string ofname(argv[2]);

	persistent_object_manager::dump_reconstruction_table = true;
	entity::object::warn_unimplemented = true;	// for verbosity

	excl_ptr<entity::module> the_module =
	persistent_object_manager::load_object_from_file(ifname);

//	the_module->dump(cerr);

	the_module->unroll();

#if 0
// ALL WRONG
	// what to do in case of error?  exit?
	global->unroll_params();
	global->unroll_instances();
	global->unroll_connections();
	global->dump(cerr);

	persistent_object_manager::save_object_to_file(ofname, global);
#endif

	// global will delete itself (recursively)
	return 0;
}

