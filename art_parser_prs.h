// "art_parser_prs.h"
// PRS-specific syntax tree

#include "art_macros.h"
#include "art_parser.h"

namespace ART {
namespace parser {
/**
	This is the namespace for the PRS sub-language.  
 */
namespace PRS {

//=============================================================================
// forward declarations

//=============================================================================
/// for now, just a carbon copy of expr class type, type-check later
typedef	expr	prs_expr;

//=============================================================================
/// a single production rule
class rule : public expr {
protected:
	prs_expr*	guard;
	terminal*	arrow;
	prs_expr*	r;
	terminal*	dir;
public:
	rule(node* g, node* a, node* rhs, node* d) : expr(), 
		guard(dynamic_cast<prs_expr*>(g)), 
		arrow(dynamic_cast<terminal*>(a)), 
		r(dynamic_cast<prs_expr*>(rhs)), 
		dir(dynamic_cast<terminal*>(dir)) {
			// various assertion statments here
		}
virtual	~rule() { SAFEDELETE(guard); SAFEDELETE(arrow);
		SAFEDELETE(r); SAFEDELETE(dir); }

virtual	ostream& what(ostream& o) const { return o << "(prs-rule)"; }
virtual	line_position leftmost(void) const { return guard->leftmost(); }
virtual	line_position rightmost(void) const { return dir->rightmost(); }
};

typedef node_list<rule>		rule_list;
#define prs_rule_list_wrap(b,l,e)					\
	dynamic_cast<PRS::rule_list*>(l)->wrap(b,e)
#define prs_rule_list_append(l,d,n)					\
	dynamic_cast<PRS::rule_list*>(l)->append(d,n)


//=============================================================================
/// a collection of production rules
class body : public language_body {
protected:
	rule_list*		rules;
public:
	body(node* t, node* r) : language_body(t), 
		rules(dynamic_cast<rule_list*>(r))
		{ if (r) assert(rules); }
virtual	~body() { SAFEDELETE(rules); }

virtual	ostream& what(ostream& o) const { return o << "(prs-body)"; }
virtual	line_position leftmost(void) const
	{ return language_body::leftmost(); }
virtual	line_position rightmost(void) const
	{ return rules->rightmost(); }
};

//=============================================================================
};	// end namespace PRS
};	// end namespace parser
};	// end namespace ART

