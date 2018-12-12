/**
	\file "main/create.cc"
	Unrolls an object file, saves it to another object file.  

	$Id: create.cc,v 1.10 2010/04/02 22:18:58 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include <cstring>
#include <vector>

#include "main/create.hh"
#include "main/compile_options.hh"
#include "main/program_registry.hh"
#include "util/getopt_portable.h"		// for optind
#include "util/stacktrace.hh"
#include "main/main_funcs.hh"
#include "main/global_options.hh"
#include "util/persistent_object_manager.hh"
#include "util/memory/count_ptr.tcc"

namespace HAC {

using util::persistent;
using util::persistent_object_manager;

#include "util/using_ostream.hh"

//=============================================================================
// class create static initializers

const char
create::name[] = "create";

const char
create::brief_str[] = "Creates unique state for aliased objects for simulation";

#ifndef	WITH_MAIN
const size_t
create::program_id = register_hackt_program_class<create>();
#endif

//=============================================================================
create::create() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
create::main(const int _argc, char* argv[], const global_options&) {
	STACKTRACE_VERBOSE;
	int argc = _argc;
//	cout << "optind = " << optind << endl;	// is initialized to 1
	options opt;
	if (parse_command_options(argc, argv, opt)) {
		usage();
		return 1;
	}
	argc -= optind -1;
	argv += optind -1;
	if (argc != 3) {
		usage();
		return 1;
	}
	const char* const input = argv[1];
	const char* const output = argv[2];
	if (!strcmp(input, output)) {
		cerr << "The output objfile should not be the same as the input."
			<< endl;
		return 1;
	}
if (opt.compile_input) {
	opt.source_file = input;
} else {
	if (!check_object_loadable(input).good)
		return 1;
}
	if (!check_file_writeable(output).good)
		return 1;

	// TODO: move this into the options class
	persistent_object_manager::dump_reconstruction_table = false;
	persistent::warn_unimplemented = true;	// for verbosity

	count_ptr<module> the_module;
if (opt.compile_input) {
	the_module = parse_and_check(opt.source_file.c_str(), opt);
} else {
	the_module = load_module(input);
}
	if (!the_module)
		return 1;

//	the_module->dump(cerr);
	if (the_module->is_created()) {
		cerr << "Module is already created, skipping..." << endl;
	} else {
		STACKTRACE("main: try creating.");
		if (!the_module->create_unique().good) {
			cerr << "ERROR in creating.  Aborting." << endl;
			return 1;
		}
//		the_module->dump(cerr);
	}

	persistent_object_manager::dump_reconstruction_table = false;
	save_module(*the_module, output);
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
// relocated to main/main_funcs.cc for re-use.
int
create::parse_command_options(const int argc, char* argv[], options& opt);
***/

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
create::usage(void) {
	create_usage(name, cerr);
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
	return HAC::create::main(argc, argv, g);
}
#endif	// WITH_MAIN

