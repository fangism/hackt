/**
	\file "art_main.cc"
	Just compiles ART source without writing object out.  
	Performs syntactic and semantic checking (with limited
	static analysis) and performs a pseudo persistent object
	write-out and read-in.

	$Id: art_main.cc,v 1.9.6.1 2005/02/24 06:16:42 fang Exp $
 */

#include <iostream>
#include <fstream>
#include "art++.h"			// has everything you need

#define ENABLE_STACKTRACE		0

#include "stacktrace.h"
USING_STACKTRACE

int
main(int argc, char* argv[]) {
	STACKTRACE_VERBOSE;
	excl_ptr<parser::root_body> root;		///< root of the syntax tree
	never_ptr<const entity::object> top;	///< root type-checked object
	entity::module the_module("-stdin-");
	parser::context the_context(the_module);

try {
	artxx_parse();
}
catch (...) {
	// parsing error
	return 1;
}

DEBUG(DEBUG_BASIC, 
	cerr << "parsing successful... tree built, on to type-checking!" 
		<< endl;
)
#if USING_YACC
	root = excl_ptr<parser::root_body>(artxx_val._root_body);
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
try {
	top = root->check_build(the_context);
}
catch (...) {
	// type-checking error
	return 1;
}
}	// end if (root)
DEBUG(DEBUG_BASIC, cerr << endl)

	if (top) {
		assert(the_module.get_global_namespace() ==
			the_context.get_current_namespace());
DEBUG(DEBUG_BASIC, top->dump(cerr))
	}

//	global->dump(cerr);
	{
#if 0
	STACKTRACE("MAIN-QUARANTINE");
	persistent_object_manager::dump_reconstruction_table = true;
#endif
#if 1
	const excl_ptr<entity::module> module_copy =
		persistent_object_manager::self_test_no_file(the_module);
	NEVER_NULL(module_copy);
#endif
//	the_module.dump(cerr);
	}

	// massive recursive deletion of syntax tree, reclaim memory
	// root will delete itself (also recursively)
	// global will delete itself (also recursively)
	return 0;
}

