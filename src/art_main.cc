/**
	\file "art_main.cc"
	Just compiles ART source without writing object out.  
	Performs syntactic and semantic checking (with limited
	static analysis) and performs a pseudo persistent object
	write-out and read-in.

	$Id: art_main.cc,v 1.15 2005/05/20 19:28:30 fang Exp $
 */

#include <iostream>
#include "main/main_funcs.h"
#include "util/getopt_portable.h"

#define ENABLE_STACKTRACE		0

#include "util/using_ostream.h"
#include "util/stacktrace.h"

using util::memory::excl_ptr;
using ART::entity::module;
using namespace ART;

static	bool dump = false;
static	int parse_command_options(int, char*[]);
static	void usage(void);

//=============================================================================
int
main(int argc, char* argv[]) {
	STACKTRACE_VERBOSE;
	if (parse_command_options(argc, argv))
		return 1;
	int index = optind;
	// if no file given, read from stdin
	const excl_ptr<module> mod =
		parse_and_check((index != argc) ? argv[index] : NULL);
	if (!mod)
		return 1;
	good_bool g(self_test_module(*mod));
	INVARIANT(g.good);
	if (dump)
		mod->dump(cout);

	// massive recursive deletion of syntax tree, reclaim memory
	// root will delete itself (also recursively)
	// global will delete itself (also recursively)
	return 0;
}

//-----------------------------------------------------------------------------
/**
	\return 0 if is ok to continue, anything else will signal early
		termination, an error will cause exit(1).
 */
static
int
parse_command_options(int argc, char* argv[]) {
	STACKTRACE_VERBOSE;
	static const char* optstring = "dh";
	int c;
	while ((c = getopt(argc, argv, optstring)) != -1) {
	switch (c) {
	case 'd':
		dump = true;
		break;
	case 'h':
		usage();
		return 1;
	case '?':
		unknown_option(optopt);
		usage();
		exit(1);
	default:
		abort();
	}	// end switch
	}	// end while
	return 0;
}

//-----------------------------------------------------------------------------
static
void
usage(void) {
	cout << "artc: parse and compile input file, and run self-test." << endl
		<< "usage: artc [-dh] [file]" << endl
		<< "\t-d: produces text dump of compiled module" << endl
		<< "\t-h: gives this usage messsage" << endl
		<< "\tif no input file is given, then reads from stdin."
		<< endl;
}

