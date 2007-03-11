/**
	\file "main/flatten.cc"
	Converts HAC source code to an object file (pre-unrolled).
	This file was born from "art++2obj.cc" in earlier revision history.

	$Id: flatten.cc,v 1.2 2007/03/11 16:34:33 fang Exp $
 */

#include <iostream>
#include <list>
#include <string>
#include <map>
#include "main/program_registry.h"
#include "main/flatten.h"
#include "main/main_funcs.h"
#include "main/compile_options.h"
#include "main/global_options.h"
#include "lexer/file_manager.h"
#include "util/getopt_portable.h"
#include "util/getopt_mapped.h"
// #include "util/dirent.h"		// configured wrapper around <dirent.h>
#include "util/attributes.h"

extern HAC::lexer::file_manager
hackt_parse_file_manager;

namespace HAC {
#include "util/using_ostream.h"
using util::memory::excl_ptr;
using std::list;
using std::string;
using lexer::file_manager;
using util::good_bool;

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
flatten::main(const int argc, char* argv[], const global_options&) {
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
	opt.export_include_paths(hackt_parse_file_manager);

	if (argc -optind > 1 || argc -optind <= 0) {
		usage();
		return 0;
	}
	argv += optind;		// shift
	FILE* f = open_source_file(argv[0]);
	if (!f)	return 1;

	// dependency generation setup
	opt.source_file = argv[0];

	// flatten it
	return flatten_source(argv[0]).good ? 0 : 1;
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
	static const char* optstring = "+hI:M:";
	int c;
	while ((c = getopt(argc, argv, optstring)) != -1) {
	switch (c) {
	case 'h':
		return 1;
	case 'I':
		// no need to check validity of paths yet
		opt.include_paths.push_back(optarg);
		break;
	case 'M':
		opt.make_depend = true;
		opt.make_depend_target = optarg;
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
	cerr << "flatten: flattens input file to single file" << endl;
	cerr << "usage: flatten [options] <hackt-source-file>"
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
	cerr << "\t-h: gives this usage messsage" << endl <<
		"\t-I <path> : adds include path (repeatable)" << endl;
	cerr << "\t-M <dependfile> : produces make dependency to file" << endl;
	cerr << "\tIf no output object file is given, flattend module will not be saved."
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
	return HAC::flatten::main(argc, argv, g);
}
#endif	// WITH_MAIN

