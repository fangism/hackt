// "art_parser_chp.h"
// CHP-specific syntax tree

#include "art_macros.h"
#include "art_parser.h"

namespace ART {
namespace parser {
/**
	This is the namespace for the CHP sub-language.  
 */
namespace CHP {

//=============================================================================
// forward declarations

//=============================================================================
/// for now, just a carbon copy of expr class type, type-check later
typedef	expr	chp_expr;


//=============================================================================
/// CHP statement base class
class statement : virtual public ART::parser::statement {
public:
	statement() : ART::parser::statement() { }
virtual	~statement() { }

virtual	ostream& what(ostream& o) const { return o << "(chp-statement)"; }
virtual	line_position leftmost(void) const = 0;
virtual	line_position rightmost(void) const = 0;
};

typedef	node_list<statement,semicolon>	stmt_list;

#define chp_stmt_list_wrap(b,l,e)					\
	dynamic_cast<CHP::stmt_list*>(l)->wrap(b,e)
#define chp_stmt_list_append(l,d,n)					\
	dynamic_cast<CHP::stmt_list*>(l)->append(d,n)

//=============================================================================
/// CHP body is just a list of statements
class body : public language_body {
protected:
	stmt_list*		stmts;		///< list of CHP statements
public:
	body(node* t, node* s) : language_body(t), 
		stmts(dynamic_cast<stmt_list*>(s))
		{ if(s) assert(stmts); }
virtual	~body() { SAFEDELETE(stmts); }

virtual	ostream& what(ostream& o) const { return o << "(chp-body)"; }
virtual	line_position leftmost(void) const
	{ return language_body::leftmost(); }
virtual	line_position rightmost(void) const
	{ return stmts->rightmost(); }
};


//=============================================================================
/// CHP guarded command contains an expression condition and body
class guarded_command : public nonterminal {
protected:
	chp_expr*		guard;		///< guard expression
	terminal*		arrow;		///< right-arrow
	body*			command;	///< statement body
public:
	guarded_command(node* g, node* a, node* c) : nonterminal(), 
		guard(dynamic_cast<chp_expr*>(g)), 
			// remember, may be keyword: else
		arrow(dynamic_cast<terminal*>(a)), 
		command(dynamic_cast<body*>(c)) { }
virtual	~guarded_command() { SAFEDELETE(guard); 
		SAFEDELETE(arrow); SAFEDELETE(command); }

virtual	ostream& what(ostream& o) const { return o << "(chp-guarded-cmd)"; }
virtual	line_position leftmost(void) const
	{ return guard->leftmost(); }
virtual	line_position rightmost(void) const
	{ return command->rightmost(); }
};

//=============================================================================
/// CHP else-clause is just a special case of a guarded_command
class else_clause : public guarded_command {
public:
	else_clause(node* g, node* a, node* c) : guarded_command(g,a,c) {
		// check for keyword else, right-arrow terminal
		}
virtual	~else_clause() { }

virtual	ostream& what(ostream& o) const { return o << "(chp-else-clause)"; }
};

//=============================================================================
/// CHP skip statement
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

virtual	ostream& what(ostream& o) const { return o << "(chp-skip)"; }
virtual	line_position leftmost(void) const
	{ return token_keyword::leftmost(); }
virtual	line_position rightmost(void) const
	{ return token_keyword::rightmost(); }
virtual	line_range where(void) const
	{ return token_keyword::where(); }
};

//=============================================================================
/// CHP wait contains just an expression
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

virtual	ostream& what(ostream& o) const { return o << "(chp-wait)"; }
virtual	line_position leftmost(void) const { return lb->leftmost(); }
virtual	line_position rightmost(void) const { return rb->rightmost(); }
};

//=============================================================================
/// CHP assignment statement in binary form
class assignment : public statement, public assign_stmt {
private:
	typedef ART::parser::assign_stmt	base_assign;
public:
	assignment(base_assign* a) : ART::parser::CHP::statement(), 
		// destructive transfer of ownership
		assign_stmt(a->release_lhs(), a->release_op(), 
			a->release_rhs()) {
		SAFEDELETE(a);
	}
virtual	~assignment() { }

// remember to type check in CHP language mode

virtual	ostream& what(ostream& o) const { return o << "(chp-assignment)"; }
virtual	line_position leftmost(void) const
	{ return assign_stmt::leftmost(); }
virtual	line_position rightmost(void) const
	{ return assign_stmt::rightmost(); }
virtual	line_range where(void) const
	{ return assign_stmt::where(); }
};

//-----------------------------------------------------------------------------
/// CHP assignment statement is only boolean
class incdec_stmt : public statement, public parser::incdec_stmt {
private:
	typedef ART::parser::incdec_stmt	base_assign;
public:
	incdec_stmt(base_assign* a) : ART::parser::CHP::statement(),
		// destructive transfer of ownership
		parser::incdec_stmt(a->release_expr(), a->release_op()) {
		SAFEDELETE(a);
	}
virtual ~incdec_stmt() { }

// remember to type check in CHP language mode

virtual ostream& what(ostream& o) const { return o << "(chp-assignment)"; }
virtual line_position leftmost(void) const   
	{ return incdec_stmt::leftmost(); }
virtual line_position rightmost(void) const
	{ return incdec_stmt::rightmost(); }
virtual line_range where(void) const
	{ return incdec_stmt::where(); }
};

//=============================================================================
/// CHP communication action base class
class communication : public statement {
protected:
	expr*		chan;
	token_char*	dir;
public:
	communication(node* c, node* d) : statement(), 
		chan(dynamic_cast<expr*>(c)), 
		dir(dynamic_cast<token_char*>(d))
		{ assert(chan); assert(dir); }
virtual	~communication() { SAFEDELETE(chan); SAFEDELETE(dir); }

virtual	line_position leftmost(void) const { return chan->leftmost(); }
};

typedef	node_list<communication,comma>		comm_list;

#define chp_comm_list_wrap(b,l,e)					\
	dynamic_cast<CHP::det_selection*>(l)->wrap(b,e)
#define chp_comm_list_append(l,d,n)					\
	dynamic_cast<CHP::det_selection*>(l)->append(d,n)


//-----------------------------------------------------------------------------
/// CHP send action
class send : public communication {
protected:
	expr_list*	rvalues;
public:
	send(node* c, node* d, node* r) : communication(c, d), 
		rvalues(dynamic_cast<expr_list*>(r))
		{ assert(rvalues); }
virtual	~send() { SAFEDELETE(rvalues); }

virtual	line_position rightmost(void) const
	{ return rvalues->rightmost(); }
};

//-----------------------------------------------------------------------------
/// CHP receive action
class receive : public communication {
protected:
	expr_list*	lvalues;
public:
	receive(node* c, node* d, node* l) : communication(c, d), 
		lvalues(dynamic_cast<expr_list*>(l))
		{ assert(lvalues); }
virtual	~receive() { SAFEDELETE(lvalues); }

virtual	line_position rightmost(void) const
	{ return lvalues->rightmost(); }
};

//=============================================================================
/// CHP selection statement abstract base class
class selection : public statement {
// is this class even necessary?
public:
	selection() : statement() { }
virtual	~selection() { }

virtual	ostream& what(ostream& o) const { return o << "(chp-selection)"; }
};

//=============================================================================
/// container for deterministic selection statement
class det_selection : public selection, 
		public node_list<guarded_command,thickbar> {
private:
	typedef	node_list<guarded_command,thickbar>	det_sel_base;
public:
	det_selection(node* n) : selection(), 
		node_list<guarded_command,thickbar>(n) { }
virtual	~det_selection() { }

virtual	ostream& what(ostream& o) const { return o << "(chp-det-sel)"; }
virtual	line_position leftmost(void) const
	{ return det_sel_base::leftmost(); }
virtual	line_position rightmost(void) const
	{ return det_sel_base::rightmost(); }
virtual	line_range where(void) const
	{ return det_sel_base::where(); }
};

#define chp_det_selection_wrap(b,l,e)					\
	dynamic_cast<CHP::det_selection*>(l)->wrap(b,e)
#define chp_det_selection_append(l,d,n)					\
	dynamic_cast<CHP::det_selection*>(l)->append(d,n)

//=============================================================================
/// container for non-deterministic selection statement
class nondet_selection : public selection, 
		public node_list<guarded_command,colon> {
private:
	typedef	node_list<guarded_command,colon>	nondet_sel_base;
public:
	nondet_selection(node* n) : selection(), 
		node_list<guarded_command,colon>(n) { }
virtual	~nondet_selection() { }

virtual	ostream& what(ostream& o) const { return o << "(chp-nondet-sel)"; }
virtual	line_position leftmost(void) const
	{ return nondet_sel_base::leftmost(); }
virtual	line_position rightmost(void) const
	{ return nondet_sel_base::rightmost(); }
virtual	line_range where(void) const
	{ return nondet_sel_base::where(); }
};

#define chp_nondet_selection_wrap(b,l,e)				\
	dynamic_cast<CHP::nondet_selection*>(l)->wrap(b,e)
#define chp_nondet_selection_append(l,d,n)				\
	dynamic_cast<CHP::nondet_selection*>(l)->append(d,n)

//=============================================================================
/// container for probablistic selection statement
class prob_selection : public selection, 
		public node_list<guarded_command,thickbar> {
private:
	typedef	node_list<guarded_command,thickbar>	prob_sel_base;
public:
	prob_selection(node* n) : selection(), 
		node_list<guarded_command,thickbar>(n) { }
virtual	~prob_selection() { }

virtual	ostream& what(ostream& o) const { return o << "(chp-prob-sel)"; }
virtual	line_position leftmost(void) const
	{ return prob_sel_base::leftmost(); }
virtual	line_position rightmost(void) const
	{ return prob_sel_base::rightmost(); }
virtual	line_range where(void) const
	{ return prob_sel_base::where(); }
};

#define chp_prob_selection_wrap(b,l,e)					\
	dynamic_cast<CHP::prob_selection*>(l)->wrap(b,e)
#define chp_prob_selection_append(l,d,n)				\
	dynamic_cast<CHP::prob_selection*>(l)->append(d,n)

//=============================================================================
/// CHP loop contains a list of statements
class loop : public statement {
protected:
	body*			command;
public:
	loop(node* n) : statement(), command(dynamic_cast<body*>(n)) { }
virtual	~loop() { SAFEDELETE(command); }

virtual	ostream& what(ostream& o) const { return o << "(chp-loop)"; }
virtual	line_position leftmost(void) const { return command->leftmost(); }
virtual	line_position rightmost(void) const { return command->rightmost(); }
};

//=============================================================================
/// CHP do-until: re-enter selection statement until all guards are false
class do_until : public statement {
protected:
	det_selection*		sel;
public:
	do_until(node* n) : statement(), 
		sel(dynamic_cast<det_selection*>(n)) { }
virtual	~do_until() { SAFEDELETE(sel); }

// type-check: cannot contain an else clause, else infinite loop!

virtual	ostream& what(ostream& o) const { return o << "(chp-do-until)"; }
virtual	line_position leftmost(void) const { return sel->leftmost(); }
virtual	line_position rightmost(void) const { return sel->rightmost(); }
};

//=============================================================================
/// CHP log statement
class log : public statement {
protected:
	token_keyword*		lc;
	expr_list*		args;
public:
	log(node* l, node* n) : statement(), 
		lc(dynamic_cast<token_keyword*>(l)), 
		args(dynamic_cast<expr_list*>(n))
		{ assert(lc); assert(args); }
virtual	~log() { SAFEDELETE(lc); SAFEDELETE(args); }

virtual	ostream& what(ostream& o) const { return o << "(chp-log)"; }
virtual	line_position leftmost(void) const { return lc->leftmost(); }
virtual	line_position rightmost(void) const { return args->rightmost(); }
};

//=============================================================================
};	// end namespace CHP
};	// end namespace parser
};	// end namespace ART

