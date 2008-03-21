/**
	\file "parser/type.cc"
	$Id: type.cc,v 1.4 2008/03/21 00:20:33 fang Exp $
 */

#include <iostream>

#include "config.h"
#include "parser/type.h"
#include "AST/AST.h"		// should be first
#include "AST/parse_context.h"
#include "parser/type-parse-real.h"
#include "lexer/flex_lexer_state.h"
#include "Object/type/fundamental_type_reference.h"
#include "util/memory/count_ptr.tcc"

#define	ENABLE_STACKTRACE		0
#include "util/stacktrace.h"

extern
int
type_parse(void*, YYSTYPE&, flex::lexer_state&);

//=============================================================================
namespace HAC {
namespace entity {
	class module;
}
namespace parser {
#include "util/using_ostream.h"

static
excl_ptr<const concrete_type_ref>
parse_complete_type(const char*);

static
concrete_type_ref::return_type
check_complete_type(const concrete_type_ref&, const entity::module&);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Parses a string into a complete type.  
	Code ripped from parser::parse_reference.  
 */
excl_ptr<const concrete_type_ref>
parse_complete_type(const char* t) {
	typedef excl_ptr<const concrete_type_ref>	return_type;
	STACKTRACE_VERBOSE;
	YYSTYPE lval;
	try {
		flex::lexer_state f(t);	// can now parse string directly!
		type_parse(NULL, lval, f);
	} catch (...) {
		cerr << "Error parsing type: " << t << endl;
		return return_type(NULL);;
	}
	// cerr << "parsed node name successfully... " << endl;
	// here is our mini-parse-tree:
	return return_type(lval._concrete_type_ref);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks an already-parse type.  
 */
concrete_type_ref::return_type
check_complete_type(const concrete_type_ref& tr,
		const entity::module& m) {
	typedef concrete_type_ref::return_type          return_type;
	STACKTRACE_VERBOSE;
/***
	Passing "true" as the 2nd arg says we want all names 
	publicly visible, see AST::parser::context::view_all_publicly.
	This only matters once we introduce nested types.  
***/
	parse_options po;
	// allow the type-parser to access ALL definitions, even non-exported
	po.export_all = true;
	const context c(m, po, true);
	return_type r;
	try {
		// NOTE: this checks for PUBLIC members only
		// but we should allow PRIVATE references too!
		// perhaps flag through context?
		r = tr.check_type(c);
	} catch (...) {
		// temporary have shitty error-handling...
		// already have type-check error message
		return return_type();
	}
	if (!r) {
		// don't expect this message to ever happen...
		cerr << "Some other error type-checking..." << endl;
		return return_type();
	}
	return r;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// extern
concrete_type_ref::return_type
parse_and_check_complete_type(const char* t, const entity::module& m) {
	typedef concrete_type_ref::return_type          return_type;
	typedef excl_ptr<const concrete_type_ref>       lval_ptr_type;
	STACKTRACE_VERBOSE;
	const lval_ptr_type ct = parse_complete_type(t);
	if (!ct) {
		return return_type();
	}
	return check_complete_type(*ct, m);
}

}	// end namespace parser
}	// end namespace HAC

