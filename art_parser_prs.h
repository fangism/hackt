// "art_parser_prs.h"
// PRS-specific syntax tree

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

/***
class prs_expr : public expr {
public:
	prs_expr() : expr() { }
virtual	~prs_expr() { }

virtual	ostream& what(ostream& o) const { return o << "(prs-expr)"; }
};
***/

//=============================================================================
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
virtual	~rule() { if (guard) delete guard; if (arrow) delete arrow;
		if (r) delete r; if (dir) delete (dir); }

virtual	ostream& what(ostream& o) const { return o << "(prs-rule)"; }
};

//=============================================================================
class body : public language_body, public node_list<rule> {
public:
	body(node* r) : language_body(), node_list<rule>(r) { }
virtual	~body() { }

virtual	ostream& what(ostream& o) const { return o << "(prs-body)"; }
};


//=============================================================================
};	// end namespace PRS
};	// end namespace parser
};	// end namespace ART

