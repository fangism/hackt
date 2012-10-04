/**
	\file "Object/expr/int_arith_expr.h"
	Class definitions for arithmetic int expressions.
	NOTE: this file was spanwed off of "Object/art_object_data_expr.h"
		for revision history tracking purposes.  
	$Id: int_arith_expr.h,v 1.13 2007/02/08 02:11:04 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_INT_ARITH_EXPR_H__
#define	__HAC_OBJECT_EXPR_INT_ARITH_EXPR_H__

#include <map>
#include "Object/expr/int_expr.h"
#include "Object/expr/types.h"
#include "Object/expr/operator_info.h"
#include "util/operators.h"
#include "util/memory/count_ptr.h"

namespace HAC {
namespace entity {
using std::map;
USING_UTIL_OPERATIONS
using util::persistent_object_manager;
//=============================================================================
/**
	Arithmetic operations on int data types.  
	Want to rip off operator map (or everything) from arith_expr...
 */
class int_arith_expr : public int_expr {
	typedef	int_arith_expr				this_type;
public:
	typedef	int_expr				parent_type;
	/**
		For arbitrary precision integers, will want a type
		fom GMP, such as mpz.  
	 */
	typedef	int_value_type		 		arg_type;
	typedef	int_value_type		 		value_type;
	typedef	binary_arithmetic_operation<value_type, arg_type>
							op_type;
	typedef	char					op_key_type;
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
	typedef	expr_detail::op_info			op_info;
public:
	typedef	map<op_key_type, const op_type*>	op_map_type;
	typedef	map<const op_type*, op_info>		reverse_op_map_type;
	static const op_map_type			op_map;
	static const reverse_op_map_type		reverse_op_map;
private:
	static const size_t				op_map_size;
	static void op_map_register(const op_key_type, 
		const op_type*, const char);
	static size_t op_map_init(void);
public:
	typedef	count_ptr<const int_expr>		operand_ptr_type;
protected:
	operand_ptr_type				lx;
	operand_ptr_type				rx;
	const op_type*					op;
private:
	int_arith_expr();

public:
	int_arith_expr(const operand_ptr_type&, const op_type*, 
		const operand_ptr_type&);
	int_arith_expr(const operand_ptr_type&, const char, 
		const operand_ptr_type&);
	~int_arith_expr();

	const operand_ptr_type&
	get_first(void) const { return lx; }

	const operand_ptr_type&
	get_second(void) const { return rx; }

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream& o, const expr_dump_context&) const;

	size_t
	dimensions(void) const { return 0; }

	GET_UNRESOLVED_DATA_TYPE_REF_PROTO;
	GET_RESOLVED_DATA_TYPE_REF_PROTO;

	UNROLL_RESOLVE_COPY_INT_PROTO;
	NONMETA_RESOLVE_RVALUE_INT_PROTO;
	NONMETA_RESOLVE_COPY_INT_PROTO;

	EXPR_ACCEPT_VISITOR_PROTO;

protected:
	using parent_type::unroll_resolve_copy;
	using parent_type::nonmeta_resolve_copy;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};	// end class int_arith_expr

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_INT_ARITH_EXPR_H__

