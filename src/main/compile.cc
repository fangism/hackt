/**
	\file "main/compile.cc"
	Converts ART source code to an object file (pre-unrolled).
	This file was born from "art++2obj.cc" in earlier revision history.

	$Id: compile.cc,v 1.6.2.1 2005/11/07 08:55:11 fang Exp $
 */

#include <iostream>
#include <list>
#include <string>
#include "main/program_registry.h"
#include "main/compile.h"
#include "main/main_funcs.h"
#include "lexer/file_manager.h"
#include "util/getopt_portable.h"
#include "util/dirent.h"

namespace ART {
#include "util/using_ostream.h"
using util::memory::excl_ptr;
using entity::module;
using std::list;
using std::string;
using lexer::file_manager;

//=============================================================================
/**
	Options for compile phase.  
 */
class compile::options {
public:
	typedef	list<string>			include_paths_type;
	bool					dump;
	include_paths_type			include_paths;

	options() : dump(false), include_paths() { }

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
	if (parse_command_options(argc, argv, opt)) {
		usage();
		return 1;
	}
	/***
		Now would be a good time to add include paths 
		to the parser's file manager.  
		Don't use a global file_manager, use local variables
		and pass as arguments, to allow these main subprograms
		to be re-entrant.  
		Q: Should file_manager be a member of module?
	***/
	file_manager fm;
{
	typedef	options::include_paths_type::const_iterator	const_iterator;
	const_iterator i(opt.include_paths.begin());
	const const_iterator e(opt.include_paths.end());
	for ( ; i!=e; i++) {
		const string& s(*i);
		// check if path exists, otherwise, don't bother adding...
		if (util::dir_exists(s.c_str())) {
			fm.add_path(s);
		}
	}
}

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
	static const char* optstring = "+dhI:";
	int c;
	while ((c = getopt(argc, argv, optstring)) != -1) {
	switch (c) {
	case 'd':
		opt.dump = true;
		break;
	case 'h':
		return 1;
	case 'I':
		// no need to check validity of paths yet
		opt.include_paths.push_back(optarg);
		break;
	case ':':
		cerr << "Expected but missing non-option argument." << endl;
		return 1;
	case '?':
		unknown_option(optopt);
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
	cerr << "\t-d: produces text dump of compiled module" << endl <<
		"\t-h: gives this usage messsage" << endl <<
		"\t-I <path> : adds include path (repeatable)" << endl;
	cerr << "\tIf output target is not given, module will not be saved."
		<< endl;
}

//=============================================================================

}	// end namespace ART

