/**
	\file "main/main_funcs.cc"
	Useful main-level functions to call.
	Indent to hide most complexity here, exposing a bare-bones
	set of public callable functions.  
	$Id: main_funcs.cc,v 1.39 2011/03/23 00:36:21 fang Exp $
 */

#define	ENABLE_STACKTRACE		0
#define	ENABLE_STATIC_TRACE		0
/**
	Eventually split out parser functions to parser library
	for better modularity.  
 */
#define	KEEP_PARSE_FUNCS		1

#include "config.h"			// for USING_BISON / USING_YACC
#include "util/static_trace.hh"
DEFAULT_STATIC_TRACE_BEGIN

#include <iostream>
#include <fstream>
#include <stack>
#include <cstdio>

#include "util/macros.h"
#include "main/main_funcs.hh"

#include "AST/root.hh"			// for parser::root_body
#include "AST/type_base.hh"		// for parser::concrete_type_ref
#include "AST/parse_context.hh"		// for parser::context
#include "main/compile_options.hh"
#include "main/compile.hh"
#include "main/create.hh"
#include "common/config.hh"
#include "Object/type/process_type_reference.hh"
#include "Object/type/canonical_type.hh"
#include "util/getopt_portable.h"
#include "util/getopt_mapped.hh"
#include "util/value_saver.hh"
#include "util/persistent_object_manager.hh"
#if IMPLICIT_SUPPLY_PORTS
#include "AST/globals.hh"
#include "AST/instance.hh"
#endif

#if KEEP_PARSE_FUNCS
// forward declarations needed for YSTYPE
#include "parser/hackt-parse-real.hh"	// for YYSTYPE
using util::memory::count_ptr;

#include "lexer/file_manager.hh"
#include "lexer/yyin_manager.hh"
#include "lexer/flex_lexer_state.hh"
#include "lexer/hacflat-yystype.hh"
#endif
#include "util/libc.h"			// for remove

#if KEEP_PARSE_FUNCS
/**
	This prototype for yyparse is either set by
	YYPARSE_PARAM for bison, or hacked by scripts for yacc.  
	Coordinate with "parser/hackt-parse-options.hh".
 */
extern	int	hackt_parse(void*, YYSTYPE&, flex::lexer_state&);
extern	HAC::lexer::file_manager	hackt_parse_file_manager;
extern	HAC::lexer::embedded_file_stack_type	hackt_embedded_file_stack;
extern	int	__hacflat_lex(std::string&, flex::lexer_state&);

namespace HAC {
namespace lexer {
extern	good_bool	__flatten_source(FILE*);
}	// end namespace lexer
}	// end namespace HAC
#endif

#include "util/stacktrace.hh"

//=============================================================================
namespace HAC {
using std::ifstream;
using std::ofstream;
using std::ios_base;
using parser::parse_options;
using parser::root_body;
using parser::concrete_type_ref;
using util::persistent;
using util::persistent_object_manager;
using util::value_saver;
using lexer::file_manager;
#if KEEP_PARSE_FUNCS
using lexer::yyin_manager;
#endif
#include "util/using_ostream.hh"
typedef	parser::context		parse_context;

//=============================================================================
/**
	Makes sure the file is readable.  
	Why FILE* instead of ifstream?  
	Because lex/yacc read from FILE* yyin.  
	\param fname name of file to open and check.  
	\return valid FILE* if opened succesfully, else NULL.  
 */
FILE*
open_source_file(const char* fname) {
	STACKTRACE_VERBOSE;
	NEVER_NULL(fname);
	FILE* ret;
	// test if source is valid
	ifstream inf(fname);
	if (!inf) {
		cerr << "Error opening file \"" << fname
			<< "\" for reading." << endl;
		return NULL;
	}
	inf.close();
	ret = fopen(fname, "r");
	NEVER_NULL(ret);
	return ret;
}

//=============================================================================
/**
	Makes sure named object file is openable in binary mode.  
 */
good_bool
check_object_loadable(const char* fname) {
	STACKTRACE_VERBOSE;
	// test if file is valid
	ifstream f(fname, ios_base::binary);
	if (!f) {
		cerr << "Error opening object file \"" << fname
			<< "\"." << endl;
		return good_bool(false);
	}
	f.close();
	return good_bool(true);
}

//=============================================================================
/**
	Checks whether or not named file is writeable.  
	Also removes the file just created, so it doesn't leave an 
	empty touched file.
	\return good if file is writable.  
 */
good_bool
check_file_writeable(const char* fname) {
	STACKTRACE_VERBOSE;
	// test if file is valid
	ofstream outf(fname, ios_base::binary | ios_base::app);
	if (!outf) {
		cerr << "Error opening file \"" << fname
			<< "\" for writing." << endl;
		return good_bool(false);
	} else {
		// OK to remove file before closing stream?
		const int err __ATTRIBUTE_UNUSED__ = remove(fname);
		// remove() is from <stdio.h>
		if (err) {
			cerr << "Error removing file \"" << fname
				<< "\"." << endl;
			return good_bool(false);
			assert(!err);
		}
	}
	outf.close();
	return good_bool(true);
}

//=============================================================================
#if KEEP_PARSE_FUNCS
#if 0
/**
	Parses a file as an independent module, resulting in a root body.
	No error handling here.  
	This is called by the lexer upon import-directive (recursive).
	\param yyin is an already opened file.
	\return NULL on failure.
 */
count_ptr<root_body>
parse_to_AST(FILE* yyin) {
	typedef	count_ptr<root_body>		return_type;
	STACKTRACE_VERBOSE;
	YYSTYPE lval;			// root token (was yyval)
	NEVER_NULL(yyin);
	try {
		flex::lexer_state f(yyin);
		hackt_parse(NULL, lval, f);
	} catch (...) {
		return return_type(NULL);
	}
	return return_type(lval._root_body);
}
#else
// relocated to AST/parser_globals.cc
#endif

//=============================================================================
/**
	Emit dependency file, based on seen and included files.
	\return true on error.
 */
static
bool
make_include_depends(const char* md, const string& target_object, 
		const string& source_file) {
	if (check_file_writeable(md).good) {
		ofstream mtf(md);
		mtf << target_object << ": " << source_file;
		// list all seen files' full paths
		hackt_parse_file_manager.make_depend(mtf, source_file);
		return false;
	} else {
		// already have error message
		return true;
	}
}

//=============================================================================
/**
	Side-effect: sets the yyin FILE* before parsing, and closes it
		before returning.  
	\param c the name associated with this file, may be NULL for stdin.  
	\return allocated AST.
 */
static
count_ptr<root_body>
parse_to_AST(const char* c, const compile_options& opt) {
	typedef	count_ptr<root_body>		return_type;
	STACKTRACE_VERBOSE;
	YYSTYPE hackt_val;		// root token (was yyval)
	hackt_val._root_body = NULL;
	// error status
	bool need_to_clean_up_file_manager = false;
{
	// hackt_in WAS the global yyin FILE*.  
	// now we let the file manager produce a valid FILE*
	// and pass it to the modified yyparse().  
	const yyin_manager ym(hackt_parse_file_manager, c, false);
	FILE* yyin = ym.get_file();
	if (yyin) {
	try {
		// pass in FILE*
		flex::lexer_state f(yyin);
		hackt_parse(NULL, hackt_val, f);
	} catch (...) {
		// then it's possible that the file_manager is not balanced.  
		need_to_clean_up_file_manager = true;
	}
	} else {
		return return_type(NULL);
	}
	if (opt.make_depend &&
			make_include_depends(opt.make_depend_target.c_str(), 
			opt.target_object, opt.source_file)) {
		return return_type(NULL);
	}
}
	if (need_to_clean_up_file_manager) {
		// clear the embedded file stack
		while (!hackt_embedded_file_stack.empty()) {
			hackt_embedded_file_stack.pop();
		}
		hackt_parse_file_manager.reset();
		return return_type(NULL);
	}
	return return_type(hackt_val._root_body);
}
#endif

//=============================================================================
#if KEEP_PARSE_FUNCS
/**
	Phase 1 of type-check.  
 */
static
count_ptr<module>
check_AST(const root_body& r, const char* name, const parse_options& po) {
	typedef	count_ptr<module>	return_type;
	STACKTRACE_VERBOSE;
	return_type mod(new module(name));
	NEVER_NULL(mod);
	parse_context pc(*mod, po);
	try {
		r.check_build(pc);
	} catch (...) {
		// error message
		return return_type(NULL);
	}
	return mod;
}
#endif

//=============================================================================
#if KEEP_PARSE_FUNCS
/**
	Prints flattened source (expanding imports) to stdout.  
	NOTE: uses global hackt_parse_file_manager.  :/
	TODO: move this to "lexer/hacflat-lex.ll"
	NOTE: we don't use a parser because there is no grammar 
		associated with preprocessing.  
	\param name is name of file or NULL for using stdin
	\param b pass true to use search paths, 
		false to use path to file as is.  
 */
good_bool
flatten_source(const char* name, const compile_options& opt) {
	STACKTRACE_VERBOSE;
	const yyin_manager ym(hackt_parse_file_manager, name, false);
	FILE* yyin = ym.get_file();
	if (yyin) {
		const bool need_to_clean_up_file_manager =
			!lexer::__flatten_source(yyin).good;
		if (opt.make_depend &&
				make_include_depends(
				opt.make_depend_target.c_str(), 
				opt.target_object, opt.source_file)) {
			return good_bool(false);
		}
		if (need_to_clean_up_file_manager) {
			// hackt_parse_file_manager.reset();
			return good_bool(false);
		}
		return good_bool(true);
	} else {
		return good_bool(false);
	}
}

//=============================================================================
/**
	Parses a file and checks it.  
	Only called by top-level programs, and hence, not recursive/reentrant.
	\param name the name of the top-level source file, 
		if NULL, then will use stdin.  
	\param opt compiler options.  
	\return if parse and type-check are successful, then return
		a pointer to top-level constructed module, else NULL.  
 */
count_ptr<module>
parse_and_check(const char* name, const compile_options& opt) {
	typedef	count_ptr<module>	return_type;
	STACKTRACE_VERBOSE;
	static const char* dflt = "-stdin-";
	// test file existence and readibility first
	if (name) {
		FILE* f = open_source_file(name);
		if (!f)	{
			// error message?
			return return_type(NULL);
		} else {
			fclose(f);
			// or use util::memory::excl_ptr<FILE, fclose_tag>
		}
	}
	// error message would be nice
	const count_ptr<root_body> AST(parse_to_AST(name, opt));
	if (!AST) return return_type(NULL);
	// error message would be nice
#if IMPLICIT_SUPPLY_PORTS
	// push in reverse order
	AST->push_front(get_Vdd_attributes());
	AST->push_front(get_GND_attributes());
	AST->push_front(get_implicit_globals());
#endif
	const count_ptr<module>
		ret(check_AST(*AST, name ? name : dflt, opt.parse_opts));
	// copy/preserve the options that were used to compile
	if (ret)
		ret->compile_opts = opt;
	return ret;
}
#endif	// KEEP_PARSE_FUNCS

//=============================================================================
/**
	Pretends to write a module out to file and load it back in.  
	Basically, a self-consistency check.  
	Effectively makes a deep-copy through persistent object management.  
 */
good_bool
self_test_module(const module& m) {
	STACKTRACE_VERBOSE;
try {
	const count_ptr<module>
		module_copy(persistent_object_manager::self_test_no_file(m)
			.is_a<module>());
	NEVER_NULL(module_copy);
} catch(...) {
	cerr << "Unhandled exception in "
		"persistent_object_manager::self_test_no_file()." << endl;
	return good_bool(false);
}
	return good_bool(true);
} 

//=============================================================================
/**
	Identifies the binary object file format version.
	Increment this whenever format changes.  
	TODO: use library versioning scheme that also encodes compatibility
		x.y, where .y denotes forward compatible versions in series.
log:
1: [initial version]
2: node supply attributes, node attribute flags now 32b
3: spec directives for processes
4: preserve compile options to module
5: PRS internal nodes now take attributes
6a: pstring_const and pstring assignment/expression operations
6b: more string/real expression variants
6c: split meta_func_call into different types, one for each meta-expr type
7: spec invariants added optional strings for assert messages
8: process directionality flags, direction_declaration types
9: added direction enum to data_type_reference
10: added pstring_relational_expr type for string comparison operators
?: support for member nonmeta instance/value references
 */
static const size_t
#if NONMETA_MEMBER_REFERENCES
object_file_format_version = 10;
#else
object_file_format_version = 11;
#endif

//=============================================================================
/**
	Saves module to object file.  
 */
void
save_module(const module& m, const char* name) {
	STACKTRACE_VERBOSE;
	const string fname(name);
	const value_saver<size_t>
		__tmp(persistent_object_manager::format_version,
			object_file_format_version);
	persistent_object_manager::save_object_to_file(fname, m);
}

//=============================================================================
/**
	Saves module to object file.  
	Always warns about unimplemented persistent objects.  
	Temporarily modifies persistent object manager flags.
	\param d whether or not to dump the reconstruction header. 
 */
void
save_module_debug(const module& m, const char* name, const bool d) {
	STACKTRACE_VERBOSE;
	const value_saver<bool>
		_d(persistent_object_manager::dump_reconstruction_table, d);
	const value_saver<bool>
		_u(persistent::warn_unimplemented, true);
	save_module(m, name);
}

//=============================================================================
/**
	Loads object file and reconstructs module in memory.  
 */
count_ptr<module>
load_module(const char* fname) {
	STACKTRACE_VERBOSE;
	const value_saver<size_t>
		__tmp(persistent_object_manager::format_version,
			object_file_format_version);
	return persistent_object_manager::load_object_from_file(fname)
		.is_a<module>();
}

//=============================================================================
/**
	Loads object file and reconstructs module in memory.  
	Side-effect: sets debugging flags (sticky) for persistent
	and persistent_object_manager classes.  
 */
count_ptr<module>
load_module_debug(const char* fname) {
	STACKTRACE_VERBOSE;
	const value_saver<bool>
		_d(persistent_object_manager::dump_reconstruction_table, true);
	const value_saver<bool>
		_u(persistent::warn_unimplemented, true);
	return load_module(fname);
}

//=============================================================================
/**
	Parse the process type and unroll/create it.  
	\return NULL on any errors.  
 */
count_ptr<const process_type_reference>
parse_and_create_complete_process_type(const char* _type, const module& m) {
	STACKTRACE_VERBOSE;
	typedef	count_ptr<const process_type_reference>	return_type;
	NEVER_NULL(_type);
	// parse the type
	// cerr << "type string: " << cf.named_process_type.c_str() << endl;
	const concrete_type_ref::return_type
		t(parser::parse_and_check_complete_type(_type, m));
	if (!t) {
		cerr << "Error with type reference \'" <<
			_type << "\'." << endl;
		return return_type(NULL);
	}
	const return_type pt(t.is_a<const process_type_reference>());
	if (!pt) {
		cerr << "Error: \'" << _type << "\' does not "
			"refer to a process type." << endl;
		return return_type(NULL);
	}
	const return_type rpt(pt->unroll_resolve());
	if (!rpt) {
		cerr << "Error resolving process type parameters." << endl;
		return return_type(NULL);
	}
	// oops: isn't this supposed to create()?
	const entity::canonical_process_type cpt(rpt->make_canonical_type());
	if (!cpt.create_definition_footprint(m.get_footprint()).good) {
		cerr << "Error instantiating type \'" << _type << "\'." << endl;
		return return_type(NULL);
	}
	return rpt;
}

//=============================================================================
// class create method definitions

/**
	Preserve the global variables optind, optarg because getopt
	will modify them.  
	\param f the flag returned by getopt.  
 */
int
parse_create_flag(const int f, compile_options& opt) {
	STACKTRACE_VERBOSE;
switch (f) {
/***
@texinfo compile/option-c.texi
@defopt -c
Indicate that input file is a source, not object file, 
and needs to be compiled.
@end defopt
@end texinfo
***/
	case 'c':
		opt.compile_input = true;
		break;
/***
@texinfo compile/option-C-upper.texi
@defopt -C opts
When compiling source, forward options @var{opts} to the compiler-driver.  
@strong{Suggestion}: when passing compiler-options on the command-line, 
wrap in ``double-quotes'' to group a list of arguments into a single string
in the shell.
@end defopt
@end texinfo
***/
	case 'C': {
		// forward bundled arguments to compile-driver
		// because getopt is not re-entrant...
		const value_saver<int> _1(optind);
		const value_saver<int> _2(optopt);
		const value_saver<int> _3(opterr);
		const value_saver<char*> _4(optarg);
		// despite my best efforts, this STILL doesn't work
		// because getopt is simply not required to be re-entrant
		// TODO: re-implement own re-entrant getopt.
		optind = 0;	// don't skip the first argument
		std::vector<char*> cargs;
		util::splitopt(optarg, cargs);
		const int r =
			compile::parse_command_options(cargs.size(), 
				&cargs[0], opt);
		if (r) {
			compile::usage();
			return r;
		}
		}
		break;
	default:
		abort();

	}
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
create::parse_command_options(const int argc, char* argv[], options& opt) {
	STACKTRACE_VERBOSE;
	static const char* optstring = "+hcC:v";
	int c;
	while ((c = getopt(argc, argv, optstring)) != -1) {
	switch (c) {
	case 'c':
	case 'C': {
		const int r = parse_create_flag(c, opt);
		if (r) return r;
	}
		break;
	case 'h':
		create_usage(argv[0], cout);
		return 0;
	case 'v':
		config::dump_all(cout);
		return 0;
	case ':':
		cerr << "Expected but missing non-option argument." << endl;
		return 1;
	case '?':
		util::unknown_option(cerr, optopt);
		return 1;
	default:
		abort();

	}
	}
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
create_usage(const char* name, ostream& o) {
        o << "usage: " << name <<
                " <hackt-obj-infile> <hackt-obj-outfile>" << endl;
        o << "options:" << endl;
        o << "\t-c : input file is a source (compile it), not object\n"
                "\t-C <opts> : forward options to compiler (driver)\n"
                "\t-h : print this help\n"
                "\t-v : print version"
                << endl;
}

//=============================================================================
}	// end namespace HAC

DEFAULT_STATIC_TRACE_END

