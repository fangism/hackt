/**
	\file "Object/expr/preal_unary_expr.h"
	Unary negation of meta real value.  
	$Id: preal_unary_expr.h,v 1.10.12.2 2006/12/25 03:27:47 fang Exp $
 */

#ifndef __HAC_OBJECT_EXPR_PREAL_UNARY_EXPR_H__
#define __HAC_OBJECT_EXPR_PREAL_UNARY_EXPR_H__

#include "Object/expr/preal_expr.h"
#include "util/memory/count_ptr.h"

namespace HAC {
namespace entity {
using util::memory::count_ptr;

//=============================================================================
/**
	Only possibility: arithmetic negation.
 */
class preal_unary_expr : public preal_expr {
	typedef	preal_unary_expr			this_type;
	typedef	preal_expr				parent_type;
public:
	typedef	preal_value_type		value_type;
	typedef	char			op_type;
	typedef	count_ptr<const preal_expr>	operand_ptr_type;
protected:
	const op_type			op;
	/** expression argument must be 0-dimensional */
	operand_ptr_type		ex;
private:
	preal_unary_expr();
public:
	preal_unary_expr(const op_type o, const operand_ptr_type& e);
	preal_unary_expr(const operand_ptr_type& e, const op_type o);

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
	must_be_equivalent(const preal_expr& ) const;

	good_bool
	resolve_value(value_type& i) const;

	good_bool
	unroll_resolve_value(const unroll_context&, value_type& i) const;

	const_index_list
	resolve_dimensions(void) const;

	count_ptr<const preal_const>
	__unroll_resolve_rvalue(const unroll_context&, 
		const count_ptr<const preal_expr>&) const;

	count_ptr<const const_param>
	unroll_resolve_rvalues(const unroll_context&, 
		const count_ptr<const preal_expr>&) const;

	UNROLL_RESOLVE_COPY_PREAL_PROTO;
	EXPR_ACCEPT_VISITOR_PROTO;

	SUBSTITUTE_DEFAULT_PARAMETERS_PREAL_PROTO;
	using parent_type::substitute_default_positional_parameters;

protected:
	using parent_type::unroll_resolve_rvalues;
	using parent_type::unroll_resolve_copy;
	using parent_type::nonmeta_resolve_copy;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};	// end class preal_unary_expr

//=============================================================================
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_EXPR_PREAL_UNARY_EXPR_H__

