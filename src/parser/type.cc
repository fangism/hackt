/**
	\file "parser/type.cc"
	$Id: type.cc,v 1.4 2008/03/21 00:20:33 fang Exp $
 */

#include <iostream>

#include "config.h"
#include "parser/type.hh"
#include "parser/instref.hh"
#include "AST/AST.hh"		// should be first
#include "AST/parse_context.hh"
#include "parser/type-parse-real.hh"
#include "lexer/flex_lexer_state.hh"
#include "Object/type/fundamental_type_reference.hh"
#include "Object/type/process_type_reference.hh"
#include "Object/def/footprint.hh"
#include "Object/common/namespace.hh"
#include "Object/module.hh"
#include "util/memory/count_ptr.tcc"

#define	ENABLE_STACKTRACE		0
#include "util/stacktrace.hh"

extern
int
type_parse(void*, YYSTYPE&, flex::lexer_state&);

//=============================================================================
namespace HAC {
namespace entity {
	class module;
}
namespace parser {
#include "util/using_ostream.hh"
using std::string;
using std::pair;
using entity::process_type_reference;

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
	po.view_all_publicly = true;
	const context c(const_cast<module&>(m), po);	// kludge
//	const context c(m, po, true);
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Lookups up a process/definition footprint from the top-level module.
	\return NULL on error
 */
const footprint*
parse_to_footprint(const char* t, const module& m) {
	STACKTRACE_VERBOSE;
	// type parse already handles namespace
	const count_ptr<const fundamental_type_reference>
		tr(parse_and_check_complete_type(t, m));
	if (!tr) {
		// cerr << ...
		return NULL;
	}
	const count_ptr<const process_type_reference>
		ptr(tr.is_a<const process_type_reference>());
	if (!ptr) {
		cerr << "Definition " << t <<
			" doesn't refer to a process or struct." << endl;
		return NULL;
	}
	// return pointer to footprint
	return ptr->lookup_footprint();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param is a reference of the form: type::instance or instance (top-level)
 */
pair<const footprint*, size_t>
parse_type_local_node(const string& r, const module& m) {
	STACKTRACE_VERBOSE;
	typedef pair<const footprint*, size_t>		return_type;
	// find last "::" if any
	size_t l = r.find_last_of("::");
	// find *last* because type may include :: namespace separators
	if (l == string::npos) {
		// is only a top-level reference
		const footprint* f = &m.get_footprint();
		const bool_index bi(parse_node_to_index(r, *f));
		if (bi.valid()) {
			return f->get_instance_owner<bool_tag>(bi.index);
		}
	} else {
		// chop up string to type::inst
		const string ts(r.substr(0, l));
		const string ir(r.substr(l+2));
		const footprint* f = parse_to_footprint(ts.c_str(), m);
		if (f) {
			const bool_index bi(parse_node_to_index(r, *f));
			if (bi.valid()) {
				return f->get_instance_owner<bool_tag>(bi.index);
			}
		}
	}
	return return_type(NULL, 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}	// end namespace parser
}	// end namespace HAC

