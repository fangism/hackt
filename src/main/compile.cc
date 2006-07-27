/**
	\file "main/compile.cc"
	Converts HAC source code to an object file (pre-unrolled).
	This file was born from "art++2obj.cc" in earlier revision history.

	$Id: compile.cc,v 1.12 2006/07/27 05:55:35 fang Exp $
 */

#include <iostream>
#include <list>
#include <string>
#include <map>
#include "main/program_registry.h"
#include "main/compile.h"
#include "main/main_funcs.h"
#include "main/compile_options.h"
#include "lexer/file_manager.h"
#include "util/getopt_portable.h"
#include "util/dirent.h"		// configured wrapper around <dirent.h>
#include "util/attributes.h"

extern HAC::lexer::file_manager
hackt_parse_file_manager;

namespace HAC {
#include "util/using_ostream.h"
using util::memory::excl_ptr;
using entity::module;
using std::list;
using std::string;
using lexer::file_manager;
using util::good_bool;

//=============================================================================
/**
	Entry for options.  
 */
struct compile::options_modifier_info {
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
// class compile static initializers

const char
compile::name[] = "compile";

const char
compile::brief_str[] = "Compiles HACKT source to object file.";

const size_t
compile::program_id = register_hackt_program_class<compile>();

/**
	Options modifier map must be initialized before any registrations.  
 */
const compile::options_modifier_map_type
compile::options_modifier_map;

//=============================================================================
static const char default_options_brief[] = "Needs description.";

/**
	Options modification registration interface.  
 */
class compile::register_options_modifier {
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
// compile::options_modifier declarations and definitions

static
good_bool
__compile_dump_include_paths(compile::options& o) {
	o.dump_include_paths = true;
	return good_bool(true);
}

static const compile::register_options_modifier
__compile_om_dump_include_paths("dump-include-paths",
	&__compile_dump_include_paths,
	"dumps -I include paths as they are processed");

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
good_bool
__compile_no_dump_include_paths(compile::options& o) {
	o.dump_include_paths = false;
	return good_bool(true);
}

static const compile::register_options_modifier
__compile_om_no_dump_include_paths("no-dump-include-paths",
	&__compile_no_dump_include_paths,
	"suppress feedback of -I include paths");

//-----------------------------------------------------------------------------
static
good_bool
__compile_dump_object_header(compile::options& o) {
	o.dump_object_header = true;
	return good_bool(true);
}

static const compile::register_options_modifier
__compile_om_dump_object_header("dump-object-header",
	&__compile_dump_object_header,
	"dumps persistent object header before saving");

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
good_bool
__compile_no_dump_object_header(compile::options& o) {
	o.dump_object_header = false;
	return good_bool(true);
}

static const compile::register_options_modifier
__compile_om_no_dump_object_header("no-dump-object-header",
	&__compile_no_dump_object_header,
	"suppress persistent object header dump");

//=============================================================================
// class compile method definitions

compile::compile() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The main program for compiling source to object.  
	TODO: clean up file manager after done?
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
	opt.export_include_paths(hackt_parse_file_manager);

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

	// dependency generation setup
	opt.source_file = argv[0];
	if (argc -optind >= 2) {
		opt.target_object = argv[1];
	} else {
		// default: append 'o' to get object file name
		opt.target_object = string(argv[0]) + 'o';
	}

	// parse it
	const excl_ptr<module> mod =
		parse_and_check(argv[0], opt);
	if (!mod) {
		return 1;
	}
	if (argc -optind >= 2) {
		// save_module(*mod, argv[1]);
		// save_module_debug(*mod, argv[1]);
		save_module_debug(*mod, argv[1], opt.dump_object_header);
	}
	if (opt.dump_module)
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
	static const char* optstring = "+df:hI:M:";
	int c;
	while ((c = getopt(argc, argv, optstring)) != -1) {
	switch (c) {
	case 'd':
		opt.dump_module = true;
		break;
	case 'f': {
		const options_modifier_map_type::const_iterator
			mi(options_modifier_map.find(optarg));
		if (mi == options_modifier_map.end() || !mi->second) {
			cerr << "Invalid option argument: " << optarg << endl;
			return 1;
		} else if (!((mi->second)(opt).good)) {
			return 1;
		}
		break;
	}
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
		unknown_option(optopt);
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
compile::usage(void) {
	cerr << "compile: compiles input file to module object file" << endl;
	cerr << "usage: compile [options] <hackt-source-file> [hackt-obj-file]"
		<< endl;
	cerr << "options:" << endl;
	cerr << "\t-d: produces text dump of compiled module" << endl <<
		"\t-f <opt> : general compile flags (repeatable)" << endl;
{
	typedef	options_modifier_map_type::const_iterator	const_iterator;
	const_iterator i(options_modifier_map.begin());
	const const_iterator e(options_modifier_map.end());
	for ( ; i!=e; ++i) {
		cerr << "\t    " << i->first << ": " <<
			i->second.brief << endl;
	}
}
	cerr << "\t-h: gives this usage messsage" << endl <<
		"\t-I <path> : adds include path (repeatable)" << endl;
	cerr << "\t-M <dependfile> : produces make dependency to file" << endl;
	cerr << "\tIf no output object file is given, compiled module will not be saved."
		<< endl;
}

//=============================================================================
// class compile_options method definitions

void
compile_options::export_include_paths(file_manager& fm) const {
	typedef	include_paths_type::const_iterator	const_iterator;
	const_iterator i(include_paths.begin());
	const const_iterator e(include_paths.end());
	for ( ; i!=e; i++) {
		const string& s(*i);
		// check if path exists, otherwise, don't bother adding...
		if (util::dir_exists(s.c_str())) {
			fm.add_path(s);
			if (dump_include_paths) {
				cerr << "Added to search path: " << s << endl;
			}
		} else {
			if (dump_include_paths) {
				cerr << "Couldn\'t open dir: " << s << endl;
			}
		}
	}
}

//=============================================================================

}	// end namespace HAC

