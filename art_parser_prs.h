// "art_parser_prs.h"
// PRS-specific syntax tree

#include "art_macros.h"

namespace ART {
namespace parser {
//=============================================================================
// forward declarations
class expr;
class terminal;
class language_body;

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
	rule(node* g, node* a, node* rhs, node* d);
virtual	~rule();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};

typedef node_list<rule>		rule_list;
#define prs_rule_list_wrap(b,l,e)					\
	IS_A(PRS::rule_list*, l)->wrap(b,e)
#define prs_rule_list_append(l,d,n)					\
	IS_A(PRS::rule_list*, l)->append(d,n)


//=============================================================================
/// a collection of production rules
class body : public language_body {
protected:
	rule_list*		rules;
public:
	body(node* t, node* r);
virtual	~body();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};

//=============================================================================
};	// end namespace PRS
};	// end namespace parser
};	// end namespace ART

