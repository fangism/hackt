// "artobjdump.cc"
// just dumps an object file

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

	persistent_object_manager::dump_reconstruction_table = true;
	entity::object::warn_unimplemented = true;	// for verbosity
	excl_ptr<entity::name_space> global =
		persistent_object_manager::load_object_from_file(fname);

	assert(global);

	global->dump(cerr);

	// global will delete itself (recursively)
	return 0;
}

