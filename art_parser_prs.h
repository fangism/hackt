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
virtual	~rule() { if (guard) delete guard; if (arrow) delete arrow;
		if (r) delete r; if (dir) delete (dir); }

virtual	ostream& what(ostream& o) const { return o << "(prs-rule)"; }
};

//=============================================================================
/// a collection of production rules
class body : public language_body, public node_list<rule> {
protected:
public:
	body(node* r) : language_body(NULL), node_list<rule>(r) { }
virtual	~body() { }

/**
	attaches the "prs" keyword to a language body
	attach_tag();
 */

#define	prs_body_tag_wrap(t,l,b,r)					\
	dynamic_cast<PRS::body*>(					\
		dynamic_cast<PRS::body*>(b)->attach_tag(t))->wrap(l,r)

virtual	ostream& what(ostream& o) const { return o << "(prs-body)"; }
};


//=============================================================================
};	// end namespace PRS
};	// end namespace parser
};	// end namespace ART

