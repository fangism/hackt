/**
	\file "main/objdump.cc"
	Just dumps an object file to human-readable (?) output.  
	Useful for testing object file integrity.  
	This file came from "artobjdump.cc" in a previous life.  

	$Id: objdump.cc,v 1.12 2010/04/07 00:13:02 fang Exp $
 */

#include <iostream>
#include "common/config.h"
#include "main/objdump.h"
#include "main/program_registry.h"
#include "main/main_funcs.h"
#include "main/global_options.h"
#include "Object/type/process_type_reference.h"
#include "util/getopt_mapped.h"
#include "util/using_ostream.h"

namespace HAC {
//=============================================================================
struct objdump::options {
	bool			show_table_of_contents;
	bool			show_hierarchical_definitions;
	bool			show_global_allocate_table;
	bool			auto_allocate;
	bool			use_referenced_type_instead_of_top_level;
	bool			help_only;
	string			named_process_type;

	options() : show_table_of_contents(true), 
		show_hierarchical_definitions(true),
		show_global_allocate_table(false),
		auto_allocate(false),
		use_referenced_type_instead_of_top_level(false),
		help_only(false),
		named_process_type() { }
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
	count_ptr<module> the_module;	// non-const b/c may auto-allocate
if (opt.show_table_of_contents) {
	the_module = load_module_debug(objfile);
} else {
	the_module = load_module(objfile);
}
	if (!the_module)
		return 1;

	// this should really go to std::cout...
	ostream& o(cout);
	count_ptr<const process_type_reference> rpt;
if (opt.use_referenced_type_instead_of_top_level) {
	// create this in advance before dumping definitions/footprints
	rpt = parse_and_create_complete_process_type(
		opt.named_process_type.c_str(), *the_module);
	if (!rpt) {
		return 1;
	}
}
if (opt.show_hierarchical_definitions) {
	the_module->dump_definitions(o);
}
	if (opt.auto_allocate && !the_module->allocate_unique().good) {
		cerr << "Error allocating global instances." << endl;
		return 1;
	}
if (opt.show_global_allocate_table) {
	const count_ptr<const module> m(the_module);
	o << "Globally allocated state:" << endl;
if (opt.use_referenced_type_instead_of_top_level) {
	NEVER_NULL(rpt);	// already checked
	rpt->lookup_footprint()->dump_allocation_map(o) << endl;
} else {
	m->get_footprint().dump_allocation_map(o) << endl;
}
}
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
objdump::parse_command_options(const int argc, char* argv[], options& o) {
	static const char optstring[] = "+aAcCdDgGhmt:Mv";
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
	case 'm':
		o.auto_allocate = false;
		break;
	case 'M':
		o.auto_allocate = true;
		break;
	case 't':
		o.use_referenced_type_instead_of_top_level = true;
		o.named_process_type = optarg;
		break;
	case 'v':
		config::dump_all(cout);
		exit(0);
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
"\t-h : help on usage and exit\n"
"\t-m : suppress automatic global instance allocation\n"
"\t-M : enable automatic global instance allocation\n"
"\t-t type : treat TYPE as top-level\n"
"\t-v : print version and exit\n"
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

