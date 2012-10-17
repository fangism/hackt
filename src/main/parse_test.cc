/**
	\file "main/parse_test.cc"
	Just compiles HAC source without writing object out.  
	Performs syntactic and semantic checking (with limited
	static analysis) and performs a pseudo persistent object
	write-out and read-in.
	This file was born out of "art_main.cc" in earlier history.  

	$Id: parse_test.cc,v 1.8 2007/03/16 07:07:24 fang Exp $
 */

#include <iostream>
#include "main/parse_test.hh"
#include "main/program_registry.hh"
#include "main/main_funcs.hh"
#include "main/compile.hh"
#include "main/compile_options.hh"
#include "util/getopt_portable.h"

#define ENABLE_STACKTRACE		0

#include "util/using_ostream.hh"
#include "util/stacktrace.hh"

extern HAC::lexer::file_manager
hackt_parse_file_manager;

namespace HAC {

using util::memory::count_ptr;
using entity::module;

//=============================================================================
const char
parse_test::name[] = "parse_test";

const char
parse_test::brief_str[] = "Parses input file and runs self-test.";

const size_t
parse_test::program_id = register_hackt_program_class<parse_test>();

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
parse_test::parse_test() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: also accept same compile flags as compile module.
	TODO: clean up file_manager after done?
	Does not generate make-depend.  
	\param argc number of command-line arguments.
	\param argv the array of string arguments.
	\param gopt currently unused.
 */
int
parse_test::main(const int argc, char* argv[],
		const global_options& gopt) {
	STACKTRACE_VERBOSE;
	options opt;		// default options
	if (parse_command_options(argc, argv, opt)) {
		usage();
		return 1;
	}
	opt.export_include_paths(hackt_parse_file_manager);

	const int index = optind;
	// if no file given, read from stdin
	if (index != argc) {
		opt.source_file = argv[index];
	}
	const count_ptr<const module>
		mod(parse_and_check((index != argc) ? argv[index] : NULL, opt));
	if (!mod)
		return 1;
	good_bool g(self_test_module(*mod));
	INVARIANT(g.good);
	if (opt.dump_module) {
		mod->dump(cout);
	}
	// massive recursive deletion of syntax tree, reclaim memory
	// root will delete itself (also recursively)
	// global will delete itself (also recursively)
	return 0;
}

//-----------------------------------------------------------------------------
/**
	\param o program options set by this routine.  
	\return 0 if is ok to continue, anything else will signal early
		termination, an error will cause exit(1).
	NOTE: the getopt option string begins with '+' to enforce
		POSIXLY correct termination at the first non-option argument.  
 */
int
parse_test::parse_command_options(const int argc, char* argv[],
		options& opt) {
	STACKTRACE_VERBOSE;
	return compile::parse_command_options(argc, argv, opt);
}

//-----------------------------------------------------------------------------
void
parse_test::usage(void) {
	cout << "parse_test: parse and compile input file, and run self-test."
		<< endl;
	compile::usage();
}

//=============================================================================

}	// end namespace HAC

