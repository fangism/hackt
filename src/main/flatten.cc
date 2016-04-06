/**
	\file "main/flatten.cc"
	Converts HAC source code to an object file (pre-unrolled).
	This file was born from "art++2obj.cc" in earlier revision history.

	$Id: flatten.cc,v 1.13 2010/03/11 18:39:26 fang Exp $
 */

#include <iostream>
#include <list>
#include <string>
#include <map>
#include <cstdio>
#include "common/config.hh"
#include "main/program_registry.hh"
#include "main/flatten.hh"
#include "main/main_funcs.hh"
#include "main/compile_options.hh"
#include "main/global_options.hh"
#include "lexer/file_manager.hh"
#include "util/getopt_portable.h"
#include "util/getopt_mapped.hh"
// #include "util/dirent.hh"		// configured wrapper around <dirent.h>
#include "util/attributes.h"

extern HAC::lexer::file_manager
hackt_parse_file_manager;

namespace HAC {
#include "util/using_ostream.hh"
using std::list;
using std::string;
using lexer::file_manager;
using util::good_bool;

namespace lexer {
// from "lexer/hacflat-lex.ll" (and .cc derivatives, of course)
extern	void	flatten_with_wrappers(const bool);
}

//=============================================================================
/**
	Entry for options.  
 */
struct flatten::options_modifier_info {
	options_modifier			op;
	string					brief;

	options_modifier_info() : op(NULL), brief() { }

	options_modifier_info(const options_modifier o, const char* b) :
		op(o), brief(b) { }

	operator bool () const { return op; }

	good_bool
	operator () (options& o) const {
		NEVER_NULL(op);
		return (*op)(o);
	}

};	// end struct options_modifier_info

//=============================================================================
// class flatten static initializers

const char
flatten::name[] = "flatten";

const char
flatten::brief_str[] = "Flattens HAC source to single file (stdout).";

#ifndef	WITH_MAIN
const size_t
flatten::program_id = register_hackt_program_class<flatten>();
#endif

/**
	Options modifier map must be initialized before any registrations.  
 */
const flatten::options_modifier_map_type
flatten::options_modifier_map;

//=============================================================================
static const char default_options_brief[] = "Needs description.";

/**
	Options modification registration interface.  
 */
class flatten::register_options_modifier {
public:
	register_options_modifier(const string& optname, 
			const options_modifier om, 
			const char* b = default_options_brief) {
		options_modifier_map_type&
			omm(const_cast<options_modifier_map_type&>(
				options_modifier_map));
		NEVER_NULL(om);
		options_modifier_info& i(omm[optname]);
		INVARIANT(!i);
		i.op = om;
		i.brief = b;
	}
} __ATTRIBUTE_UNUSED__;	// end class register_options_modifier

//=============================================================================
// flatten::options_modifier declarations and definitions
// TODO: texinfo documentation!

static
good_bool
__flatten_dump_include_paths(flatten::options& o) {
	o.dump_include_paths = true;
	return good_bool(true);
}

static const flatten::register_options_modifier
__flatten_om_dump_include_paths("dump-include-paths",
	&__flatten_dump_include_paths,
	"dumps -I include paths as they are processed");

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
good_bool
__flatten_no_dump_include_paths(flatten::options& o) {
	o.dump_include_paths = false;
	return good_bool(true);
}

static const flatten::register_options_modifier
__flatten_om_no_dump_include_paths("no-dump-include-paths",
	&__flatten_no_dump_include_paths,
	"suppress feedback of -I include paths");

//-----------------------------------------------------------------------------
#if 0
static
good_bool
__flatten_dump_object_header(flatten::options& o) {
	o.dump_object_header = true;
	return good_bool(true);
}

static const flatten::register_options_modifier
__flatten_om_dump_object_header("dump-object-header",
	&__flatten_dump_object_header,
	"dumps persistent object header before saving");

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
good_bool
__flatten_no_dump_object_header(flatten::options& o) {
	o.dump_object_header = false;
	return good_bool(true);
}

static const flatten::register_options_modifier
__flatten_om_no_dump_object_header("no-dump-object-header",
	&__flatten_no_dump_object_header,
	"suppress persistent object header dump");
#endif

//=============================================================================
// class flatten method definitions

flatten::flatten() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The main program for compiling source to object.  
	TODO: clean up file manager after done?
 */
int
flatten::main(const int _argc, char* argv[], const global_options&) {
	int argc = _argc;
	options opt;
	if (parse_command_options(argc, argv, opt)) {
		usage();
		return 1;
	}
	argc -= optind;	// shift
	argv += optind;	// shift
	/***
		Now would be a good time to add include paths 
		to the parser's file manager.  
		Don't use a global file_manager, use local variables
		and pass as arguments, to allow these main subprograms
		to be re-entrant.  
		Q: Should file_manager be a member of module?
	***/
	opt.export_include_paths(hackt_parse_file_manager);

	// TODO: support multi-file flattening, or just defer to cat?
	if (argc > 1 || argc < 0) {
		usage();
		return 1;
	}
if (argc == 1) {
	// check file readability
	FILE* f = open_source_file(argv[0]);
	if (!f)	return 1;
	fclose(f);
	// or use util::memory::excl_ptr<FILE, fclose_tag>

	// dependency generation setup
	opt.source_file = argv[0];
} else {
	opt.use_stdin = true;
}

	// flatten it
	return flatten_source(
		(argc == 0) ? NULL : opt.source_file.c_str(), opt).good ? 0 : 1;
}

//-----------------------------------------------------------------------------
/**
	\return 0 if is ok to continue, anything else will signal early
		termination, an error will cause exit(1).
	NOTE: the getopt option string begins with '+' to enforce
		POSIXLY correct termination at the first non-option argument.  
 */
int
flatten::parse_command_options(const int argc, char* argv[], options& opt) {
	static const char* optstring = "+hi:I:M:Pv";
	int c;
	while ((c = getopt(argc, argv, optstring)) != -1) {
	switch (c) {
	case 'h':
		return 1;
	case 'I':
		// no need to check validity of paths yet
		opt.include_paths.push_back(optarg);
		break;
	case 'i':
		opt.prepend_files.push_back(optarg);
		break;
	case 'M':
		opt.make_depend = true;
		opt.make_depend_target = optarg;
		break;
	case 'P':
		// doesn't have complement or undo...
		lexer::flatten_with_wrappers(false);
		break;
	case 'v':
		config::dump_all(cout);
		exit(0);
		break;
	case ':':
		cerr << "Expected but missing non-option argument." << endl;
		return 1;
	case '?':
		util::unknown_option(cerr, optopt);
		return 1;
	default:
		abort();
	}       // end switch
	}       // end while
	return 0;
}

//-----------------------------------------------------------------------------
/**
	Prints summary of options and arguments.  
 */
void
flatten::usage(void) {
	cerr <<
"hacpp: flattens input file (import directives) to single file, print to stdout"
		<< endl;
	cerr << "usage: hacpp [options] [hac-source-file]" << endl;
	cerr << "\tIf no input file is named, then read from stdin (pipe)"
		<< endl;
	cerr << "options:" << endl;
#if 0
{
	typedef	options_modifier_map_type::const_iterator	const_iterator;
	const_iterator i(options_modifier_map.begin());
	const const_iterator e(options_modifier_map.end());
	for ( ; i!=e; ++i) {
		cerr << "\t    " << i->first << ": " <<
			i->second.brief << endl;
	}
}
#endif
	cerr << "\t-h : gives this usage message" << endl <<
		"\t-I <path> : adds include path (repeatable)" << endl <<
		"\t-i <file> : prepends import file (repeatable)" << endl;
	cerr << "\t-M <dependfile> : produces make dependency to file" << endl;
//	cerr << "\t-p : pipe in source from stdin" << endl;
	cerr << "\t-P : suppress #FILE hierarchical wrappers in output" << endl;
	cerr << "\t-v : print version and exit" << endl;
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
	return HAC::flatten::main(argc, argv, g);
}
#endif	// WITH_MAIN

