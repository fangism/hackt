// "art_parser_chp.h"
// CHP-specific syntax tree

#ifndef	__ART_PARSER_CHP_H__
#define	__ART_PARSER_CHP_H__

#include "art_macros.h"

namespace ART {
namespace parser {
//=============================================================================
// forward declarations
class node;
class statement;
class expr;
class language_body;
class terminal;
class assign_stmt;
class incdec_stmt;

class terminal;
class token_else;

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
class statement : virtual public node {
public:
	statement();
virtual	~statement();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const = 0;
virtual	line_position rightmost(void) const = 0;
};

typedef	node_list<statement,semicolon>	stmt_list;

#define chp_stmt_list_wrap(b,l,e)					\
	IS_A(CHP::stmt_list*, l->wrap(b,e))
#define chp_stmt_list_append(l,d,n)					\
	IS_A(CHP::stmt_list*, l->append(d,n))

//=============================================================================
/// CHP body is just a list of statements
class body : public language_body {
protected:
	stmt_list*		stmts;		///< list of CHP statements
public:
	body(token_keyword* t, stmt_list* s);
virtual	~body();

virtual	ostream& what(ostream& o) const;
using	language_body::leftmost;
virtual	line_position rightmost(void) const;
};

//=============================================================================
/// CHP guarded command contains an expression condition and body
class guarded_command : public node {
protected:
	chp_expr*		guard;		///< guard expression
	terminal*		arrow;		///< right-arrow
	stmt_list*		command;	///< statement body
public:
	guarded_command(chp_expr* g, terminal* a, stmt_list* c);
virtual	~guarded_command();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};

//=============================================================================
/// CHP else-clause is just a special case of a guarded_command
class else_clause : public guarded_command {
public:
	else_clause(token_else* g, terminal* a, stmt_list* c);
virtual	~else_clause();

	ostream& what(ostream& o) const;
};

//=============================================================================
/// CHP skip statement
class skip : public statement, public token_keyword {
public:
/**
	Constructor takes a plain keyword token and re-wraps the string
	containing "skip", which effectively casts this as a sub-class.  
 */
	skip(token_keyword* s);
virtual	~skip();

// check that nothing appears after skip statement

	ostream& what(ostream& o) const;
	line_position leftmost(void) const;
	line_position rightmost(void) const;
using	token_keyword::where;
};

//=============================================================================
/// CHP wait contains just an expression
class wait : public statement {
protected:
	terminal*	lb;			///< left bracket
	expr*		cond;			///< wait until condition
	terminal*	rb;			///< right bracket
public:
	wait(terminal* l, expr* c, terminal* r);
virtual	~wait();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};

//=============================================================================
/// CHP assignment statement in binary form
class assignment : public statement, public assign_stmt {
private:
	typedef ART::parser::assign_stmt	base_assign;
public:
	assignment(base_assign* a);
virtual	~assignment();

// remember to type check in CHP language mode

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
using	assign_stmt::where;
};

//-----------------------------------------------------------------------------
/// CHP assignment statement is only boolean
class incdec_stmt : public statement, public parser::incdec_stmt {
private:
	typedef ART::parser::incdec_stmt	base_assign;
public:
	incdec_stmt(base_assign* a);
virtual ~incdec_stmt();

// remember to type check in CHP language mode

virtual ostream& what(ostream& o) const;
virtual line_position leftmost(void) const;
virtual line_position rightmost(void) const;
using	incdec_stmt::where;
};

//=============================================================================
/// CHP communication action base class
class communication : public statement {
protected:
	expr*		chan;
	token_char*	dir;
public:
	communication(expr* c, token_char* d);
virtual	~communication();

virtual	line_position leftmost(void) const;
};

//-----------------------------------------------------------------------------
class comm_list : public statement, public node_list<communication,comma> {
private:
	typedef	node_list<communication,comma>		comm_list_base;
public:
	comm_list(communication* c);
virtual	~comm_list();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
using	comm_list_base::where;
};

#define chp_comm_list_wrap(b,l,e)					\
	IS_A(CHP::comm_list*, l->wrap(b,e))
#define chp_comm_list_append(l,d,n)					\
	IS_A(CHP::comm_list*, l->append(d,n))


//-----------------------------------------------------------------------------
/// CHP send action
class send : public communication {
protected:
	expr_list*	rvalues;
public:
	send(expr* c, token_char* d, expr_list* r);
virtual	~send();

virtual	line_position rightmost(void) const;
};

//-----------------------------------------------------------------------------
/// CHP receive action
class receive : public communication {
protected:
	expr_list*	lvalues;
public:
	receive(expr* c, token_char* d, expr_list* l);
virtual	~receive();

virtual	line_position rightmost(void) const;
};

//=============================================================================
/// CHP selection statement abstract base class
class selection : public statement {
// is this class even necessary?
public:
	selection();
virtual	~selection();

virtual	ostream& what(ostream& o) const;
};

//=============================================================================
/// container for deterministic selection statement
class det_selection : public selection, 
		public node_list<guarded_command,thickbar> {
private:
	typedef	node_list<guarded_command,thickbar>	det_sel_base;
public:
	det_selection(guarded_command* n);
virtual	~det_selection();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
using	det_sel_base::where;
};

#define chp_det_selection_wrap(b,l,e)					\
	IS_A(CHP::det_selection*, l->wrap(b,e))
#define chp_det_selection_append(l,d,n)					\
	IS_A(CHP::det_selection*, l->append(d,n))

//=============================================================================
/// container for non-deterministic selection statement
class nondet_selection : public selection, 
		public node_list<guarded_command,colon> {
private:
	typedef	node_list<guarded_command,colon>	nondet_sel_base;
public:
	nondet_selection(guarded_command* n);
virtual	~nondet_selection();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
using	nondet_sel_base::where;
};

#define chp_nondet_selection_wrap(b,l,e)				\
	IS_A(CHP::nondet_selection*, l->wrap(b,e))
#define chp_nondet_selection_append(l,d,n)				\
	IS_A(CHP::nondet_selection*, l->append(d,n))

//=============================================================================
/// container for probablistic selection statement
class prob_selection : public selection, 
		public node_list<guarded_command,thickbar> {
private:
	typedef	node_list<guarded_command,thickbar>	prob_sel_base;
public:
	prob_selection(guarded_command* n);
virtual	~prob_selection();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
using	prob_sel_base::where;
};

#define chp_prob_selection_wrap(b,l,e)					\
	IS_A(CHP::prob_selection*, l->wrap(b,e))
#define chp_prob_selection_append(l,d,n)				\
	IS_A(CHP::prob_selection*, l->append(d,n))

//=============================================================================
/// CHP loop contains a list of statements
class loop : public statement {
protected:
	stmt_list*			commands;
public:
	loop(stmt_list* n);
virtual	~loop();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};

//=============================================================================
/// CHP do-until: re-enter selection statement until all guards are false
class do_until : public statement {
protected:
	det_selection*		sel;
public:
	do_until(det_selection* n);
virtual	~do_until();

// type-check: cannot contain an else clause, else infinite loop!

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};

//=============================================================================
/// CHP log statement
class log : public statement {
protected:
	token_keyword*		lc;
	expr_list*		args;
public:
	log(token_keyword* l, expr_list* n);
virtual	~log();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};

//=============================================================================
}	// end namespace CHP
}	// end namespace parser
}	// end namespace ART

#endif	//	__ART_PARSER_CHP_H__
