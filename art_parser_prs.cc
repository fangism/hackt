// "art_parser_prs.cc"

#include "art_parser_template_methods.h"
#include "art_parser_prs.h"

#define	CONSTRUCTOR_INLINE
#define	DESTRUCTOR_INLINE

namespace ART {
namespace parser {
namespace PRS {
//=============================================================================
// class rule method definitions

CONSTRUCTOR_INLINE
rule::rule(node* g, node* a, node* rhs, node* d) : node(),
		guard(IS_A(expr*, g)),
		arrow(IS_A(terminal*, a)),
		r(IS_A(expr*, rhs)),
		dir(IS_A(terminal*, d)) {
	assert(guard); assert(arrow); assert(r); assert(dir);
	assert(IS_A(id_expr*, r) || IS_A(postfix_expr*, r));
}

DESTRUCTOR_INLINE
rule::~rule() {
	SAFEDELETE(guard); SAFEDELETE(arrow);
	SAFEDELETE(r); SAFEDELETE(dir);
}

ostream&
rule::what(ostream& o) const {
	return o << "(prs-rule)";
}

line_position
rule::leftmost(void) const {
	return guard->leftmost();
}

line_position
rule::rightmost(void) const {
	return dir->rightmost();
}

//=============================================================================
// class body method definitions

CONSTRUCTOR_INLINE
body::body(node* t, node* r) : language_body(t),
		rules(IS_A(rule_list*, r)) {
	if (r) assert(rules);
}

DESTRUCTOR_INLINE
body::~body() {
	SAFEDELETE(rules);
}

ostream&
body::what(ostream& o) const { return o << "(prs-body)"; }

line_position
body::leftmost(void) const {
	return language_body::leftmost();
}

line_position
body::rightmost(void) const {
	return rules->rightmost();
}

//=============================================================================
// EXPLICIT TEMPLATE INSTANTIATIONS -- entire classes

template class node_list<rule>;				// PRS::rule_list

//=============================================================================
};	// end namespace PRS
};	// end namespace parser
};	// end namespace ART

#undef	CONSTRUCTOR_INLINE
#undef	DESTRUCTOR_INLINE

