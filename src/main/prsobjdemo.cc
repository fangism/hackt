/**
	\file "main/prsobjdemo.cc"
	Unrolls an object file, saves it to another object file.  

	$Id: prsobjdemo.cc,v 1.4.14.1 2005/11/07 08:55:11 fang Exp $
 */

#include <iostream>
#include <list>

// this needs to be first because it includes "util/hash_specializations.h"
#include "Object/module.tcc"	// for template method definitions
#include "main/prsobjdemo.h"
#include "main/program_registry.h"
#include "main/main_funcs.h"
#include "Object/def/process_definition.h"

// using declarations
namespace ART {
#include "util/using_ostream.h"
using std::list;
using util::memory::never_ptr;		// never-delete pointer
using entity::process_definition;

//=============================================================================
class prsobjdemo::options {
	// none
};	// end class options

//=============================================================================
// class prsobjdemo static initializers

const char
prsobjdemo::name[] = "prsobjdemo";

const char
prsobjdemo::brief_str[] = "manipulates prs in process definitions";

const size_t
prsobjdemo::program_id = register_hackt_program_class<prsobjdemo>();

//=============================================================================
prsobjdemo::prsobjdemo() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
prsobjdemo::main(int argc, char* argv[], const global_options&) {
	if (argc != 3) {
		usage();
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
		proc_list_type::const_iterator i(proc_defs.begin());
		const proc_list_type::const_iterator e(proc_defs.end());
		for ( ; i!=e; i++) {
			(*i)->expand_prs_complements();
//			(*i)->compact_prs_references();
		}
	}

	save_module(*the_module, argv[2]);
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
prsobjdemo::usage(void) {
	cerr << name << ": manipulates prs in process definitions." << endl;
	cerr << "usage: " << name <<
		" <hackt-obj-infile> <hackt-obj-outfile>" << endl;
}

//=============================================================================

}	// end namespace ART

