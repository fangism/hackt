/**
	\file "main/main_funcs.cc"
	Useful main-level functions to call.
	Indent to hide most complexity here, exposing a bare-bones
	set of public callable functions.  
	$Id: main_funcs.cc,v 1.15.18.1 2007/08/14 05:16:45 fang Exp $
 */

#include <iostream>
#include <fstream>
#include <stack>

#define	ENABLE_STACKTRACE		0
#define	ENABLE_STATIC_TRACE		0

/**
	Eventually split out parser functions to parser library
	for better modularity.  
 */
#define	KEEP_PARSE_FUNCS		1

#include "config.h"			// for USING_BISON / USING_YACC

#include "util/macros.h"
#include "main/main_funcs.h"
#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include "AST/root.h"	// for parser::root_body
#include "AST/parse_context.h"		// for parser::context
#include "main/compile_options.h"
#include "util/getopt_portable.h"
#include "util/persistent_object_manager.h"

#if KEEP_PARSE_FUNCS
// forward declarations needed for YSTYPE
#include "parser/hackt-parse-real.h"	// for YYSTYPE
using util::memory::count_ptr;

#include "lexer/file_manager.h"
#include "lexer/yyin_manager.h"
#include "lexer/flex_lexer_state.h"
#include "lexer/hacflat-yystype.h"
#endif
#include "util/libc.h"			// for remove
#include "util/dirent.h"		// configured wrapper around <dirent.h>

#if KEEP_PARSE_FUNCS
/**
	This prototype for yyparse is either set by
	YYPARSE_PARAM for bison, or hacked by scripts for yacc.  
	Coordinate with "parser/hackt-parse-options.h".
 */
extern	int	hackt_parse(void*, YYSTYPE&, FILE*);
extern	HAC::lexer::file_manager	hackt_parse_file_manager;
extern	HAC::lexer::embedded_file_stack_type	hackt_embedded_file_stack;
extern	int	__hacflat_lex(std::string&, flex::lexer_state&);

namespace HAC {
namespace lexer {
extern	good_bool	__flatten_source(FILE*);
}	// end namespace lexer
}	// end namespace HAC
#endif

#include "util/stacktrace.h"

//=============================================================================
namespace HAC {
using std::ifstream;
using std::ofstream;
using std::ios_base;
using parser::root_body;
using util::persistent;
using util::persistent_object_manager;
using lexer::file_manager;
#if KEEP_PARSE_FUNCS
using lexer::yyin_manager;
#endif
#include "util/using_ostream.h"
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
	TODO: check object header to verify hackt format?
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
		assert(!err);
	}
	outf.close();
	return good_bool(true);
}

//=============================================================================
#if KEEP_PARSE_FUNCS
/**
	Parses a file as an independent module, resulting in a root body.
	No error handling here.  
	\param yyin is an already opened file.
	\return NULL on failure.
 */
count_ptr<root_body>
parse_to_AST(FILE* yyin) {
	typedef	count_ptr<root_body>		return_type;
	YYSTYPE lval;			// root token (was yyval)
	NEVER_NULL(yyin);
	try {
		hackt_parse(NULL, lval, yyin);
	} catch (...) {
		return return_type(NULL);
	}
	return return_type(lval._root_body);
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
		hackt_parse(NULL, hackt_val, yyin);
	} catch (...) {
		// then it's possible that the file_manager is not balanced.  
		need_to_clean_up_file_manager = true;
	}
	} else {
		return return_type(NULL);
	}
	if (opt.make_depend) {
		const char* const md = opt.make_depend_target.c_str();
	if (check_file_writeable(md).good) {
		ofstream mtf(md);
		mtf << opt.target_object << ": " << opt.source_file;
		// list all seen files' full paths
		hackt_parse_file_manager.make_depend(mtf, opt.source_file);
	} else {
		// already have error message
		return return_type(NULL);
	}
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
check_AST(const root_body& r, const char* name) {
	typedef	count_ptr<module>	return_type;
	STACKTRACE_VERBOSE;
	return_type mod(new module(name));
	NEVER_NULL(mod);
	parse_context pc(*mod);
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
	\param b pass true to use search paths, 
		false to use path to file as is.  
 */
good_bool
flatten_source(const char* name) {
	STACKTRACE_VERBOSE;
	const yyin_manager ym(hackt_parse_file_manager, name, false);
	FILE* yyin = ym.get_file();
	if (yyin) {
		const bool need_to_clean_up_file_manager =
			!lexer::__flatten_source(yyin).good;
#if 0
		if (opt.make_depend) {
			const char* const md = opt.make_depend_target.c_str();
		if (check_file_writeable(md).good) {
			ofstream mtf(md);
			mtf << opt.target_object << ": " << opt.source_file;
			// list all seen files' full paths
			hackt_parse_file_manager.make_depend(mtf, opt.source_file);
		} else {
			// already have error message
			return return_type(NULL);
		}
		}
#endif
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
		}
	}
	// error message would be nice
	const count_ptr<root_body> AST(parse_to_AST(name, opt));
	if (!AST) return return_type(NULL);
	// error message would be nice
	return check_AST(*AST, name ? name : dflt);
}
#endif

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
	Saves module to object file.  
 */
void
save_module(const module& m, const char* name) {
	STACKTRACE_VERBOSE;
	const string fname(name);
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
	const bool _d = persistent_object_manager::dump_reconstruction_table;
	const bool _u = persistent::warn_unimplemented;
	persistent::warn_unimplemented = true;
	persistent_object_manager::dump_reconstruction_table = d;
	save_module(m, name);
	// these will not be restored if there is an exception
	persistent_object_manager::dump_reconstruction_table = _d;
	persistent::warn_unimplemented = _u;
}

//=============================================================================
/**
	Loads object file and reconstructs module in memory.  
 */
count_ptr<module>
load_module(const char* fname) {
	STACKTRACE_VERBOSE;
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
	persistent::warn_unimplemented = true;
	persistent_object_manager::dump_reconstruction_table = true;
	return load_module(fname);
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

DEFAULT_STATIC_TRACE_END

