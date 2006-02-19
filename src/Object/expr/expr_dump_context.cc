/**
	\file "Object/expr/exp_dump_context.cc"
	$Id: expr_dump_context.cc,v 1.4.10.1 2006/02/19 03:52:51 fang Exp $
 */

#include "Object/expr/expr_dump_context.h"
#include "Object/expr/operator_precedence.h"
#include "Object/lang/PRS_base.h"	// for PRS::expr_dump_context

namespace HAC {
namespace entity {
//=============================================================================

const expr_dump_context
expr_dump_context::default_value;

const expr_dump_context
expr_dump_context::brief(OP_PREC_DEFAULT, NULL, false);

const expr_dump_context
expr_dump_context::error_mode(OP_PREC_DEFAULT, NULL, true);

//-----------------------------------------------------------------------------
expr_dump_context::expr_dump_context() : caller_stamp(OP_PREC_DEFAULT),
		enclosing_scope(), include_type_info(false),
		parent_associativity(false) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
expr_dump_context::expr_dump_context(const scopespace* s) :
		caller_stamp(OP_PREC_DEFAULT),
		enclosing_scope(s), include_type_info(false),
		parent_associativity(false) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
expr_dump_context::expr_dump_context(const char cs,
		const scopespace* s, const bool t) :
		caller_stamp(cs), enclosing_scope(s), 
		include_type_info(t), parent_associativity(false) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
expr_dump_context::expr_dump_context(const PRS::expr_dump_context& c) :
		caller_stamp(OP_PREC_DEFAULT), enclosing_scope(c.parent_scope), 
		include_type_info(false), parent_associativity(false) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
expr_dump_context::~expr_dump_context() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param c the precedence of the child operator.
	\param a true if the parent operator is associative.  
 */
bool
expr_dump_context::need_parentheses(const char c, const bool a) const {
	if (c < caller_stamp)
		return true;
	else	return (c == caller_stamp && !a);
}

//=============================================================================
// class stamp_modifier method definitions

/**
	Constructor saves away relevant information to restore later.  
 */
expr_dump_context::stamp_modifier::stamp_modifier(
		const expr_dump_context& c, const char s, const bool a) :
		_context(const_cast<expr_dump_context&>(c)),
		_old_caller_stamp(c.caller_stamp), 
		_old_assoc(c.parent_associativity) {
	_context.caller_stamp = s;
	_context.parent_associativity = a;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Destructor restores the original values.  
 */
expr_dump_context::stamp_modifier::~stamp_modifier() {
	_context.caller_stamp = _old_caller_stamp;
	_context.parent_associativity = _old_assoc;
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

