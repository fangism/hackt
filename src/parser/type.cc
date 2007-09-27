/**
	\file "parser/type.cc"
	$Id: type.cc,v 1.1.64.1 2007/09/27 05:18:09 fang Exp $
 */

#include <iostream>

#include "config.h"
#include "parser/type.h"
#include "AST/AST.h"		// should be first
#include "AST/parse_context.h"
#include "parser/type-parse-real.h"
#include "Object/type/fundamental_type_reference.h"
#include "util/memory/count_ptr.tcc"
#include "util/memory/deallocation_policy.h"

#define	ENABLE_STACKTRACE		0
#include "util/stacktrace.h"

extern
int
type_parse(void*, YYSTYPE&, FILE*);

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
	NOTE: context should really be const.  
	UGLY: writes string to file first, eventually fix parser
		with iostreams instead of FILE*.  
	Code ripped from parser::parse_reference.  
 */
excl_ptr<const concrete_type_ref>
parse_complete_type(const char* t) {
	STACKTRACE_VERBOSE;
	typedef count_ptr<FILE, util::memory::FILE_tag> FILE_ptr;
	typedef excl_ptr<const concrete_type_ref>	return_type;
	NEVER_NULL(t);
	STACKTRACE_INDENT_PRINT("type = " << t << endl);
	const FILE_ptr temp(tmpfile());
	if (!temp) {
		cerr << "Failed to create temporary file-buffer!" << endl;
		THROW_EXIT;
	}
	// see implementation comments in parser::parse_reference.
	if (fputs(t, &*temp) == EOF) {
		cerr << "Error writing string to temporary file." << endl;
		THROW_EXIT;
	} else {
		STACKTRACE_INDENT_PRINT("attempting parsing..." << endl);
		fputc('\n', &*temp);
		// fflush(&*temp);
		rewind(&*temp);         // same as fseek(temp, 0, SEEK_SET);
		YYSTYPE lval;
		lval._concrete_type_ref = NULL;	// initialize
		try {
			type_parse(NULL, lval, &*temp);
		} catch (...) {
			cerr << "Error parsing type: " << t << endl;
			return return_type(NULL);;
		}
		// cerr << "parsed node name successfully... " << endl;
		// here is our mini-parse-tree:
		return return_type(lval._concrete_type_ref);
	}
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
	const context c(m, parse_options(), true);
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

