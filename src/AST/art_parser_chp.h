/**
	\file "art_parser_chp.h"
	CHP-specific syntax tree classes.  
	$Id: art_parser_chp.h,v 1.5 2005/01/13 22:47:54 fang Exp $
 */

#ifndef	__ART_PARSER_CHP_H__
#define	__ART_PARSER_CHP_H__

#include "art_parser_base.h"

namespace ART {
namespace parser {
/**
	This is the namespace for the CHP sub-language.  
 */
namespace CHP {

//=============================================================================
/// for now, just a carbon copy of expr class type, type-check later
typedef	expr	chp_expr;

//=============================================================================
/// CHP statement base class
class statement : virtual public node {
public:
	statement();

virtual	~statement();

virtual	ostream&
	what(ostream& o) const = 0;

virtual	line_position
	leftmost(void) const = 0;

virtual	line_position
	rightmost(void) const = 0;
};

typedef	node_list<const statement,semicolon>	stmt_list;

#define chp_stmt_list_wrap(b,l,e)					\
	IS_A(CHP::stmt_list*, l->wrap(b,e))
#define chp_stmt_list_append(l,d,n)					\
	IS_A(CHP::stmt_list*, l->append(d,n))

//=============================================================================
/// CHP body is just a list of statements
class body : public language_body {
protected:
	const excl_ptr<const stmt_list>	stmts;	///< list of CHP statements
public:
	body(const token_keyword* t, const stmt_list* s);

	~body();

	ostream&
	what(ostream& o) const;

using	language_body::leftmost;

	line_position
	rightmost(void) const;
};

//=============================================================================
/// CHP guarded command contains an expression condition and body
class guarded_command : public node {
protected:
	const excl_ptr<const chp_expr>	guard;		///< guard expression
	const excl_ptr<const terminal>	arrow;		///< right-arrow
	const excl_ptr<const stmt_list>	command;	///< statement body
public:
	guarded_command(const chp_expr* g, const terminal* a,
		const stmt_list* c);

virtual	~guarded_command();

virtual	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;
};

//=============================================================================
/// CHP else-clause is just a special case of a guarded_command
class else_clause : public guarded_command {
public:
	else_clause(const token_else* g, const terminal* a, const stmt_list* c);

	~else_clause();

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
	explicit
	skip(const token_keyword* s);

	~skip();

// check that nothing appears after skip statement

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

using	token_keyword::where;
};

//=============================================================================
/// CHP wait contains just an expression
class wait : public statement {
protected:
	const excl_ptr<const terminal>	lb;	///< left bracket
	const excl_ptr<const expr>	cond;	///< wait until condition
	const excl_ptr<const terminal>	rb;	///< right bracket
public:
	wait(const terminal* l, const expr* c, const terminal* r);

	~wait();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;
};

//=============================================================================
/// CHP assignment statement in binary form
class assignment : public statement, public assign_stmt {
private:
	typedef	CHP::statement			parent_type;
	typedef ART::parser::assign_stmt	base_assign;
public:
	explicit
	assignment(base_assign* a);

	~assignment();

// remember to type check in CHP language mode

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

using	assign_stmt::where;
};

//-----------------------------------------------------------------------------
/// CHP assignment statement is only boolean
class incdec_stmt : public statement, public parser::incdec_stmt {
private:
	typedef	CHP::statement			parent_type;
	typedef ART::parser::incdec_stmt	base_assign;
public:
	explicit
	incdec_stmt(base_assign* a);

	~incdec_stmt();

// remember to type check in CHP language mode

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

using	incdec_stmt::where;
};

//=============================================================================
/// CHP communication action base class
class communication : public statement {
protected:
	const excl_ptr<const expr>		chan;
	const excl_ptr<const token_char>	dir;
public:
	communication(const expr* c, const token_char* d);

virtual	~communication();

	line_position
	leftmost(void) const;
};

//-----------------------------------------------------------------------------
class comm_list : public statement,
		public node_list<const communication,comma> {
private:
	typedef	node_list<const communication,comma>		comm_list_base;
public:
	explicit
	comm_list(const communication* c);

	~comm_list();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

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
	const excl_ptr<const expr_list>	rvalues;
public:
	send(const expr* c, const token_char* d, const expr_list* r);

	~send();

	ostream&
	what(ostream& o) const;

	line_position
	rightmost(void) const;
};

//-----------------------------------------------------------------------------
/// CHP receive action
class receive : public communication {
protected:
	const excl_ptr<const expr_list>	lvalues;
public:
	receive(const expr* c, const token_char* d, const expr_list* l);

	~receive();

	ostream&
	what(ostream& o) const;

	line_position
	rightmost(void) const;
};

//=============================================================================
/// CHP selection statement abstract base class
class selection : public statement {
// is this class even necessary?
public:
	selection();

virtual	~selection();

virtual	ostream&
	what(ostream& o) const = 0;
};

//=============================================================================
/// container for deterministic selection statement
class det_selection : public selection, 
		public node_list<const guarded_command,thickbar> {
private:
	typedef	node_list<const guarded_command,thickbar>	det_sel_base;
public:
	explicit
	det_selection(const guarded_command* n);

	~det_selection();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

using	det_sel_base::where;
};

#define chp_det_selection_wrap(b,l,e)					\
	IS_A(CHP::det_selection*, l->wrap(b,e))
#define chp_det_selection_append(l,d,n)					\
	IS_A(CHP::det_selection*, l->append(d,n))

//=============================================================================
/// container for non-deterministic selection statement
class nondet_selection : public selection, 
		public node_list<const guarded_command,colon> {
private:
	typedef	node_list<const guarded_command,colon>	nondet_sel_base;
public:
	explicit
	nondet_selection(const guarded_command* n);

	~nondet_selection();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

using	nondet_sel_base::where;
};

#define chp_nondet_selection_wrap(b,l,e)				\
	IS_A(CHP::nondet_selection*, l->wrap(b,e))
#define chp_nondet_selection_append(l,d,n)				\
	IS_A(CHP::nondet_selection*, l->append(d,n))

//=============================================================================
/// container for probablistic selection statement
class prob_selection : public selection, 
		public node_list<const guarded_command,thickbar> {
private:
	typedef	node_list<const guarded_command,thickbar>	prob_sel_base;
public:
	explicit
	prob_selection(const guarded_command* n);

	~prob_selection();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

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
	const excl_ptr<const stmt_list>		commands;
public:
	explicit
	loop(const stmt_list* n);

	~loop();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;
};

//=============================================================================
/// CHP do-until: re-enter selection statement until all guards are false
class do_until : public statement {
protected:
	const excl_ptr<const det_selection>		sel;
public:
	explicit
	do_until(const det_selection* n);

	~do_until();

// type-check: cannot contain an else clause, else infinite loop!

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;
};

//=============================================================================
/// CHP log statement
class log : public statement {
protected:
	const excl_ptr<const token_keyword>		lc;
	const excl_ptr<const expr_list>			args;
public:
	log(const token_keyword* l, const expr_list* n);

	~log();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;
};

//=============================================================================
}	// end namespace CHP
}	// end namespace parser
}	// end namespace ART

#endif	//	__ART_PARSER_CHP_H__
