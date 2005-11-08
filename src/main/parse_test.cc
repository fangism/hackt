/**
	\file "main/parse_test.cc"
	Just compiles ART source without writing object out.  
	Performs syntactic and semantic checking (with limited
	static analysis) and performs a pseudo persistent object
	write-out and read-in.
	This file was born out of "art_main.cc" in earlier history.  

	$Id: parse_test.cc,v 1.4.14.1 2005/11/08 05:09:45 fang Exp $
 */

#include <iostream>
#include "main/parse_test.h"
#include "main/program_registry.h"
#include "main/main_funcs.h"
#include "util/getopt_portable.h"

#define ENABLE_STACKTRACE		0

#include "util/using_ostream.h"
#include "util/stacktrace.h"

namespace ART {

using util::memory::excl_ptr;
using entity::module;

//=============================================================================
class parse_test::options {
public:
	bool dump;

	options() : dump(false) { }

};	// end class options

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
	\param argc number of command-line arguments.
	\param argv the array of string arguments.
	\param gopt currently unused.
 */
int
parse_test::main(const int argc, char* argv[],
		const global_options& gopt) {
	STACKTRACE_VERBOSE;
	options opt;		// default options
	if (parse_command_options(argc, argv, opt))
		return 1;
	const int index = optind;
	// if no file given, read from stdin
	const excl_ptr<module> mod =
		parse_and_check((index != argc) ? argv[index] : NULL);
	if (!mod)
		return 1;
	good_bool g(self_test_module(*mod));
	INVARIANT(g.good);
	if (opt.dump)
		mod->dump(cout);

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
	static const char optstring[] = "+dh";
	int c;
	while ((c = getopt(argc, argv, optstring)) != -1) {
	switch (c) {
	case 'd':
		opt.dump = true;
		break;
	case 'h':
		usage();
		return 1;
	case '?':
		unknown_option(optopt);
		usage();
		return 1;
		// exit(1);
	default:
		abort();
	}	// end switch
	}	// end while
	return 0;
}

//-----------------------------------------------------------------------------
void
parse_test::usage(void) {
	cout << "parse_test: parse and compile input file, and run self-test."
		<< endl
		<< "usage: parse_test [-dh] [file]" << endl
		<< "\t-d: produces text dump of compiled module" << endl
		<< "\t-h: gives this usage messsage" << endl
		<< "\tif no input file is given, then reads from stdin."
		<< endl;
}

//=============================================================================

}	// end namespace ART

