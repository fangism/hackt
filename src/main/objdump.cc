/**
	\file "main/objdump.cc"
	Just dumps an object file to human-readable (?) output.  
	Useful for testing object file integrity.  
	This file came from "artobjdump.cc" in a previous life.  

	$Id: objdump.cc,v 1.8 2009/02/16 01:38:30 fang Exp $
 */

#include <iostream>
#include "main/objdump.h"
#include "main/program_registry.h"
#include "main/main_funcs.h"
#include "main/global_options.h"
#include "util/getopt_mapped.h"
#include "util/using_ostream.h"

namespace HAC {
//=============================================================================
struct objdump::options {
	bool			show_table_of_contents;
	bool			show_hierarchical_definitions;
	bool			show_global_allocate_table;
	bool			help_only;

	options() : show_table_of_contents(true), 
		show_hierarchical_definitions(true),
		show_global_allocate_table(true),
		help_only(false) { }
};	// end class options

//=============================================================================
// class objdump static initializers

const char
objdump::name[] = "objdump";

const char
objdump::brief_str[] = "Dumps HACKT object semi-human-readably to stderr.";

#ifndef	WITH_MAIN
const size_t
objdump::program_id = register_hackt_program_class<objdump>();
#endif

//=============================================================================
// class objdump member definitions

objdump::objdump() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
objdump::main(int argc, char* argv[], const global_options&) {
	options opt;
	if (parse_command_options(argc, argv, opt)) {
		cerr << "Error in command invocation." << endl;
		usage();
		return 1;
	}
	if (opt.help_only) {
		usage();
		return 0;
	}
	if (optind +1 != argc) {
		cerr << "Error: Exactly one non-option argument "
			"allowed/required." << endl;
		usage();
		return 1;
	}
	const char* objfile = argv[optind];
	if (!check_object_loadable(objfile).good)
		return 1;
	count_ptr<const module> the_module;
if (opt.show_table_of_contents) {
	the_module = load_module_debug(objfile);
} else {
	the_module = load_module(objfile);
}
	if (!the_module)
		return 1;

	// this should really go to std::cout...
if (opt.show_hierarchical_definitions) {
	the_module->dump_definitions(cerr);
}
if (opt.show_global_allocate_table) {
	the_module->dump_instance_map(cerr);
}
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
objdump::parse_command_options(const int argc, char* argv[], options& o) {
	static const char optstring[] = "+aAcCdDgGh";
	int c;
	while ((c = getopt(argc, argv, optstring)) != -1) {
	switch (c) {
	case 'a':
		o.show_table_of_contents = false;
		o.show_hierarchical_definitions = false;
		o.show_global_allocate_table = false;
		break;
	case 'A':
		o.show_table_of_contents = true;
		o.show_hierarchical_definitions = true;
		o.show_global_allocate_table = true;
		break;
	case 'c':
		o.show_table_of_contents = false;
		break;
	case 'C':
		o.show_table_of_contents = true;
		break;
	case 'd':
		o.show_hierarchical_definitions = false;
		break;
	case 'D':
		o.show_hierarchical_definitions = true;
		break;
	case 'g':
		o.show_global_allocate_table = false;
		break;
	case 'G':
		o.show_global_allocate_table = true;
		break;
	case 'h':
		o.help_only = true;
		break;
	case ':':
		cerr << "Expected but missing option-argument." << endl;
		return 1;
	case '?':
		util::unknown_option(cerr, optopt);
		return 1;
	default:
		THROW_EXIT;
	}
	}
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
objdump::usage(void) {
	cerr << "usage: " << name << " [options] <hackt-obj-file>" << endl;
	cerr <<
"options:\n"
"\t-a : all sections off\n"
"\t-A : all sections on (default)\n"
"\t-c : hide table of contents\n"
"\t-C : show table of contents\n"
"\t-d : hide hierarchical definitions\n"
"\t-D : show hierarchical definitions\n"
"\t-g : hide global allocation tables\n"
"\t-G : show global allocation tables\n"
"\t-h : help on usage"
	<< endl;
}

//=============================================================================

}	// end namespace HAC

#ifdef	WITH_MAIN
/**
	Assumes no global hackt options.  
 */
int
main(const int argc, char* argv[]) {
	const HAC::global_options g;
	return HAC::objdump::main(argc, argv, g);
}
#endif	// WITH_MAIN

