/**
	\file "artobjunroll.cc"
	Unrolls an object file, saves it to another object file.  

	$Id: artobjunroll.cc,v 1.8 2004/12/02 01:38:38 fang Exp $
 */

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
			cerr << "Error opening object file \"" << argv[2]
				<< "\"." << endl;
			exit(1);
		}
		f.close();
	}
	const string ifname(argv[1]);
	const string ofname(argv[2]);

	// the following is needed to force linkage of modules from libart++
	{ entity::module bogus("please link modules from libart++.la"); }

	persistent_object_manager::dump_reconstruction_table = true;
	persistent::warn_unimplemented = true;	// for verbosity

	excl_ptr<entity::module> the_module =
		persistent_object_manager::load_object_from_file
			<entity::module>(ifname);

//	the_module->dump(cerr);
	the_module->unroll_module();
	the_module->dump(cerr);

	persistent_object_manager::save_object_to_file(ofname, *the_module);

	// global will delete itself (recursively)
	return 0;
}

