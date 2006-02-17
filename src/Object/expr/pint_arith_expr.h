/**
	\file "Object/expr/pint_arith_expr.h"
	Arithmetic on integer parameters.  
	NOTE: this file was spawned from the old
		"Object/art_object_expr.h" for revision history tracking.  
	$Id: pint_arith_expr.h,v 1.6.18.1 2006/02/17 05:07:36 fang Exp $
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
using util::qmap;
using util::memory::count_ptr;

//=============================================================================
/**
	Binary arithmetic expression accepts ints and returns an int.  
 */
class pint_arith_expr : public pint_expr {
	typedef	pint_arith_expr			this_type;
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

private:
	// safe to use naked (never-delete) pointers on static objects
	typedef	expr_detail::op_info		op_info;
	typedef	qmap<char, const op_type*>	op_map_type;
	typedef	qmap<const op_type*, op_info>	reverse_op_map_type;
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

#if ENABLE_STATIC_DIMENSION_ANALYSIS
	bool
	has_static_constant_dimensions(void) const { return true; }

	const_range_list
	static_constant_dimensions(void) const;
#endif

	bool
	may_be_initialized(void) const
		{ return lx->may_be_initialized() && rx->may_be_initialized(); }

	bool
	must_be_initialized(void) const {
		return lx->must_be_initialized() && rx->must_be_initialized();
	}

	bool
	is_static_constant(void) const;

	bool
	is_relaxed_formal_dependent(void) const;

#if WANT_IS_TEMPLATE_DEPENDENT
	bool
	is_template_dependent(void) const;

	bool
	is_loop_independent(void) const;

	bool
	is_unconditional(void) const;
#endif

	value_type
	static_constant_value(void) const;

	bool
	must_be_equivalent(const pint_expr& ) const;

	good_bool
	resolve_value(value_type& i) const;

	good_bool
	unroll_resolve_value(const unroll_context&, value_type& i) const;

	const_index_list
	resolve_dimensions(void) const;

	count_ptr<const_param>
	unroll_resolve(const unroll_context&) const;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};	// end class pint_arith_expr

//=============================================================================
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_EXPR_PINT_ARITH_EXPR_H__

