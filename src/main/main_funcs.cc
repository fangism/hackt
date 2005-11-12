/**
	\file "main/main_funcs.cc"
	Useful main-level functions to call.
	Indent to hide most complexity here, exposing a bare-bones
	set of public callable functions.  
	$Id: main_funcs.cc,v 1.4.2.3 2005/11/12 01:52:44 fang Exp $
 */

#include <iostream>
#include <fstream>

#define	ENABLE_STACKTRACE		0
#define	ENABLE_STATIC_TRACE		0

#include "config.h"			// for USING_BISON / USING_YACC

#include "util/macros.h"
#include "main/main_funcs.h"
#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include "AST/art_parser_root.h"	// for parser::root_body
#include "AST/parse_context.h"		// for parser::context
#include "util/getopt_portable.h"
#include "util/persistent_object_manager.h"

// forward declarations needed for YSTYPE
#include "parser/hackt-parse-real.h"	// for YYSTYPE
using util::memory::excl_ptr;

#include "lexer/file_manager.h"
#include "lexer/yyin_manager.h"
#if 0
/**
	This is the file pointer used by hackt_parse().  
 */
extern	FILE*	hackt_in;
#endif
/**
	This prototype for yyparse is either set by
	YYPARSE_PARAM for bison, or hacked by scripts for yacc.  
	Coordinate with "parser/hackt-parse-options.h".
 */
extern	int	hackt_parse(void*, YYSTYPE&, FILE*);
extern	ART::lexer::file_manager	hackt_parse_file_manager;

#include "util/stacktrace.h"

//=============================================================================
namespace ART {
using std::ifstream;
using std::ofstream;
using std::ios_base;
using parser::root_body;
using util::persistent;
using util::persistent_object_manager;
using lexer::yyin_manager;
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
	}
	outf.close();
	return good_bool(true);
}

//=============================================================================
/**
	Side-effect: sets the yyin FILE* before parsing, and closes it
		before returning.  
	\param c the name associated with this file, may be NULL for stdin.  
	\return allocated AST.
 */
static
excl_ptr<root_body>
parse_to_AST(const char* c) {
	typedef	excl_ptr<root_body>		return_type;
	STACKTRACE_VERBOSE;
	YYSTYPE hackt_val;		// root token (was yyval)
	hackt_val._root_body = NULL;
	// error status
	bool need_to_clean_up_file_manager = false;
{
	// hackt_in WAS the global yyin FILE*.  
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
}
	if (need_to_clean_up_file_manager) {
		hackt_parse_file_manager.reset();
		return return_type(NULL);
	}
	return return_type(hackt_val._root_body);
}

//=============================================================================
/**
	Phase 1 of type-check.  
 */
static
excl_ptr<module>
check_AST(const root_body& r, const char* name) {
	typedef	excl_ptr<module>	return_type;
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

//=============================================================================
/**
	Parses a file and checks it.  
	\param name the name of the top-level source file, 
		if NULL, then will use stdin.  
	\return if parse and type-check are successful, then return
		a pointer to top-level constructed module, else NULL.  
 */
excl_ptr<module>
parse_and_check(const char* name) {
	typedef	excl_ptr<module>	return_type;
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
	excl_ptr<root_body> AST = parse_to_AST(name);
	if (!AST) return return_type(NULL);
	// error message would be nice
	return check_AST(*AST, name ? name : dflt);
}

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
	const excl_ptr<module> module_copy =
		persistent_object_manager::self_test_no_file(m)
			.is_a_xfer<module>();
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
	Side-effect: sets debugging flags (sticky) for persistent
	and persistent_object_manager classes.  
 */
void
save_module_debug(const module& m, const char* name) {
	STACKTRACE_VERBOSE;
	persistent::warn_unimplemented = true;
	persistent_object_manager::dump_reconstruction_table = true;
	save_module(m, name);
}

//=============================================================================
/**
	Loads object file and reconstructs module in memory.  
 */
excl_ptr<module>
load_module(const char* fname) {
	STACKTRACE_VERBOSE;
	return persistent_object_manager::load_object_from_file(fname)
		.is_a_xfer<module>();
}

//=============================================================================
/**
	Loads object file and reconstructs module in memory.  
	Side-effect: sets debugging flags (sticky) for persistent
	and persistent_object_manager classes.  
 */
excl_ptr<module>
load_module_debug(const char* fname) {
	STACKTRACE_VERBOSE;
	persistent::warn_unimplemented = true;
	persistent_object_manager::dump_reconstruction_table = true;
	return load_module(fname);
}

//=============================================================================
// getopt-related utility functions

/**
	Standard bad option error message.  
 */
void
unknown_option(const int o) {
       if (isprint(optopt))
		cerr << "Unknown option `-" << char(optopt) << "'." << endl;
	else cerr << "Unknown option character `" <<
		reinterpret_cast<void*>(optopt) << "'." << endl;
}

//=============================================================================
}	// end namespace ART

DEFAULT_STATIC_TRACE_END

