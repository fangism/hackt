/**
	\file "AST/art_parser_chp.h"
	CHP-specific syntax tree classes.  
	$Id: art_parser_chp.h,v 1.11.2.3 2005/06/11 03:34:00 fang Exp $
 */

#ifndef	__AST_ART_PARSER_CHP_H__
#define	__AST_ART_PARSER_CHP_H__

#include "AST/art_parser_chp_fwd.h"
#include "AST/art_parser_token_string.h"
#include "AST/art_parser_statement.h"
#include "AST/art_parser_definition_item.h"
#include "util/memory/count_ptr.h"
#include "util/STL/vector_fwd.h"

namespace ART {
namespace entity {
	struct channel_tag;
	template <class> class simple_nonmeta_instance_reference;
	typedef	simple_nonmeta_instance_reference<channel_tag>
		simple_channel_nonmeta_instance_reference;
namespace CHP {
	class action;
	class guarded_action;
}
}	// end namespace entity

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
class statement {
public:
	typedef	count_ptr<entity::CHP::action>		return_type;
public:
	statement();

virtual	~statement();

virtual	ostream&
	what(ostream& o) const = 0;

virtual	line_position
	leftmost(void) const = 0;

virtual	line_position
	rightmost(void) const = 0;

#define	CHP_CHECK_STMT_PROTO						\
	CHP::statement::return_type					\
	check_action(context&) const

virtual	CHP_CHECK_STMT_PROTO = 0;
};	// end class statement

//=============================================================================
#if 0
/**
	Statement list.  
 */
class stmt_list : public stmt_list_base {
public:
	stmt_list();
	~stmt_list();

	check_actions(...)
};	// end class stmt_list
#endif

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

	never_ptr<const object>
	check_build(context&) const;
};	// end class body

//=============================================================================
/// CHP guarded command contains an expression condition and body
class guarded_command {
public:
	typedef	count_ptr<entity::CHP::guarded_action>	return_type;
protected:
	const excl_ptr<const chp_expr>	guard;		///< guard expression
	const excl_ptr<const string_punctuation_type>	arrow;		///< right-arrow
	const excl_ptr<const stmt_list>	command;	///< statement body
public:
	guarded_command(const chp_expr* g, const string_punctuation_type* a,
		const stmt_list* c);

virtual	~guarded_command();

virtual	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

#define	CHP_CHECK_GC_PROTO						\
	CHP::guarded_command::return_type				\
	check_guarded_action(context&) const

	CHP_CHECK_GC_PROTO;
};	// end class guarded_command

//=============================================================================
/// CHP else-clause is just a special case of a guarded_command
class else_clause : public guarded_command {
public:
	else_clause(const token_else* g, 
		const string_punctuation_type* a, const stmt_list* c);

	~else_clause();

	ostream& what(ostream& o) const;
};	// end class else_clause

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

	CHP_CHECK_STMT_PROTO;
};	// end class skip

//=============================================================================
/// CHP wait contains just an expression
class wait : public statement {
protected:
	const excl_ptr<const expr>	cond;	///< wait until condition
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

	CHP_CHECK_STMT_PROTO;
};	// end class wait

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

	CHP_CHECK_STMT_PROTO;
};	// end class assignment

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

#if 1
	never_ptr<const object>
	check_build(context& ) const;
#endif
	CHP_CHECK_STMT_PROTO;
};	// end class incdec_stmt

//=============================================================================
/// CHP communication action base class
class communication : public statement {
	typedef	statement			parent_type;
protected:
	typedef	count_ptr<entity::simple_channel_nonmeta_instance_reference>
						checked_channel_type;
protected:
	const excl_ptr<const inst_ref_expr>		chan;
	const excl_ptr<const char_punctuation_type>	dir;
#if 0
public:
	static const char			separator[];	// comma
#endif
public:
	communication(const inst_ref_expr* c, const char_punctuation_type* d);

virtual	~communication();

	line_position
	leftmost(void) const;

protected:
	checked_channel_type
	check_channel(context& c) const;

	static
	char
	get_channel_direction(const checked_channel_type::element_type&);

};	// end class communication

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

	CHP_CHECK_STMT_PROTO;
private:
	typedef	DEFAULT_VECTOR(communication::return_type)
						checked_actions_type;
};	// end class comm_list


//-----------------------------------------------------------------------------
/// CHP send action
class send : public communication {
protected:
	const excl_ptr<const expr_list>	rvalues;
public:
	send(const inst_ref_expr* c, const char_punctuation_type* d, 
		const expr_list* r);

	~send();

	ostream&
	what(ostream& o) const;

	line_position
	rightmost(void) const;

	CHP_CHECK_STMT_PROTO;
};	// end class send

//-----------------------------------------------------------------------------
/// CHP receive action
class receive : public communication {
protected:
	const excl_ptr<const inst_ref_expr_list>	lvalues;
public:
	receive(const inst_ref_expr* c, 
		const char_punctuation_type* d, const inst_ref_expr_list* l);

	~receive();

	ostream&
	what(ostream& o) const;

	line_position
	rightmost(void) const;

	CHP_CHECK_STMT_PROTO;
};	// end class receive

//=============================================================================
/// CHP selection statement abstract base class
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

	CHP_CHECK_STMT_PROTO;
};	// end class det_selection

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

	CHP_CHECK_STMT_PROTO;
};	// end class nondet_selection

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

	CHP_CHECK_STMT_PROTO;
};	// end class prob_selection

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

	CHP_CHECK_STMT_PROTO;
};	// end class loop

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

	CHP_CHECK_STMT_PROTO;
};	// end class do_until

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

	CHP_CHECK_STMT_PROTO;
};	// end class log

//=============================================================================
}	// end namespace CHP
}	// end namespace parser
}	// end namespace ART

#endif	//	__AST_ART_PARSER_CHP_H__
