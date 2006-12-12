/**
	\file "Object/expr/pint_unary_expr.h"
	Unary negation of meta integer.  
	NOTE: this file was spawned from the old
		"Object/art_object_expr.h" for revision history tracking.  
	$Id: pint_unary_expr.h,v 1.14.12.1 2006/12/12 10:18:07 fang Exp $
 */

#ifndef __HAC_OBJECT_EXPR_PINT_UNARY_EXPR_H__
#define __HAC_OBJECT_EXPR_PINT_UNARY_EXPR_H__

#include "Object/expr/pint_expr.h"
#include "util/memory/count_ptr.h"

namespace HAC {
namespace entity {
using util::memory::count_ptr;
class pint_const;

//=============================================================================
/**
	Only possibilities, unary negation, bit-wise negation.  
 */
class pint_unary_expr : public pint_expr {
	typedef	pint_unary_expr			this_type;
	typedef	pint_expr			parent_type;
public:
	typedef	pint_value_type		value_type;
	typedef	char			op_type;
	typedef	count_ptr<const pint_expr>	operand_ptr_type;
protected:
	const op_type			op;
	/** expression argument must be 0-dimensional */
	operand_ptr_type		ex;
private:
	pint_unary_expr();
public:
	pint_unary_expr(const op_type o, const operand_ptr_type& e);
	pint_unary_expr(const operand_ptr_type& e, const op_type o);

	// default destructor

	const operand_ptr_type&
	get_operand(void) const { return ex; }

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o, const expr_dump_context&) const;

	size_t
	dimensions(void) const { return 0; }

	bool
	is_static_constant(void) const;

	bool
	is_relaxed_formal_dependent(void) const;

	value_type
	static_constant_value(void) const;

	static
	value_type
	evaluate(const op_type, const value_type);

	bool
	must_be_equivalent(const pint_expr& ) const;

	good_bool
	resolve_value(value_type& i) const;

	good_bool
	unroll_resolve_value(const unroll_context&, value_type& i) const;

	const_index_list
	resolve_dimensions(void) const;

	count_ptr<const pint_const>
	__unroll_resolve_rvalue(const unroll_context&, 
		const count_ptr<const pint_expr>&) const;

	count_ptr<const const_param>
	unroll_resolve_rvalues(const unroll_context&, 
		const count_ptr<const pint_expr>&) const;

	UNROLL_RESOLVE_COPY_PINT_PROTO;
	EXPR_ACCEPT_VISITOR_PROTO;

	SUBSTITUTE_DEFAULT_PARAMETERS_PINT_PROTO;
	using parent_type::substitute_default_positional_parameters;

protected:
	using parent_type::unroll_resolve_rvalues;
	using parent_type::unroll_resolve_copy;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};	// end class pint_unary_expr

//=============================================================================
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_EXPR_PINT_UNARY_EXPR_H__

