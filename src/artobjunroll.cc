/**
	\file "artobjunroll.cc"
	Unrolls an object file, saves it to another object file.  

	$Id: artobjunroll.cc,v 1.10 2005/02/27 22:54:07 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include <fstream>
#include "art++.h"			// has everything you need

#include "stacktrace.h"

#include "using_ostream.h"
USING_STACKTRACE

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

	persistent_object_manager::dump_reconstruction_table = false;
	persistent::warn_unimplemented = true;	// for verbosity

	excl_ptr<entity::module> the_module;
try {
	the_module = persistent_object_manager::load_object_from_file(ifname)
			.is_a_xfer<entity::module>();
}
catch (...) {
	// possibly empty file error from
	// persistent_object_manager::load_header()
	return 1;
}

//	the_module->dump(cerr);
	if (the_module->is_unrolled()) {
		cerr << "Module is already unrolled, skipping..." << endl;
	} else {
		try {
			STACKTRACE("main: try unrolling.");
			the_module->unroll_module();
		}
		catch (...) {
			cerr << "ERROR in unrolling.  Aborting." << endl;
			// although an empty unroll file was already created
			return 1;
		}
//		the_module->dump(cerr);
	}

	persistent_object_manager::dump_reconstruction_table = false;
	persistent_object_manager::save_object_to_file(ofname, *the_module);

	// global will delete itself (recursively)
	return 0;
}

#undef	ENABLE_STACKTRACE

