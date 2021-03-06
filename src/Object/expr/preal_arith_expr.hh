/**
	\file "Object/expr/preal_arith_expr.hh"
	Arithmetic on real-valued parameters.  
	$Id: preal_arith_expr.hh,v 1.13 2007/02/08 02:11:08 fang Exp $
 */

#ifndef __HAC_OBJECT_EXPR_PREAL_ARITH_EXPR_H__
#define __HAC_OBJECT_EXPR_PREAL_ARITH_EXPR_H__

#include <map>
#include "Object/expr/preal_expr.hh"
#include "Object/expr/operator_info.hh"
#include "util/memory/count_ptr.hh"
#include "util/operators.hh"

namespace HAC {
namespace entity {
using std::ostream;
USING_UTIL_OPERATIONS
using std::map;
using util::memory::count_ptr;

//=============================================================================
/**
	Binary arithmetic expression accepts ints and returns an int.  
 */
class preal_arith_expr : public preal_expr {
	typedef	preal_arith_expr			this_type;
public:
	typedef	preal_expr				parent_type;
	typedef	preal_value_type			arg_type;
	typedef	preal_value_type			value_type;
	typedef	count_ptr<const preal_expr>	operand_ptr_type;
	typedef	binary_arithmetic_operation<value_type, arg_type>
						op_type;
	typedef	char				op_key_type;
	static const plus<value_type, arg_type>		adder;
	static const minus<value_type, arg_type>	subtractor;
	static const multiplies<value_type, arg_type>	multiplier;
	static const divides<value_type, arg_type>	divider;
	static const modulus<value_type, arg_type>	remainder;

private:
	// safe to use naked (never-delete) pointers on static objects
	typedef	expr_detail::op_info		op_info;
public:
	typedef	map<op_key_type, const op_type*>	op_map_type;
	typedef	map<const op_type*, op_info>	reverse_op_map_type;
	static const op_map_type		op_map;
	static const reverse_op_map_type	reverse_op_map;
private:
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
	preal_arith_expr();
public:
	// change: const ptr& arguments
	preal_arith_expr(const operand_ptr_type& l, const char o, 
		const operand_ptr_type& r);

	preal_arith_expr(const operand_ptr_type& l, const op_type* o, 
		const operand_ptr_type& r);

	~preal_arith_expr();

	const operand_ptr_type&
	get_first(void) const { return lx; }

	const operand_ptr_type&
	get_second(void) const { return rx; }

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
	evaluate(const op_key_type, const value_type, const value_type);

	static
	value_type
	evaluate(const op_type*, const value_type, const value_type);

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
};	// end class preal_arith_expr

//=============================================================================
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_EXPR_PREAL_ARITH_EXPR_H__

