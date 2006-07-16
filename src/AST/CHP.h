/**
	\file "AST/CHP.h"
	CHP-specific syntax tree classes.  
	$Id: CHP.h,v 1.5 2006/07/16 03:34:42 fang Exp $
	Used to be the following before rename:
	Id: art_parser_chp.h,v 1.13.40.1 2005/12/11 00:45:03 fang Exp
 */

#ifndef	__HAC_AST_CHP_H__
#define	__HAC_AST_CHP_H__

#include "AST/CHP_fwd.h"
#include "AST/token_string.h"
#include "AST/statement.h"
#include "AST/definition_item.h"
#include "util/memory/count_ptr.h"
#include "util/STL/vector_fwd.h"
#include "util/boolean_types.h"

namespace HAC {
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
using util::good_bool;
using std::default_vector;

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
/**
	Sequential or concurrent statement list.  
 */
class stmt_list : public statement, public stmt_list_base {
public:
	typedef	list<statement::return_type>	checked_stmts_type;
protected:
	bool				is_concurrent;
public:
	stmt_list();

	explicit
	stmt_list(const statement*);

	~stmt_list();

	void
	set_concurrent(const bool b) { is_concurrent = b; }

	ostream&
	what(ostream&) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	void
	postorder_check_stmts(checked_stmts_type&, context&) const;

	CHP_CHECK_STMT_PROTO;

};	// end class stmt_list

//=============================================================================
/// CHP body is just a list of statements
class body : public language_body {
	typedef	stmt_list::checked_stmts_type		checked_stmts_type;
	typedef	checked_stmts_type::const_iterator	const_checked_iterator;
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

	// common check routine
	good_bool
	check_CHP(checked_stmts_type&, context&) const;

	// called by process-definitions
	never_ptr<const object>
	check_build(context&) const;

	// called by channel-definitions
	good_bool
	check_channel_CHP(context&, const bool is_send) const;

	// called by datatype-definitions
	good_bool
	check_datatype_CHP(context&, const bool is_set) const;

};	// end class body

//=============================================================================
/// CHP guarded command contains an expression condition and body
class guarded_command {
public:
	typedef	count_ptr<entity::CHP::guarded_action>	return_type;
protected:
	const excl_ptr<const chp_expr>	guard;		///< guard expression
	const excl_ptr<const string_punctuation_type>	arrow;		///< right-arrow
	const excl_ptr<const statement>	command;	///< statement body
public:
	guarded_command(const chp_expr* g, const string_punctuation_type* a,
		const statement* c);

	~guarded_command();

	ostream&
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
// CHP else-clause is just a special case of a guarded_command

//=============================================================================
/// CHP skip statement
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
class binary_assignment : public statement {
private:
	typedef	CHP::statement			parent_type;
private:
	const excl_ptr<const inst_ref_expr>	lval;
	const excl_ptr<const expr>		rval;
public:
	explicit
	binary_assignment(const inst_ref_expr*, const expr*);

	~binary_assignment();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	CHP_CHECK_STMT_PROTO;
};	// end class binary_assignment

//-----------------------------------------------------------------------------
/// CHP assignment statement is only boolean
class bool_assignment : public statement {
private:
	typedef	CHP::statement			parent_type;
	const excl_ptr<const inst_ref_expr>		bool_var;
	const excl_ptr<const char_punctuation_type>	dir;
public:
	bool_assignment(const inst_ref_expr*,
		const char_punctuation_type*);

	~bool_assignment();

// remember to type check in CHP language mode

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	CHP_CHECK_STMT_PROTO;
};	// end class bool_assignment

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
typedef	node_list<const guarded_command>		selection_list_base;

/// CHP selection statement abstract base class
class selection : public statement, public selection_list_base {
	typedef	selection_list_base			list_type;
// is this class even necessary?
public:
	typedef	default_vector<guarded_command::return_type>::type
							checked_gcs_type;
public:
	selection();

	explicit
	selection(const guarded_command*);

virtual	~selection();

virtual	ostream&
	what(ostream& o) const = 0;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	good_bool
	postorder_check_gcs(checked_gcs_type&, context&) const;

};	// end class selection

//=============================================================================
/// container for deterministic selection statement
class det_selection : public selection {
private:
	typedef	selection			parent_type;
public:
	explicit
	det_selection(const guarded_command* n);

	~det_selection();

	ostream&
	what(ostream& o) const;

	CHP_CHECK_STMT_PROTO;
};	// end class det_selection

//=============================================================================
/// container for non-deterministic selection statement
class nondet_selection : public selection {
private:
	typedef	selection		parent_type;
public:
	explicit
	nondet_selection(const guarded_command* n);

	~nondet_selection();

	ostream&
	what(ostream& o) const;

	CHP_CHECK_STMT_PROTO;
};	// end class nondet_selection

//=============================================================================
/// container for probablistic selection statement
class prob_selection : public selection {
private:
	typedef	selection		parent_type;
public:
	explicit
	prob_selection(const guarded_command* n);

	~prob_selection();

	ostream&
	what(ostream& o) const;

	CHP_CHECK_STMT_PROTO;
};	// end class prob_selection

//=============================================================================
/**
	Compile-time expanded loop for selection statement.  
	This converts a run-time selection into an explicit
	compile-time selection.  
	e.g.: [[]i:N: i==j -> X!(x[j]) ]
 */
class metaloop_selection : public statement {
private:
	typedef	statement				parent_type;
private:
	const excl_ptr<const char_punctuation_type>	lb;
	const excl_ptr<const char_punctuation_type>	selection_type;
	const excl_ptr<const token_identifier>		index;
	const excl_ptr<const range>			bounds;
	const excl_ptr<const guarded_command>		body;
	const excl_ptr<const char_punctuation_type>	rb;
public:
	metaloop_selection(const char_punctuation_type*, 
		const char_punctuation_type*,
		const token_identifier*, const range*, 
		const guarded_command*, const char_punctuation_type*);
	~metaloop_selection();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	CHP_CHECK_STMT_PROTO;
};	// end class metaloop_selection

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
private:
	typedef	default_vector<statement::return_type>::type
						checked_actions_type;
};	// end class loop

//=============================================================================
/**
	CHP do-until: re-enter selection statement until all guards are false
	TODO: rename this to do_while, because loop is repeated
		while at least one guard is true.  
 */
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
}	// end namespace HAC

#endif	//	__HAC_AST_CHP_H__
