/**
	\file "art++2obj.cc"
	Converts ART source code to an object file (pre-unrolled).

	$Id: art++2obj.cc,v 1.11 2005/05/20 19:28:30 fang Exp $
 */

#include <iostream>
#include "main/main_funcs.h"
#include "util/getopt_portable.h"

#include "util/using_ostream.h"

using util::memory::excl_ptr;
using ART::entity::module;
using namespace ART;

static	bool dump = false;
static	int parse_command_options(int, char*[]);
static	void usage(void);

//=============================================================================
int
main(int argc, char* argv[]) {
	if (parse_command_options(argc, argv))
		return 1;
	int index = optind;
	if (argc -optind > 2 || argc -optind <= 0) {
		usage();
		return 0;
	}
	argv += optind;		// shift
	FILE* f = open_source_file(argv[0]);
	if (!f)	return 1;
	if (argc -optind >= 2) {
		if (!check_file_writeable(argv[1]).good)
			return 1;
	}
	const excl_ptr<module> mod =
		parse_and_check(argv[0]);
	if (!mod)	return 1;
	if (argc -optind >= 2) {
		// save_module(*mod, argv[1]);
		save_module_debug(*mod, argv[1]);
	}
	if (dump)
		mod->dump(cerr);

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
	}       // end switch
	}       // end while
	return 0;
}

//-----------------------------------------------------------------------------
static
void
usage(void) {
	cerr << "art++2obj: compiles input file to module object file" << endl;
	cerr << "usage: art++2obj [-dh] <art-source-file> [art-obj-file]" << endl;
	cerr << "\t-d: produces text dump of compiled module" << endl
		<< "\t-h: gives this usage messsage" << endl;
	cerr << "\tIf output target is not given, module will not be saved."
		<< endl;
}

