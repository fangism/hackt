// "art++2obj.cc"

#include <iostream>
#include <fstream>
#include "art++.h"			// has everything you need

int
main(int argc, char* argv[]) {
	if (argc != 3 && argc != 2) {
		cerr << "Usage: " << argv[0] << " art-source-file "
			"[art-obj-file]" << endl;
		exit(0);
	}
	{
		// test if source is valid
		ifstream f(argv[1], ios_base::binary);
		if (!f.good()) {
			cerr << "Error opening source file \"" << argv[1]
				<< "\"." << endl;
			exit(1);
		}
		f.close();
		// I hate old stdio... FILE* yyin;
		yyin = fopen(argv[1], "r");
		assert(yyin);
	}
	if (argc >= 3) {
		// test if file is valid
		ofstream f(argv[2], ios_base::binary | ios_base::app);
		if (!f.good()) {
			cerr << "Error opening object file \"" << argv[2]
				<< "\"." << endl;
			exit(1);
		}
		f.close();
	}


	excl_ptr<parser::node> root;		///< root of the syntax tree
	excl_ptr<entity::name_space> global(new name_space(""));
	excl_ptr<parser::context> the_context(new context(global));

	// make sure yyin is set
	yyparse();
	fclose(yyin);
	root = excl_ptr<parser::node>(yyval._root_body);
if (root) {
	// type-check, build a useful manipulable art object, and return it
	// the symbol tables will selectively retain info from the syntax tree
	// need to build global table first, then pass it in context
	root->check_build(the_context);		// useless return value
} else {
	cerr << "Empty file." << endl;
	exit(0);
}

if (argc >= 3) {
	const string fname(argv[2]);	// name of file
	entity::object::warn_unimplemented = true;	// just for verbosity
	persistent_object_manager::dump_reconstruction_table = true;
	persistent_object_manager::save_object_to_file(fname, global);
}
	// else don't bother
	global->dump(cerr);

	// massive recursive deletion of syntax tree, reclaim memory
	// root will delete itself (also recursively)
	// global will delete itself (also recursively)
	return 0;
}

