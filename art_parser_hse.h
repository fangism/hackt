// "art_parser_hse.h"
// HSE-specific syntax tree

#include "art_macros.h"
#include "art_parser.h"

namespace ART {
namespace parser {
/**
	This is the namespace for the HSE sub-language.  
 */
namespace HSE {

//=============================================================================
// forward declarations

//=============================================================================
/// for now, just a carbon copy of expr class type, type-check later
typedef	expr	hse_expr;


//=============================================================================
/// HSE statement base class
class statement : virtual public ART::parser::statement {
public:
	statement() : ART::parser::statement() { }
virtual	~statement() { }

virtual	ostream& what(ostream& o) const { return o << "(hse-statement)"; }
};

//=============================================================================
/// HSE body is just a list of statements
class body : public language_body, public node_list<statement> {
public:
	body(node* r) : language_body(NULL), node_list<statement>(r) { }
virtual	~body() { }

virtual	ostream& what(ostream& o) const { return o << "(hse-body)"; }
};

#define hse_body_tag_wrap(t,l,b,r)					\
	dynamic_cast<HSE::body*>(					\
		dynamic_cast<HSE::body*>(b)->attach_tag(t))->wrap(l,r)

#define hse_body_wrap(b,l,e)						\
	dynamic_cast<HSE::body*>(l)->wrap(b,e)
#define hse_body_append(l,d,n)						\
	dynamic_cast<HSE::body*>(l)->append(d,n)


//=============================================================================
/// HSE guarded command contains an expression condition and body
class guarded_command : public nonterminal {
protected:
	hse_expr*		guard;
	terminal*		arrow;
	body*			command;
public:
	guarded_command(node* g, node* a, node* c) : nonterminal(), 
		guard(dynamic_cast<hse_expr*>(g)), 
			// remember, may be keyword: else
		arrow(dynamic_cast<terminal*>(a)), 
		command(dynamic_cast<body*>(c)) { }
virtual	~guarded_command() { SAFEDELETE(guard); 
		SAFEDELETE(arrow); SAFEDELETE(command); }

virtual	ostream& what(ostream& o) const { return o << "(hse-guarded-cmd)"; }
};

//=============================================================================
/// HSE else-clause is just a special case of a guarded_command
class else_clause : public guarded_command {
public:
	else_clause(node* g, node* a, node* c) : guarded_command(g,a,c) {
		// check for keyword else, right-arrow terminal
		}
virtual	~else_clause() { }

virtual	ostream& what(ostream& o) const { return o << "(hse-else-clause)"; }
};

//=============================================================================
/// HSE skip statement
class skip : public statement, public token_keyword {
public:
/**
	Constructor takes a plain keyword token and re-wraps the string
	containing "skip", which effectively casts this as a sub-class.  
 */
	skip(node* s) : statement(), 
		token_keyword(dynamic_cast<token_keyword*>(s)->c_str()) 
		{ SAFEDELETE(s); }
virtual	~skip() { }

// check that nothing appears after skip statement

virtual	ostream& what(ostream& o) const { return o << "(hse-skip)"; }
};

//=============================================================================
/// HSE wait contains just an expression
class wait : public statement {
protected:
	terminal*	lb;
	expr*		cond;
	terminal*	rb;
public:
	wait(node* l, node* c, node* r) : statement(), 
		lb(dynamic_cast<terminal*>(l)), cond(dynamic_cast<expr*>(c)), 
		rb(dynamic_cast<terminal*>(r))
		{ assert(cond); assert(lb); assert(rb); }
virtual	~wait() { SAFEDELETE(lb); SAFEDELETE(cond); SAFEDELETE(rb); }

virtual	ostream& what(ostream& o) const { return o << "(hse-wait)"; }
};

//=============================================================================
/// HSE assignment statement is only boolean
class assignment : public statement, public incdec_stmt {
private:
	typedef ART::parser::incdec_stmt	base_assign;
public:
	assignment(base_assign* a) : ART::parser::HSE::statement(), 
		// destructive transfer of ownership
		incdec_stmt(a->release_expr(), a->release_op()) {
		SAFEDELETE(a);
	}
virtual	~assignment() { }

// remember to type check in HSE language mode

virtual	ostream& what(ostream& o) const { return o << "(hse-assignment)"; }
};

//=============================================================================
/// HSE selection statement abstract base class
class selection : public statement {
// is this class even necessary?
public:
	selection() : statement() { }
virtual	~selection() { }

virtual	ostream& what(ostream& o) const { return o << "(hse-selection)"; }
};

//=============================================================================
/// container for deterministic selection statement
class det_selection : public selection, 
		public node_list<guarded_command,thickbar> {
public:
	det_selection(node* n) : selection(), 
		node_list<guarded_command,thickbar>(n) { }
virtual	~det_selection() { }

virtual	ostream& what(ostream& o) const { return o << "(hse-det-sel)"; }
};

#define hse_det_selection_wrap(b,l,e)					\
	dynamic_cast<HSE::det_selection*>(l)->wrap(b,e)
#define hse_det_selection_append(l,d,n)					\
	dynamic_cast<HSE::det_selection*>(l)->append(d,n)

//=============================================================================
/// container for non-deterministic selection statement
class nondet_selection : public selection, 
		public node_list<guarded_command,colon> {
public:
	nondet_selection(node* n) : selection(), 
		node_list<guarded_command,colon>(n) { }
virtual	~nondet_selection() { }

virtual	ostream& what(ostream& o) const { return o << "(hse-nondet-sel)"; }
};

#define hse_nondet_selection_wrap(b,l,e)				\
	dynamic_cast<HSE::nondet_selection*>(l)->wrap(b,e)
#define hse_nondet_selection_append(l,d,n)				\
	dynamic_cast<HSE::nondet_selection*>(l)->append(d,n)

//=============================================================================
/// container for probablistic selection statement
class prob_selection : public selection, 
		public node_list<guarded_command,thickbar> {
public:
	prob_selection(node* n) : selection(), 
		node_list<guarded_command,thickbar>(n) { }
virtual	~prob_selection() { }

virtual	ostream& what(ostream& o) const { return o << "(hse-prob-sel)"; }
};

//=============================================================================
/// HSE loop contains a list of statements
class loop : public statement {
protected:
	body*			command;
public:
	loop(node* n) : statement(), command(dynamic_cast<body*>(n)) { }
virtual	~loop() { SAFEDELETE(command); }

virtual	ostream& what(ostream& o) const { return o << "(hse-loop)"; }
};

//=============================================================================
/// HSE do-until: re-enter selection statement until all guards are false
class do_until : public statement {
protected:
	det_selection*		sel;
public:
	do_until(node* n) : statement(), 
		sel(dynamic_cast<det_selection*>(n)) { }
virtual	~do_until() { SAFEDELETE(sel); }

// type-check: cannot contain an else clause, else infinite loop!

virtual	ostream& what(ostream& o) const { return o << "(hse-do-until)"; }
};

//=============================================================================
};	// end namespace HSE
};	// end namespace parser
};	// end namespace ART

