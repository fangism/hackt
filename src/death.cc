/**
	\file "death.cc"
	Just compiles ART source without writing object out.  
	Performs syntactic and semantic checking (with limited
	static analysis) and performs a pseudo persistent object
	write-out and read-in.

	$Id: death.cc,v 1.1.2.1 2005/02/22 03:33:18 fang Exp $
 */

// #include <iostream>
// #include <fstream>
#include "death.h"			// has everything you need

int
main(int argc, char* argv[]) {
#if 0
	excl_ptr<parser::node> root;		///< root of the syntax tree
	never_ptr<const entity::object> top;	///< root type-checked object
	entity::module the_module("-stdin-");
	parser::context the_context(the_module);

	artxx_parse();

#if USING_YACC
	root = excl_ptr<parser::node>(artxx_val._root_body);
#elif USING_BISON
	root = AST_root;
#endif
if (root) {

	// type-check, build a useful manipulable art object, and return it
	// the symbol tables will selectively retain info from the syntax tree
	// need to build global table first, then pass it in context
	top = root->check_build(never_ptr<parser::context>(&the_context));
}	// end if (root)
	if (top) {
		assert(the_module.get_global_namespace() ==
			the_context.get_current_namespace());
	}

//	global->dump(cerr);
	{
//	persistent_object_manager::dump_reconstruction_table = true;
	assert(persistent_object_manager::self_test_no_file(the_module));
//	the_module.dump(cerr);
	}
#endif
	// massive recursive deletion of syntax tree, reclaim memory
	// root will delete itself (also recursively)
	// global will delete itself (also recursively)
	return 0;
}

