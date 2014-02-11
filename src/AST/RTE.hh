/**
	\file "AST/RTE.hh"
	RTE: atomic run-time expressions
	RTE-specific syntax tree classes.
 */

#ifndef	__HAC_AST_RTE_HH__
#define	__HAC_AST_RTE_HH__

#include "AST/common.hh"
#include "AST/RTE_fwd.hh"
#include "AST/expr_base.hh"
#include "AST/lang.hh"
#include "util/STL/pair_fwd.hh"
#include "util/memory/count_ptr.hh"

namespace HAC {
namespace parser {
/**
	This is the namespace for the RTE sub-language.  
 */
namespace RTE {
using util::memory::count_ptr;

//=============================================================================
// local forward declarations

class assignment;
class body;

//=============================================================================
/// a single assignment
class body_item {
public:
	typedef	void				return_type;
protected:
	// no members
public:
	body_item();

virtual	~body_item();

virtual	ostream&
	what(ostream& o) const = 0;

virtual	line_position
	leftmost(void) const = 0;

virtual	line_position
	rightmost(void) const = 0;

#define	RTE_ITEM_CHECK_PROTO						\
	body_item::return_type						\
	check_assignment(context&) const

virtual	RTE_ITEM_CHECK_PROTO = 0;
};	// end class body_item

//=============================================================================
/**
	Single assignment statement.
	See also CHP::binary_assignment.
 */
class assignment : public body_item {
protected:
	const excl_ptr<const inst_ref_expr>	lvalue;
	const excl_ptr<const expr>		rvalue;
public:
	assignment(const inst_ref_expr*, const expr*);

	~assignment();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	RTE_ITEM_CHECK_PROTO;

};	// end class assignment

//=============================================================================
#if 0
/**
	Repetition of assignments in a loop.  
 */
class loop : public body_item {
protected:
	const excl_ptr<const char_punctuation_type>	lp;
	const excl_ptr<const token_identifier>	index;
	const excl_ptr<const range>		bounds;
	const excl_ptr<const assignment_list>		assignments;
	const excl_ptr<const char_punctuation_type>	rp;
public:
	loop(const char_punctuation_type* l,
		const token_identifier* id, const range* b,
		const assignment_list* rl, const char_punctuation_type* r);

	~loop();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	RTE_ITEM_CHECK_PROTO;
};	// end class loop
#endif

//=============================================================================
#if 0
/**
	Guarded RTE body clause.  
 */
class guarded_body {
	const excl_ptr<const expr>			guard;
	const excl_ptr<const char_punctuation_type>	arrow;
	const excl_ptr<const assignment_list>		assignments;
public:
	typedef	void				return_type;
public:
	guarded_body(const expr*, const char_punctuation_type*,
		const assignment_list*);
	~guarded_body();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	return_type
	check_clause(context&) const;

};	// end class guarded_body
#endif

//=============================================================================
#if 0
/**
	Class for wrapping assignments inside conditionals.  
 */
class conditional : public body_item {
	const excl_ptr<const guarded_assignment_list>		gp;
public:
	// explicit
	conditional(const guarded_assignment_list*);
	~conditional();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	RTE_ITEM_CHECK_PROTO;
};	// end class conditional
#endif

//=============================================================================
/**
	Collection of run-time expression assignments.
	Now is also a body_item because of nested bodies.  
 */
class body : public language_body, public body_item {
protected:
	const excl_ptr<const assignment_list>		assignments;
public:
	body(const generic_keyword_type* t, 
		const assignment_list* r);
virtual	~body();

virtual	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	// needs the return-type of language-body
// virtual?
	ROOT_CHECK_PROTO;
	RTE_ITEM_CHECK_PROTO;

protected:
	bool
	__check_assignments(context&) const;

};	// end class body

//=============================================================================
#if 0
/**
	Shortcut loop of AND or OR operation.  
	Don't know if this will be useful outside of the RTE context.  
 */
class op_loop : public expr {
protected:
	const excl_ptr<const char_punctuation_type>	lp;
	const excl_ptr<const char_punctuation_type>	op;
	const excl_ptr<const token_identifier>	index;
	const excl_ptr<const range>		bounds;
	const excl_ptr<const expr>		ex;
	const excl_ptr<const char_punctuation_type>	rp;
public:
	op_loop(const char_punctuation_type* l,
		const char_punctuation_type* o,
		const token_identifier* id, 
		const range* b, 
		const expr* e, const char_punctuation_type* r);

	~op_loop();

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream&) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	CHECK_META_EXPR_PROTO;
	CHECK_NONMETA_EXPR_PROTO;
	CHECK_RTE_EXPR_PROTO;
};	// end class op_loop
#endif

//=============================================================================
}	// end namespace RTE
}	// end namespace parser
}	// end namespace HAC

#endif	//	__HAC_AST_RTE_HH__

