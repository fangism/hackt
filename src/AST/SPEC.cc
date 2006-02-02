/**
	\file "AST/SPEC.cc"
	$Id: SPEC.cc,v 1.1.2.1 2006/02/02 22:44:53 fang Exp $
 */

#include <iostream>
#include <vector>
#include "AST/SPEC.h"
#include "AST/node_list.tcc"
#include "AST/token_string.h"
#include "AST/expr_list.h"
#include "common/TODO.h"
#include "util/stacktrace.h"

namespace HAC {
namespace parser {
namespace SPEC {
#include "util/using_ostream.h"
//=============================================================================
// class directive method definitions

directive::directive(const token_identifier* n, const inst_ref_expr_list* a) :
		name(n), args(a) {
	NEVER_NULL(name); NEVER_NULL(args);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
directive::~directive() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PARSER_WHAT_DEFAULT_IMPLEMENTATION(directive)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
line_position
directive::leftmost(void) const { return name->leftmost(); }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
line_position
directive::rightmost(void) const { return args->rightmost(); }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// check definition

//=============================================================================
// class body method definitions

body::body(const generic_keyword_type* t, const directive_list* d) :
		language_body(t), directives(d) {
	NEVER_NULL(directives);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
body::~body() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PARSER_WHAT_DEFAULT_IMPLEMENTATION(body)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
line_position
body::leftmost(void) const { return language_body::leftmost(); }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
line_position
body::rightmost(void) const { return directives->rightmost(); }

//=============================================================================
never_ptr<const object>
body::check_build(context& c) const {
	STACKTRACE_VERBOSE;
	FINISH_ME(Fang);
	return never_ptr<const object>(NULL);
}

//=============================================================================
// EXPLICIT TEMPLATE INSTANTIATIONS

// template class node_list<const directive>;
template node_list<const directive>::node_list(const SPEC::directive*);
template ostream& node_list<const directive>::what(ostream&) const;
template line_position node_list<const directive>::leftmost(void) const;
template line_position node_list<const directive>::rightmost(void) const;

//=============================================================================
}	// end namespace SPEC
}	// end namespace parser
}	// end namespace HAC

