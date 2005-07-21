/**
	\file "main/main_funcs.cc"
	Useful main-level functions to call.
	Indent to hide most complexity here, exposing a bare-bones
	set of public callable functions.  
	$Id: main_funcs.cc,v 1.2.16.1 2005/07/21 20:44:14 fang Exp $
 */

#include <iostream>
#include <fstream>

#define	ENABLE_STACKTRACE		0
#define	ENABLE_STATIC_TRACE		0

#include "util/macros.h"
#include "main/main_funcs.h"
#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include "AST/art_parser_root.h"	// for parser::root_body
#include "AST/parse_context.h"		// for parser::context
#include "util/getopt_portable.h"
#include "util/persistent_object_manager.h"

// forward declarations needed for YSTYPE
#include "parser/yacc_or_bison.h"	// configuration-generated
#include "parser/art++-parse-prefix.h"	// for YYSTYPE
using util::memory::excl_ptr;

#if	USING_YACC
extern	YYSTYPE artxx_val;		// root token (was yyval)
#elif	USING_BISON
extern	excl_ptr<ART::parser::root_body>	AST_root;
#else
#error	"USING_YACC or USING_BISON?  One must be set by configuration."
#endif
extern	FILE*	artxx_in;
extern	int	artxx_parse(void);

#include "util/stacktrace.h"

//=============================================================================
namespace ART {
using std::ifstream;
using std::ofstream;
using std::ios_base;
using parser::root_body;
using util::persistent;
using util::persistent_object_manager;
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
	\return 
 */
good_bool
check_file_writeable(const char* fname) {
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
 */
static
excl_ptr<root_body>
parse_to_AST(FILE* f) {
	typedef	excl_ptr<root_body>		return_type;
	return_type root;
	// artxx_in is the global yyin FILE*.  
	artxx_in = f ? f : stdin;
	try {
		artxx_parse();
	} catch (...) {
		return return_type(NULL);
	}
	fclose(artxx_in);		// even if stdin?
#if USING_YACC
	return return_type(artxx_val._root_body);
#else
	return AST_root;
#endif
}

//=============================================================================
/**
	Phase 1 of type-check.  
 */
static
excl_ptr<module>
check_AST(const root_body& r, const char* name) {
	typedef	excl_ptr<module>	return_type;
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
	static const char* dflt = "-stdin-";
	FILE* f = name ? open_source_file(name) : stdin;
	if (!f)	return return_type(NULL);
	// error message would be nice
	excl_ptr<root_body> AST = parse_to_AST(f);
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

