/**
	\file "art_main.cc"
	Just compiles ART source without writing object out.  
	Performs syntactic and semantic checking (with limited
	static analysis) and performs a pseudo persistent object
	write-out and read-in.

	$Id: art_main.cc,v 1.6 2004/11/30 01:24:57 fang Exp $
 */

#include <iostream>
#include <fstream>
#include "art++.h"			// has everything you need

int
main(int argc, char* argv[]) {
	excl_ptr<parser::node> root;		///< root of the syntax tree
	never_ptr<const entity::object> top;	///< root type-checked object
	entity::module the_module("-stdin-");
	parser::context the_context(the_module);

	artxx_parse();

DEBUG(DEBUG_BASIC, 
	cerr << "parsing successful... tree built, on to type-checking!" 
		<< endl;
)
#if USING_YACC
	root = excl_ptr<parser::node>(artxx_val._root_body);
#elif USING_BISON
	root = AST_root;
#endif
if (root) {
DEBUG(DEBUG_BASIC, 
	root->what(cerr << "root is a ") << endl;	// what's our top root?
	cerr << "----------------------------------------------------";
)

	// type-check, build a useful manipulable art object, and return it
	// the symbol tables will selectively retain info from the syntax tree
	// need to build global table first, then pass it in context
	top = root->check_build(never_ptr<parser::context>(&the_context));
}	// end if (root)
DEBUG(DEBUG_BASIC, cerr << endl)

	if (top) {
		assert(the_module.get_global_namespace() ==
			the_context.get_current_namespace());
DEBUG(DEBUG_BASIC, top->dump(cerr))
	}

//	global->dump(cerr);
	{
//	persistent_object_manager::dump_reconstruction_table = true;
	assert(persistent_object_manager::self_test_no_file(the_module));
//	the_module.dump(cerr);
	}

	// massive recursive deletion of syntax tree, reclaim memory
	// root will delete itself (also recursively)
	// global will delete itself (also recursively)
	return 0;
}

