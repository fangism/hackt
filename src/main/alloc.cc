/**
	\file "main/alloc.cc"
	Allocates global unique state.  

	$Id: alloc.cc,v 1.7 2007/09/13 01:14:05 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include <cstring>

#include "main/alloc.h"
#include "main/create.h"
#include "main/compile_options.h"
#include "main/program_registry.h"
#include "util/stacktrace.h"
#include "main/main_funcs.h"
#include "main/global_options.h"
#include "util/persistent_object_manager.h"

namespace HAC {

using util::persistent;
using util::persistent_object_manager;

#include "util/using_ostream.h"

//=============================================================================
// class alloc static initializers

const char
alloc::name[] = "alloc";

const char
alloc::brief_str[] = "Globally allocates unique state for simulation";

#ifndef	WITH_MAIN
const size_t
alloc::program_id = register_hackt_program_class<alloc>();
#endif

//=============================================================================
alloc::alloc() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
alloc::main(const int _argc, char* argv[], const global_options&) {
	int argc = _argc;
	options opt;
	// re-use create's options
	if (create::parse_command_options(argc, argv, opt)) {
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
	if (the_module->is_allocated()) {
		cerr << "Module is already allocated, skipping..." << endl;
	} else {
		STACKTRACE("main: try allocing.");
		if (!the_module->allocate_unique().good) {
			cerr << "ERROR in allocating.  Aborting." << endl;
			return 1;
		}
//		the_module->dump(cerr);
	}

	persistent_object_manager::dump_reconstruction_table = false;
	save_module(*the_module, argv[2]);
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
alloc::usage(void) {
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
	return HAC::alloc::main(argc, argv, g);
}
#endif	// WITH_MAIN

