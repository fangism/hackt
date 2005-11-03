/**
	\file "main/compile.cc"
	Converts ART source code to an object file (pre-unrolled).
	This file was born from "art++2obj.cc" in earlier revision history.

	$Id: compile.cc,v 1.6 2005/11/03 07:52:06 fang Exp $
 */

#include <iostream>
#include "main/program_registry.h"
#include "main/compile.h"
#include "main/main_funcs.h"
#include "util/getopt_portable.h"

namespace ART {
#include "util/using_ostream.h"
using util::memory::excl_ptr;
using entity::module;

//=============================================================================
/**
	Options for compile phase.  
 */
class compile::options {
public:
	bool dump;

	options() : dump(false) { }

};	// end class options

//=============================================================================
// class compile static initializers

const char
compile::name[] = "compile";

const char
compile::brief_str[] = "Compiles HACKT source to object file.";

const size_t
compile::program_id = register_hackt_program_class<compile>();

//=============================================================================
compile::compile() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The main program for compiling source to object.  
 */
int
compile::main(const int argc, char* argv[], const global_options&) {
	options opt;
	if (parse_command_options(argc, argv, opt))
		return 1;
//	int index = optind;
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
	if (opt.dump)
		mod->dump(cerr);

	return 0;
}

//-----------------------------------------------------------------------------
/**
	\return 0 if is ok to continue, anything else will signal early
		termination, an error will cause exit(1).
	NOTE: the getopt option string begins with '+' to enforce
		POSIXLY correct termination at the first non-option argument.  
 */
int
compile::parse_command_options(const int argc, char* argv[], options& opt) {
	static const char* optstring = "+dh";
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
		// exit(1);
		return 1;
	default:
		abort();
	}       // end switch
	}       // end while
	return 0;
}

//-----------------------------------------------------------------------------
void
compile::usage(void) {
	cerr << "compile: compiles input file to module object file" << endl;
	cerr << "usage: compile [-dh] <hackt-source-file> [hackt-obj-file]"
		<< endl;
	cerr << "\t-d: produces text dump of compiled module" << endl
		<< "\t-h: gives this usage messsage" << endl;
	cerr << "\tIf output target is not given, module will not be saved."
		<< endl;
}

//=============================================================================

}	// end namespace ART

