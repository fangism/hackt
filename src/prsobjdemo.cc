/**
	\file "prsobjdemo.cc"
	Unrolls an object file, saves it to another object file.  

	$Id: prsobjdemo.cc,v 1.1.16.1 2005/07/21 05:34:54 fang Exp $
 */

#include <iostream>
#include <list>

#include "main/main_funcs.h"
#include "Object/art_object_module.tcc"	// for template method definitions
#include "Object/def/process_definition.h"

// using declarations
using namespace ART;
#include "util/using_ostream.h"
using std::list;
using util::memory::never_ptr;		// never-delete pointer
using ART::entity::process_definition;

//=============================================================================
int
main(int argc, char* argv[]) {
	if (argc != 3) {
		cerr << argv[0] << ": manipulates prs in process definitions."
			<< endl;
		cerr << "Usage: " << argv[0] <<
			" <art-obj-infile> <art-obj-outfile>" << endl;
		return 1;
	}
	if (!strcmp(argv[1], argv[2])) {
		cerr << "The output objfile should not be the same as the input."
			<< endl;
		return 1;
	}
	if (!check_object_loadable(argv[1]).good)
		return 1;
	if (!check_file_writeable(argv[2]).good)
		return 1;

	excl_ptr<module> the_module = load_module(argv[1]);
	if (!the_module)
		return 1;

	// lexical scope for convenience
	{
		typedef	list<never_ptr<process_definition> > proc_list_type;
		// the_module->do_something();
		// this is written like a standard pass, and may end up
		// in a library of useful module passes one day.
		proc_list_type proc_defs;
		the_module->collect(proc_defs);	// is a template method call
		proc_list_type::const_iterator i = proc_defs.begin();
		const proc_list_type::const_iterator e = proc_defs.end();
		for ( ; i!=e; i++) {
			(*i)->expand_prs_complements();
//			(*i)->compact_prs_references();
		}
	}

	save_module(*the_module, argv[2]);
	return 0;
}
//=============================================================================

