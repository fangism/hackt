/**
	\file "main/compile.cc"
	Converts HAC source code to an object file (pre-unrolled).
	This file was born from "art++2obj.cc" in earlier revision history.

	$Id: compile.cc,v 1.25 2010/08/05 18:25:31 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include <iostream>
#include <list>
#include <string>
#include <map>
#include <cstdio>
#include "common/config.h"
#include "main/compile.h"
#include "main/main_funcs.h"
#include "main/compile_options.h"
#include "main/global_options.h"
#include "lexer/file_manager.h"
#if COMPILE_USE_OPTPARSE
#include "util/optparse.tcc"
#endif
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

#if !COMPILE_USE_OPTPARSE
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
#endif

//=============================================================================
// class compile static initializers

const char
compile::name[] = "compile";

const char
compile::brief_str[] = "Compiles HACKT source to object file.";


#if COMPILE_USE_OPTPARSE
typedef	util::options_map_impl<compile_options>		options_map_impl_type;
typedef	options_map_impl_type::opt_map_type		opt_map_type;
static	options_map_impl_type				options_map_wrapper;
#else
/**
	Options modifier map must be initialized before any registrations.  
 */
const compile::options_modifier_map_type
compile::options_modifier_map;
#endif

//=============================================================================
static const char default_options_brief[] = "Needs description.";

#if COMPILE_USE_OPTPARSE
class compile::register_options_modifier {
	typedef options_map_impl_type::opt_entry        opt_entry;
	typedef options_map_impl_type::opt_func         modifier_type;
	const opt_entry&                                receipt;

public:
	register_options_modifier(const string& Mode,
			const modifier_type COM,
			const string& h = default_options_brief) :
		receipt(options_map_wrapper.options_map[Mode] =
			opt_entry(COM, NULL, NULL, h)) {
	}

	register_options_modifier(const string& Mode,
			const modifier_type COM,
			const char* h) :
		receipt(options_map_wrapper.options_map[Mode] =
			opt_entry(COM, NULL, NULL, h)) {
	}
} __ATTRIBUTE_UNUSED__ ;
#else
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
#endif	// COMPILE_USE_OPTPARSE

//=============================================================================
// compile::options_modifier declarations and definitions
// Texinfo documentation is below, under -f option.  

#if COMPILE_USE_OPTPARSE
#define	OPTARG			const util::option_value& v, 
#define	OPTARG_UNUSED		const util::option_value&, 
#define	OPTARG_FWD		v, 
typedef	bool			optfun_return_type;
#define	OPTFUN_RETURN		return false;
#else
#define	OPTARG
#define	OPTARG_UNUSED
#define	OPTARG_FWD
typedef	good_bool		optfun_return_type;
#define	OPTFUN_RETURN		return good_bool(true);
#endif

using HAC::parser::parse_options;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if COMPILE_USE_OPTPARSE
#define SET_OPT(t,m,v)							\
	options_map_impl_type::set_member_constant<t, &compile_options::m, v>

#define SET_OPT2(t1,m1,t2,m2,v)						\
	options_map_impl_type::set_member_member_constant<		\
		t1, t2, &compile_options::m1, &t1::m2, v>

#define SET_BOOL_OPT(m,v)		SET_OPT(bool, m, v)

#define SET_BOOL_OPT2(t1,m1,t2,m2,v)	SET_OPT2(t1, m1, bool, m2, v)

#define SET_PARSE_OPT2(t2,m2,v)						\
	SET_OPT2(parse_options, parse_opts, t2, m2, v)
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if COMPILE_USE_OPTPARSE
#define	DEFINE_OPTION(type, mem, key, val, str)				\
static const compile::register_options_modifier				\
	compile_opt_mod_ ## mem(key, &SET_OPT(type, mem, val), str);

#define	DEFINE_BOOL_OPTION(mem, key, val, str)				\
	DEFINE_OPTION(bool, mem, key, val, str)

#define	DEFINE_OPTION2(type1, mem1, type2, mem2, key, val, str)		\
static const compile::register_options_modifier				\
	compile_opt_mod_ ## mem2 ## _ ## val(key,			\
		&SET_OPT2(type1, mem1, type2, mem2, val), str);

#define	DEFINE_PARSE_OPTION2(type2, mem2, key, val, str)		\
	DEFINE_OPTION2(parse_options, parse_opts, 			\
		type2, mem2, key, val, str)
#define	DEFINE_CREATE_OPTION2(type2, mem2, key, val, str)		\
	DEFINE_OPTION2(create_options, create_opts, 			\
		type2, mem2, key, val, str)
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if COMPILE_USE_OPTPARSE
#define	DEFINE_BOOL_OPTION_PAIR(mem, key, truestr, falsestr)		\
static const compile::register_options_modifier				\
	compile_opt_mod_ ## mem(key, &SET_BOOL_OPT(mem, true), truestr), \
	compile_opt_mod_no_## mem("no-" key, &SET_BOOL_OPT(mem, false),	\
		falsestr);
#define	DEFINE_BOOL_PARSE_OPTION_PAIR(mem, key, truestr, falsestr)	\
	DEFINE_PARSE_OPTION2(bool, mem, key, true, truestr)		\
	DEFINE_PARSE_OPTION2(bool, mem, "no-" key, false, falsestr)
#else
#define	DEFINE_BOOL_OPTION_PAIR(mem, key, truestr, falsestr)		\
static									\
optfun_return_type							\
__compile_ ## mem(OPTARG_UNUSED compile::options& cf) {			\
	cf.mem = true;							\
	OPTFUN_RETURN							\
}									\
static									\
optfun_return_type							\
__compile_no_ ## mem(OPTARG_UNUSED compile::options& cf) {		\
	cf.mem = false;							\
	OPTFUN_RETURN							\
}									\
static const compile::register_options_modifier				\
	compile_opt_mod_ ## mem(key, &__compile_ ## mem, truestr),	\
	compile_opt_mod_no_## mem("no-" key, &__compile_no_ ## mem, falsestr);
#endif

//-----------------------------------------------------------------------------
DEFINE_BOOL_OPTION_PAIR(dump_include_paths, "dump-include-paths",
	"dumps -I include paths as they are processed",
	"suppress feedback of -I include paths")

//-----------------------------------------------------------------------------
DEFINE_BOOL_OPTION_PAIR(dump_object_header, "dump-object-header",
	"dumps persistent object header before saving",
	"suppress persistent object header dump")

//-----------------------------------------------------------------------------
// dialect flags
// texinfo documentation is below under option-f

DEFINE_PARSE_OPTION2(bool, export_all, "export-all", true,
	"treat all definitions as exported")
DEFINE_PARSE_OPTION2(bool, export_all, "export-strict", false,
	"only export definitions that are marked as such (default, ACT)")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// the following are create-phase options forwarded to the create phase
DEFINE_CREATE_OPTION2(canonicalize_policy, canonicalize_mode,
	"canonical-shortest-hier", SHORTEST_HIER_NO_LENGTH,
	"prefer aliases by number of hierarchy levels only (default)")
DEFINE_CREATE_OPTION2(canonicalize_policy, canonicalize_mode,
	"canonical-shortest-length", SHORTEST_HIER_MIN_LENGTH,
	"prefer aliases using overall length as tie-breaker")
DEFINE_CREATE_OPTION2(canonicalize_policy, canonicalize_mode,
	"canonical-shortest-stage", SHORTEST_EMULATE_ACT,
	"prefer aliases using per-member length as tie-breaker (ACT, unimplemented)")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DEFINE_BOOL_PARSE_OPTION_PAIR(namespace_instances, "namespace-instances",
	"allow instance management outside global namespace (default)",
	"forbid instance management outside global namespace (ACT)")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DEFINE_BOOL_PARSE_OPTION_PAIR(array_internal_nodes, "array-internal-nodes",
	"allow implicit arrays of internal nodes in PRS (default)",
	"reject implicit arrays of internal nodes in PRS (ACT)")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if COMPILE_USE_OPTPARSE
template <error_policy parse_options::*mem>
static
bool
__set_policy_member(const util::option_value& opt,
		compile_options& c_opt) {
const size_t s = opt.values.size();
if (s >= 1) {
	if (s > 1) {
	cerr << "Warning: extra arguments passed to \'" << opt.key
		<< "\' option ignored." << endl;
	}
	const string& p(opt.values.front());
	if (p == "ignore") {
		c_opt.parse_opts.*mem = OPTION_IGNORE;
	} else if (p == "warn") {
		c_opt.parse_opts.*mem = OPTION_WARN;
	} else if (p == "error") {
		c_opt.parse_opts.*mem = OPTION_ERROR;
	} else {
		cerr << "Error: error policy values for option " <<
			opt.key << " are [ignore|warn|error]." << endl;
		return true;
	}
}
	return false;
}

static const compile::register_options_modifier
__compile_om_case_collision("case-collision", 
	&__set_policy_member<&parse_options::case_collision_policy>, 
	"handle case-insensitive collisions (= ignore|[warn]|error)");

#else
static
good_bool
__compile_case_collision_ignore(compile::options& o) {
	o.parse_opts.case_collision_policy = OPTION_IGNORE;
	return good_bool(true);
}

static
good_bool
__compile_case_collision_warn(compile::options& o) {
	o.parse_opts.case_collision_policy = OPTION_WARN;
	return good_bool(true);
}

static
good_bool
__compile_case_collision_error(compile::options& o) {
	o.parse_opts.case_collision_policy = OPTION_ERROR;
	return good_bool(true);
}

// TODO: actually parse the policy argument using optparse()
static const compile::register_options_modifier
__compile_om_case_collision_ignore("case-collision=ignore", 
	&__compile_case_collision_ignore, 
	"ignore case-insensitive collisions"),
__compile_om_case_collision_warn("case-collision=warn", 
	&__compile_case_collision_warn, 
	"warn about case-insensitive collisions (default)"),
__compile_om_case_collision_error("case-collision=error", 
	&__compile_case_collision_error, 
	"reject case-insensitive collisions");
#endif	// COMPILE_USE_OPTPARSE

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static const compile::register_options_modifier
__compile_om_unknown_spec("unknown-spec", 
	&__set_policy_member<&parse_options::unknown_spec_policy>, 
	"handle unknown spec directives (= ignore|warn|[error])");

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
optfun_return_type
__compile_ACT(OPTARG_UNUSED compile::options& o) {
#if COMPILE_USE_OPTPARSE
	o.parse_opts.export_all = false;
	o.parse_opts.namespace_instances = false;
	o.parse_opts.array_internal_nodes = false;
#else
	__compile_export_strict(o);
	__compile_no_namespace_instances(o);
	__compile_no_array_internal_nodes(o);
#endif
	OPTFUN_RETURN
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
	// or use util::memory::excl_ptr<FILE, fclose_tag>
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
	static const char* optstring = "+df:hI:M:o:pv";
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

// TODO: summarize ACT-compatibility in form of a table
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
@item @option{case-collision=[ignore|warn|error]}:
	Set the error handling policy for case-insensitive collisions.
	@itemize
	@item @option{ignore} skips the check altogether
	@item @option{warn} issues a warning but continues
	@item @option{error} rejects and aborts compiling
	@end itemize
@item @option{unknown-spec=[ignore|warn|error]}:
	Sets the error handling policy for unknown spec directives.
@end itemize

Dialect flags (for ACT-compatibility):
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

The following options are forwarded to the create-phase of compilation
by the driver.  That is, they do not have any affect until the create phase.
These options must be specified up-front at compile-time and cannot be 
overriden at create-time on the command line.
@itemize
@item @option{canonical-shortest-hier}:
	Only consider hierarchical depth (number of member-dots) for
	choosing canonical alias, no further tiebreaker.
@item @option{canonical-shortest-length}:
	In addition to minimizing the hierarchy depth, also use overall
	string length as a tiebreaker.
@item @option{canonical-shortest-stage}: (unimplemented)
	Considers length of identifiers stage-by-stage when hierarchical 
	depths are equal.
	This is for best compatibility with ACT mode.
@end itemize
@end defopt
@end texinfo
***/
	case 'f': {
#if COMPILE_USE_OPTPARSE
	typedef opt_map_type::const_iterator                    const_iterator;
	const opt_map_type&
		options_modifier_map(options_map_wrapper.options_map);
	const util::option_value ov(util::optparse(optarg));
	const const_iterator mi(options_modifier_map.find(ov.key));
#else
	typedef options_modifier_map_type::mapped_type          mapped_type;
	typedef options_modifier_map_type::const_iterator       const_iterator;
	const const_iterator mi(options_modifier_map.find(optarg));
#endif
		if (mi == options_modifier_map.end()
#if !COMPILE_USE_OPTPARSE
			|| !mi->second
#endif
			) {
			cerr << "Invalid option argument: " << optarg << endl;
			return 1;
		}
#if COMPILE_USE_OPTPARSE
			else if ((mi->second.func)(ov, opt))
#else
			else if (!((mi->second)(opt).good))
#endif
		{
			return 1;
		}
		break;
	}
/***
@texinfo compile/option-h.texi
@defopt -h
Show usage and exit.
@end defopt
@end texinfo
***/
	case 'h':
		// return 1;
		usage();
		exit(0);
		break;
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
/***
@texinfo compile/option-v.texi
@defopt -v
Show version and build information and exit.
@end defopt
@end texinfo
***/
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
	cerr << "  -d: produces text dump of compiled module" << endl <<
		"  -f <opt> : general compile flags (repeatable)" << endl;
{
#if COMPILE_USE_OPTPARSE
	options_map_wrapper.help(cerr, false, false);
#else
	typedef	options_modifier_map_type::const_iterator	const_iterator;
	const_iterator i(options_modifier_map.begin());
	const const_iterator e(options_modifier_map.end());
	for ( ; i!=e; ++i) {
		cerr << "    " << i->first << ": " <<
			i->second.brief << endl;
	}
#endif
}
	cerr << "  -h: gives this usage messsage and exits" << endl <<
		"  -I <path> : adds include path (repeatable)" << endl;
	cerr << "  -M <dependfile> : produces make dependency to file" << endl;
	cerr << "  -o <objfile> : option to name output object file" << endl;
	cerr << "  -p : pipe in source from stdin" << endl;
	cerr << "  -v : print version information and exit" << endl;
	cerr << "If no output object file is given, compiled module will not be saved."
		<< endl;
}

//=============================================================================
}	// end namespace HAC



