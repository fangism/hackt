/**
	\file "art_parser_hse.h"
	HSE-specific syntax tree classes.
	$Id: art_parser_hse.h,v 1.6.40.1 2005/02/22 08:15:20 fang Exp $
 */

#ifndef	__ART_PARSER_HSE_H__
#define	__ART_PARSER_HSE_H__

#include "art_parser_definition_item.h"
#include "art_parser_terminal.h"
#include "art_parser_statement.h"

namespace ART {
namespace parser {
//=============================================================================
/**
	This is the namespace for the HSE sub-language.  
 */
namespace HSE {
//=============================================================================
/// for now, just a carbon copy of expr class type, type-check later
typedef	expr	hse_expr;

//=============================================================================
/// HSE statement base class
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
};	// end class statement

typedef	node_list<const statement,semicolon>	stmt_list;

#define hse_stmt_list_wrap(b,l,e)					\
	IS_A(HSE::stmt_list*, l->wrap(b,e))
#define hse_stmt_list_append(l,d,n)					\
	IS_A(HSE::stmt_list*, l->append(d,n))

//=============================================================================
/// HSE body is just a list of statements
class body : public language_body {
protected:
	const excl_ptr<const stmt_list>	stmts;	///< list of HSE statements
public:
	body(const token_keyword* t, const stmt_list* s);
	~body();

	ostream&
	what(ostream& o) const;

using	language_body::leftmost;

	line_position
	rightmost(void) const;
};	// end class body

//=============================================================================
/// HSE guarded command contains an expression condition and body
class guarded_command : public node {
protected:
	const excl_ptr<const hse_expr>	guard;		///< guard expression
	const excl_ptr<const terminal>	arrow;		///< right-arrow
	const excl_ptr<const stmt_list>	command;	///< statement body
public:
	guarded_command(const hse_expr* g, const terminal* a,
		const stmt_list* c);

virtual	~guarded_command();

virtual	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;
};	// end class guarded_command

//=============================================================================
/// HSE else-clause is just a special case of a guarded_command
class else_clause : public guarded_command {
public:
	else_clause(const token_else* g, const terminal* a, const stmt_list* c);

	~else_clause();

	ostream&
	what(ostream& o) const;
};	// end class else_clause

//=============================================================================
/// HSE skip statement
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
};	// end class skip

//=============================================================================
/// HSE wait contains just an expression
class wait : public statement {
protected:
	const excl_ptr<const terminal>	lb;
	const excl_ptr<const expr>	cond;
	const excl_ptr<const terminal>	rb;
public:
	wait(const terminal* l, const expr* c, const terminal* r);

	~wait();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;
};	// end class wait

//=============================================================================
/// HSE assignment statement is only boolean
class assignment : public statement, public incdec_stmt {
private:
	typedef ART::parser::incdec_stmt	base_assign;
public:
	explicit
	assignment(base_assign* a);

	~assignment();

// remember to type check in HSE language mode

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

using	incdec_stmt::where;
};	// end class assignment

//=============================================================================
/// HSE selection statement abstract base class
class selection : public statement {
// is this class even necessary?
public:
	selection();
virtual	~selection();

virtual	ostream&
	what(ostream& o) const = 0;
};	// end class selection

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
};	// end class det_selection

#define hse_det_selection_wrap(b,l,e)					\
	IS_A(HSE::det_selection*, l->wrap(b,e))
#define hse_det_selection_append(l,d,n)					\
	IS_A(HSE::det_selection*, l->append(d,n))

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
};	// end class nondet_selection

#define hse_nondet_selection_wrap(b,l,e)				\
	IS_A(HSE::nondet_selection*, l->wrap(b,e))
#define hse_nondet_selection_append(l,d,n)				\
	IS_A(HSE::nondet_selection*, l->append(d,n))

//=============================================================================
/*** not available ***
// container for probablistic selection statement
class prob_selection : public selection, 
		public node_list<guarded_command,thickbar> {
private:
	typedef	node_list<guarded_command,thickbar>	prob_sel_base;
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
};	// end class prob_selection
*** not available ***/

//=============================================================================
/// HSE loop contains a list of statements
class loop : public statement {
protected:
	const excl_ptr<const stmt_list>			commands;
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
};	// end class loop

//=============================================================================
/// HSE do-until: re-enter selection statement until all guards are false
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
};	// end class do_until

//=============================================================================
}	// end namespace HSE
}	// end namespace parser
}	// end namespace ART

#endif	// __ART_PARSER_HSE_H__

