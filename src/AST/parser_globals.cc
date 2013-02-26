/**
	\file "AST/parser_globals.cc"
 */

#include "AST/parser_globals.hh"
#include "AST/root.hh"
#include "lexer/input_manager.hh"
#include "lexer/file_manager.hh"
#include "lexer/flex_lexer_state.hh"
#include "parser/hackt-parse-real.hh"	// for YYSTYPE
#include "util/memory/count_ptr.tcc"
#include "util/stacktrace.hh"

extern	int	hackt_parse(void*, YYSTYPE&, flex::lexer_state&);

/**
	This is the file stack and include path manager for 
	the hackt parser.  
	This is globally visible and accessible (unfortunately).  
 */
HAC::lexer::file_manager
hackt_parse_file_manager;

/**
	Auxiliary stack for file-embedding construct, whereby the
	lexer pushes a fake file onto this stack, and the parser
	pops it off the stack when the file construct is reduced.  
 */
HAC::lexer::embedded_file_stack_type
hackt_embedded_file_stack;

namespace HAC {
using util::memory::count_ptr;
using parser::root_body;

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

}	// end namespace HAC
