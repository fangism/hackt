/**
	\file "AST/HSE.h"
	HSE-specific syntax tree classes.
	$Id: HSE.h,v 1.3 2007/11/26 08:27:21 fang Exp $
	This file used to be the following before renamed:
	Id: art_parser_hse.h,v 1.12.48.1 2005/12/11 00:45:07 fang Exp
 */

#ifndef	__HAC_AST_HSE_H__
#define	__HAC_AST_HSE_H__

#include "AST/HSE_fwd.h"
#include "AST/lang.h"
#include "AST/token_string.h"
#include "AST/statement.h"

namespace HAC {
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
class statement {
public:
	statement();
virtual	~statement();

virtual	ostream&
	what(ostream& o) const = 0;

virtual	line_position
	leftmost(void) const = 0;

virtual	line_position
	rightmost(void) const = 0;

#if 1
virtual	never_ptr<const object>
	check_build(context& c) const = 0;
#endif
};	// end class statement

typedef	node_list<const statement>		stmt_list;

//=============================================================================
/// HSE body is just a list of statements
class body : public language_body {
protected:
	const excl_ptr<const stmt_list>	stmts;	///< list of HSE statements
public:
	body(const generic_keyword_type* t, const stmt_list* s);
	~body();

	ostream&
	what(ostream& o) const;

using	language_body::leftmost;

	line_position
	rightmost(void) const;

#if 1
	never_ptr<const object>
	check_build(context& ) const;
#endif
};	// end class body

//=============================================================================
/// HSE guarded command contains an expression condition and body
class guarded_command {
protected:
	const excl_ptr<const hse_expr>	guard;		///< guard expression
	const excl_ptr<const string_punctuation_type>	arrow;	///< right-arrow
	const excl_ptr<const stmt_list>	command;	///< statement body
public:
	guarded_command(const hse_expr* g, const string_punctuation_type* a,
		const stmt_list* c);

virtual	~guarded_command();

virtual	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

#if 1
	never_ptr<const object>
	check_build(context& ) const;
#endif
};	// end class guarded_command

//=============================================================================
/// HSE else-clause is just a special case of a guarded_command
class else_clause : public guarded_command {
public:
	else_clause(const token_else* g, const string_punctuation_type* a, 
		const stmt_list* c);

	~else_clause();

	ostream&
	what(ostream& o) const;

#if 1
	never_ptr<const object>
	check_build(context& ) const;
#endif
};	// end class else_clause

//=============================================================================
/// HSE skip statement
class skip : public statement {
private:
	const excl_ptr<const generic_keyword_type>	kw;
public:
/**
	Constructor takes a plain keyword token and re-wraps the string
	containing "skip", which effectively casts this as a sub-class.  
 */
	explicit
	skip(const generic_keyword_type* s);

	~skip();

// check that nothing appears after skip statement

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

// using	token_keyword::where;

#if 1
	never_ptr<const object>
	check_build(context& ) const;
#endif
};	// end class skip

//=============================================================================
/// HSE wait contains just an expression
class wait : public statement {
protected:
//	const excl_ptr<const char_punctuation_type>	lb;
	const excl_ptr<const expr>			cond;
//	const excl_ptr<const char_punctuation_type>	rb;
public:
	explicit
	wait(const expr* c);

	~wait();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

#if 1
	never_ptr<const object>
	check_build(context& ) const;
#endif
};	// end class wait

//=============================================================================
/// HSE assignment statement is only boolean
class assignment : public statement, public incdec_stmt {
private:
	typedef HAC::parser::incdec_stmt	base_assign;
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

#if 1
	never_ptr<const object>
	check_build(context& ) const;
#endif
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
typedef	node_list<const guarded_command>		det_selection_list_base;

/// container for deterministic selection statement
class det_selection : public selection, public det_selection_list_base {
private:
	typedef	det_selection_list_base			parent_type;
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

#if 1
	never_ptr<const object>
	check_build(context& ) const;
#endif
};	// end class det_selection

//=============================================================================
typedef node_list<const guarded_command>	nondet_selection_list_base;

/// container for non-deterministic selection statement
class nondet_selection : public selection, public nondet_selection_list_base {
private:
	typedef	nondet_selection_list_base		parent_type;
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

#if 1
	never_ptr<const object>
	check_build(context& ) const;
#endif
};	// end class nondet_selection

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

#if 1
	never_ptr<const object>
	check_build(context& ) const;
#endif
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

#if 1
	never_ptr<const object>
	check_build(context& ) const;
#endif
};	// end class do_until

//=============================================================================
}	// end namespace HSE
}	// end namespace parser
}	// end namespace HAC

#endif	// __HAC_AST_HSE_H__

