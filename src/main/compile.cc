/**
	\file "main/compile.cc"
	Converts HAC source code to an object file (pre-unrolled).
	This file was born from "art++2obj.cc" in earlier revision history.

	$Id: compile.cc,v 1.20 2008/07/30 05:26:46 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include <iostream>
#include <list>
#include <string>
#include <map>
#include "main/compile.h"
#include "main/main_funcs.h"
#include "main/compile_options.h"
#include "main/global_options.h"
#include "lexer/file_manager.h"
#include "util/getopt_portable.h"
#include "util/getopt_mapped.h"
#include "util/attributes.h"
#include "util/stacktrace.h"

extern HAC::lexer::file_manager
hackt_parse_file_manager;

namespace HAC {
#include "util/using_ostream.h"
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
// Texinfo documentation is below, under -f option.  

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

//-----------------------------------------------------------------------------
// dialect flags
// texinfo documentation is below under option-f

static
good_bool
__compile_export_all(compile::options& o) {
	o.parse_opts.export_all = true;
	return good_bool(true);
}

static const compile::register_options_modifier
__compile_om_export_all("export-all", 
	&__compile_export_all,
	"treat all definitions as exported");

static
good_bool
__compile_export_strict(compile::options& o) {
	o.parse_opts.export_all = false;
	return good_bool(true);
}

static const compile::register_options_modifier
__compile_om_export_strict("export-strict", 
	&__compile_export_strict,
	"only export definitions that are marked as such (default, ACT)");

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
good_bool
__compile_namespace_instances(compile::options& o) {
	o.parse_opts.namespace_instances = true;
	return good_bool(true);
}

static const compile::register_options_modifier
__compile_om_namespace_instances("namespace-instances", 
	&__compile_namespace_instances,
	"allow instance management outside global namespace (default)");

static
good_bool
__compile_no_namespace_instances(compile::options& o) {
	o.parse_opts.namespace_instances = false;
	return good_bool(true);
}

static const compile::register_options_modifier
__compile_om_no_namespace_instances("no-namespace-instances", 
	&__compile_no_namespace_instances,
	"forbid instance management outside global namespace (ACT)");

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
good_bool
__compile_array_internal_nodes(compile::options& o) {
	o.parse_opts.array_internal_nodes = true;
	return good_bool(true);
}

static const compile::register_options_modifier
__compile_om_array_internal_nodes("array-internal-nodes", 
	&__compile_array_internal_nodes,
	"allow implicit arrays of internal nodes in PRS (default)");

static
good_bool
__compile_no_array_internal_nodes(compile::options& o) {
	o.parse_opts.array_internal_nodes = false;
	return good_bool(true);
}

static const compile::register_options_modifier
__compile_om_no_array_internal_nodes("no-array-internal-nodes", 
	&__compile_no_array_internal_nodes,
	"reject implicit arrays of internal nodes in PRS (ACT)");

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
good_bool
__compile_ACT(compile::options& o) {
	__compile_export_strict(o);
	__compile_no_namespace_instances(o);
	__compile_no_array_internal_nodes(o);
	return good_bool(true);
}

static const compile::register_options_modifier
__compile_om_ACT("ACT", &__compile_ACT,
	"preset: all ACT-mode flags maximum compatibility");

//=============================================================================
// class compile method definitions

compile::compile() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The main program for compiling source to object.  
	Also parses options and sets flags.  
	\return negative number for non-error 'exit-now' status, 
		0 for success, positive number for other error.  
	TODO: is there a way to return the number of arguments parsed, 
		or just optind?
 */
int
compile::make_module(int argc, char* argv[], options& opt, 
		count_ptr<module>& ret) {
	STACKTRACE_VERBOSE;
	if (parse_command_options(argc, argv, opt)) {
		usage();
		return 1;
	}
	argv += optind;		// shift
	argc -= optind;

	const int max_args = opt.use_stdin ? 1 : 2;
	const int target_ind = max_args -1;
	if (argc > max_args || argc <= 0) {
		usage();
		return 1;
	}
if (!opt.use_stdin) {
	opt.source_file = argv[0];
	FILE* f = open_source_file(opt.source_file.c_str());
	if (!f)
		return 1;
	fclose(f);
}

	// dependency generation setup
	if (!opt.have_target()) {
		// if not already named by -o, use next non-option argument
	if (argc >= max_args) {
		opt.target_object = argv[target_ind];
	} else {
		// default: append 'o' to get object file name
		// problem: doesn't automatically strip srcdir
		// opt.target_object = opt.source_file + 'o';
	}
	}	// else ignore argv[1], use whatever was set before
	// have target by now
	if (opt.have_target()) {
	if (!check_file_writeable(opt.target_object.c_str()).good)
		return 1;
	}

	// parse it
	ret = parse_and_check(
		opt.use_stdin ? NULL : opt.source_file.c_str(), opt);
	if (!ret) {
		return 1;
	}
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
compile::main(const int argc, char* argv[], const global_options&) {
	options opt;
	count_ptr<module> mod;
	const int err = make_module(argc, argv, opt, mod);
	if (err < 0) {
		return 1;
	} else if (err > 0) {
		return err;
	}
	if (!mod) {
		return 1;
	}
	// if (argc -optind >= 2)
	if (opt.have_target()) {
		// save_module(*mod, opt.target_object);
		// save_module_debug(*mod, opt.target_object);
		save_module_debug(*mod, opt.target_object.c_str(), 
			opt.dump_object_header);
	}
	if (opt.dump_module)
		mod->dump(cerr);

	return 0;
}

//-----------------------------------------------------------------------------
/**
	\return 0 if is ok to continue, anything else will signal early
		termination, an error will cause exit(1).
	Set global variable optind to the number of initial tokens to
		skip (default = 1).
	NOTE: the getopt option string begins with '+' to enforce
		POSIXLY correct termination at the first non-option argument.  
 */
int
compile::parse_command_options(const int argc, char* argv[], options& opt) {
	STACKTRACE_VERBOSE;
	static const char* optstring = "+df:hI:M:o:p";
	int c;
	while ((c = getopt(argc, argv, optstring)) != -1) {
	switch (c) {
/***
@texinfo compile/option-d.texi
@defopt -d
Produces text dump of compiled module, 
like @command{hacobjdump} in @ref{Objdump}.
@end defopt
@end texinfo
***/
	case 'd':
		opt.dump_module = true;
		break;
/***
@texinfo compile/option-f.texi
@defopt -f optname
general compile flags (repeatable) where @var{optname} is one of the following:
@itemize
@item @option{dump-include-paths}:
        dumps @option{-I} include paths as they are processed
@item @option{dump-object-header}:
        (diagnostic) dumps persistent object header before saving
@item @option{no-dump-include-paths}:
        suppress feedback of @option{-I} include paths
@item @option{no-dump-object-header}:
        suppress persistent object header dump
@end itemize

Dialect flags (for ACT-compatbility):
@itemize
@item @option{export-all}:
	Treat all definitions as exported, i.e. no export checking.
@item @option{export-strict}:
	Check that definitions are exported for use outside their
	respective home namespaces (default, ACT).
@item @option{namespace-instances}
	Allow instance management outside global namespace (default).
	Negatable with @t{no-} prefixed.
	ACT mode: @option{no-namespace-instances}.
@item @option{array-internal-nodes}
	Allow implicit arrays of internal nodes in PRS (default).
	Negatable with @t{no-} prefixed.
	ACT mode: @option{no-array-internal-nodes}.
@end itemize
@option{ACT} is a preset that activates all ACT-mode flags for compatibility.
@end defopt
@end texinfo
***/
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
/***
@texinfo compile/option-h.texi
@defopt -h
Show usage.
@end defopt
@end texinfo
***/
	case 'h':
		return 1;
/***
@texinfo compile/option-I-upper.texi
@defopt -I path
Adds include path @var{path} for importing other source files (repeatable).
@end defopt
@end texinfo
***/
	case 'I':
		// no need to check validity of paths yet
		opt.include_paths.push_back(optarg);
		break;
/***
@texinfo compile/option-M-upper.texi
@defopt -M depfile
Emit import dependencies in file @var{depfile} as a side-effect.
Useful for automatic dynamic dependency-tracking in Makefiles.  
@end defopt
@end texinfo
***/
	case 'M':
		opt.make_depend = true;
		opt.make_depend_target = optarg;
		break;
/***
@texinfo compile/option-o.texi
@defopt -o objfile
Names @var{objfile} as the output object file to save.  
This is an alternative to naming the object file as the second
non-option argument.
@end defopt
@end texinfo
***/
	case 'o':
		opt.target_object = optarg;
		break;
/***
@texinfo compile/option-p.texi
@defopt -p
Expect input to be piped from stdin rather than a named file.  
Since the name of the input file is omitted in this case, 
the only non-option argument (if any) is interpreted as the name
of the output object file.  
@end defopt
@end texinfo
***/
	case 'p':
		opt.use_stdin = true;
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
	/***
		Now would be a good time to add include paths 
		to the parser's file manager.  
		Don't use a global file_manager, use local variables
		and pass as arguments, to allow these main subprograms
		to be re-entrant.  
		Q: Should file_manager be a member of module?
		Caution: calling this function multiple times with the 
		same options object will accumulate duplicate paths.
	***/
	opt.export_include_paths(hackt_parse_file_manager);
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
	cerr << "\t-o <objfile> : option to name output object file" << endl;
	cerr << "\t-p : pipe in source from stdin" << endl;
	cerr << "\tIf no output object file is given, compiled module will not be saved."
		<< endl;
}

//=============================================================================
}	// end namespace HAC



