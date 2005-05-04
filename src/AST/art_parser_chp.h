/**
	\file "art_parser_chp.h"
	CHP-specific syntax tree classes.  
	$Id: art_parser_chp.h,v 1.8.4.2 2005/05/04 05:06:29 fang Exp $
 */

#ifndef	__ART_PARSER_CHP_H__
#define	__ART_PARSER_CHP_H__

#include "art_parser_chp_fwd.h"
#include "art_parser_token_string.h"
#include "art_parser_statement.h"
#include "art_parser_definition_item.h"

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
class statement
#if USE_MOTHER_NODE
	: virtual public node
#endif
{
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
	check_build(context& ) const = 0;
#endif
};

//=============================================================================
/// CHP body is just a list of statements
class body : public language_body {
protected:
	const excl_ptr<const stmt_list>	stmts;	///< list of CHP statements
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
};

//=============================================================================
/// CHP guarded command contains an expression condition and body
class guarded_command
#if USE_MOTHER_NODE
	: public node
#endif
{
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

#if 1
	never_ptr<const object>
	check_build(context& ) const;
#endif
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
class skip : public statement {
// , public token_keyword {
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
};

//=============================================================================
/// CHP wait contains just an expression
class wait : public statement {
protected:
//	const excl_ptr<const char_punctuation_type>	lb;	///< left bracket
	const excl_ptr<const expr>			cond;	///< wait until condition
//	const excl_ptr<const char_punctuation_type>	rb;	///< right bracket
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

#if USE_MOTHER_NODE
using	assign_stmt::where;
#endif

#if 1
	never_ptr<const object>
	check_build(context& ) const;
#endif
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

#if USE_MOTHER_NODE
using	incdec_stmt::where;
#endif

#if 1
	never_ptr<const object>
	check_build(context& ) const;
#endif
};

//=============================================================================
/// CHP communication action base class
class communication : public statement {
protected:
	const excl_ptr<const expr>		chan;
	const excl_ptr<const token_char>	dir;
#if 0
public:
	static const char			separator[];	// comma
#endif
public:
	communication(const expr* c, const token_char* d);

virtual	~communication();

	line_position
	leftmost(void) const;
};

//-----------------------------------------------------------------------------
/// base type for CHP communication list
typedef	node_list<const communication>			comm_list_base;

/**
	CHP concurrent communication list.  
 */
class comm_list : public statement, public comm_list_base {
private:
	typedef	comm_list_base				parent_type;
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

#if USE_MOTHER_NODE
using	comm_list_base::where;
#endif

#if 1
	never_ptr<const object>
	check_build(context& ) const;
#endif
};


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

#if 1
	never_ptr<const object>
	check_build(context& ) const;
#endif
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

#if 1
	never_ptr<const object>
	check_build(context& ) const;
#endif
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

#if USE_MOTHER_NODE
using	parent_type::where;
#endif

#if 1
	never_ptr<const object>
	check_build(context& ) const;
#endif
};

//=============================================================================
typedef	node_list<const guarded_command>	nondet_selection_list_base;

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

#if USE_MOTHER_NODE
using	parent_type::where;
#endif

#if 1
	never_ptr<const object>
	check_build(context& ) const;
#endif
};

//=============================================================================
typedef	node_list<const guarded_command>	prob_selection_list_base;

/// container for probablistic selection statement
class prob_selection : public selection, public prob_selection_list_base {
private:
	typedef	prob_selection_list_base		parent_type;
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

#if USE_MOTHER_NODE
using	parent_type::where;
#endif

#if 1
	never_ptr<const object>
	check_build(context& ) const;
#endif
};

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

#if 1
	never_ptr<const object>
	check_build(context& ) const;
#endif
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

#if 1
	never_ptr<const object>
	check_build(context& ) const;
#endif
};

//=============================================================================
/// CHP log statement
class log : public statement {
protected:
	const excl_ptr<const generic_keyword_type>	lc;
	const excl_ptr<const expr_list>			args;
public:
	log(const generic_keyword_type* l, const expr_list* n);

	~log();

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
};

//=============================================================================
}	// end namespace CHP
}	// end namespace parser
}	// end namespace ART

#endif	//	__ART_PARSER_CHP_H__
