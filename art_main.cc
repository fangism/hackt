// "art_main.cc"

#include <iostream>
#include <fstream>
#include "art++.h"			// has everything you need

int
main(int argc, char* argv[]) {
	excl_ptr<parser::node> root;		///< root of the syntax tree
	never_const_ptr<entity::object> top;	///< root type-checked object
	excl_ptr<entity::name_space> global(new name_space(""));
	excl_ptr<parser::context> the_context(new context(global));

		yyparse();
DEBUG(DEBUG_BASIC, 
	cerr << "parsing successful... tree built, on to type-checking!" 
		<< endl;
)
	root = excl_ptr<parser::node>(yyval._root_body);
if (root) {
DEBUG(DEBUG_BASIC, 
	root->what(cerr << "root is a ") << endl;	// what's our top root?
	cerr << "----------------------------------------------------";
)

	// type-check, build a useful manipulable art object, and return it
	// the symbol tables will selectively retain info from the syntax tree
	// need to build global table first, then pass it in context
	top = root->check_build(the_context);	// useless return value
}	// end if (root)
DEBUG(DEBUG_BASIC, cerr << endl)

	if (top) {
		assert(global == the_context->get_current_namespace());
DEBUG(DEBUG_BASIC, top->dump(cerr))
	}

//	global->dump(cerr);
	{
#if 0
	const string fname("fang.artobj");	// name of file
	excl_ptr<entity::name_space> global2 =
		persistent_object_manager::self_test(fname, global);
#else
//	persistent_object_manager::dump_reconstruction_table = true;
	excl_ptr<entity::name_space> global2 =
		persistent_object_manager::self_test_no_file(global);
#endif
	assert(global2);
//	global2->dump(cerr);			// should match
	}

	// massive recursive deletion of syntax tree, reclaim memory
	// root will delete itself (also recursively)
	// global will delete itself (also recursively)
	return 0;
}

