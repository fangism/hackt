/**
	\file "Object/expr/pint_arith_expr.h"
	Arithmetic on integer parameters.  
	NOTE: this file was spawned from the old
		"Object/art_object_expr.h" for revision history tracking.  
	$Id: pint_arith_expr.h,v 1.12.4.2 2006/10/08 21:52:06 fang Exp $
 */

#ifndef __HAC_OBJECT_EXPR_PINT_ARITH_EXPR_H__
#define __HAC_OBJECT_EXPR_PINT_ARITH_EXPR_H__

#include "Object/expr/pint_expr.h"
#include "Object/expr/operator_info.h"
#include "util/memory/count_ptr.h"
#include "util/qmap.h"
#include "util/operators.h"

namespace HAC {
namespace entity {
using std::ostream;
USING_UTIL_OPERATIONS
using util::default_qmap;
using util::memory::count_ptr;

//=============================================================================
/**
	Binary arithmetic expression accepts ints and returns an int.  
	TODO: replace binary_arithmetic_operation with 
		non-virtual function pointers.  
 */
class pint_arith_expr : public pint_expr {
	typedef	pint_arith_expr			this_type;
	typedef	pint_expr			parent_type;
public:
	typedef	pint_value_type			arg_type;
	typedef	pint_value_type			value_type;
	typedef	count_ptr<const pint_expr>	operand_ptr_type;
	typedef	binary_arithmetic_operation<value_type, arg_type>
						op_type;
	static const plus<value_type, arg_type>		adder;
	static const minus<value_type, arg_type>	subtractor;
	static const multiplies<value_type, arg_type>	multiplier;
	static const divides<value_type, arg_type>	divider;
	static const modulus<value_type, arg_type>	remainder;
	static const shift_left<value_type, arg_type>	doubler;
	static const shift_right<value_type, arg_type>	halver;
	static const bitwise_and<value_type, arg_type>	masker;
	static const bitwise_or<value_type, arg_type>	unmasker;
	static const bitwise_xor<value_type, arg_type>	hasher;

private:
	// safe to use naked (never-delete) pointers on static objects
	typedef	expr_detail::op_info		op_info;
	typedef	char				op_key_type;
	typedef	default_qmap<op_key_type, const op_type*>::type
						op_map_type;
	typedef	default_qmap<const op_type*, op_info>::type
						reverse_op_map_type;
	static const op_map_type		op_map;
	static const reverse_op_map_type	reverse_op_map;
	static const size_t			op_map_size;
	static void op_map_register(const char, const op_type*, const char);
	static size_t op_map_init(void);
protected:
	operand_ptr_type		lx;
	operand_ptr_type		rx;

	/**
		Safe to use a naked pointer, b/c/ refers to a static object.  
	 */
	const op_type*			op;
private:
	pint_arith_expr();

	pint_arith_expr(const operand_ptr_type& l, const op_type*, 
		const operand_ptr_type& r);
public:
	// change: const ptr& arguments
	pint_arith_expr(const operand_ptr_type& l, const char o, 
		const operand_ptr_type& r);

	~pint_arith_expr();

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o, const expr_dump_context&) const;

	size_t
	dimensions(void) const { return 0; }

#if ENABLE_STATIC_ANALYSIS
	bool
	may_be_initialized(void) const {
		return lx->may_be_initialized() && rx->may_be_initialized();
	}

	bool
	must_be_initialized(void) const {
		return lx->must_be_initialized() && rx->must_be_initialized();
	}
#endif

	bool
	is_static_constant(void) const;

	bool
	is_relaxed_formal_dependent(void) const;

	value_type
	static_constant_value(void) const;

	static
	value_type
	evaluate(const op_key_type, const value_type, const value_type);

	static
	value_type
	evaluate(const op_type*, const value_type, const value_type);

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

#if SUBSTITUTE_DEFAULT_PARAMETERS
	SUBSTITUTE_DEFAULT_PARAMETERS_PINT_PROTO;
	using parent_type::substitute_default_positional_parameters;
#endif
protected:
	using parent_type::unroll_resolve_rvalues;
	using parent_type::unroll_resolve_copy;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};	// end class pint_arith_expr

//=============================================================================
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_EXPR_PINT_ARITH_EXPR_H__

