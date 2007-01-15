/**
	\file "Object/expr/int_relational_expr.h"
	Class definitions for relational int expressions.
	NOTE: this file was spanwed off of "Object/art_object_data_expr.h"
		for revision history tracking purposes.  
	$Id: int_relational_expr.h,v 1.10.12.3 2007/01/15 06:29:06 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_INT_RELATIONAL_EXPR_H__
#define	__HAC_OBJECT_EXPR_INT_RELATIONAL_EXPR_H__

#include "Object/expr/bool_expr.h"
#include "Object/expr/types.h"
#include "util/string_fwd.h"
#include "util/qmap_fwd.h"
#include "util/operators.h"
#include "util/memory/count_ptr.h"

namespace HAC {
namespace entity {
class int_expr;
using std::string;
using util::default_qmap;
USING_UTIL_OPERATIONS
using util::persistent_object_manager;
//=============================================================================
/**
	Integer arithmetic on integer data types.  
	(Non-meta language component)
 */
class int_relational_expr : public bool_expr {
	typedef	int_relational_expr		this_type;
	typedef	bool_expr			parent_type;
public:
	typedef	bool_value_type			value_type;
	typedef	int_value_type			arg_type;
	typedef count_ptr<const int_expr>	operand_ptr_type;
	typedef	binary_relational_operation<value_type, arg_type>
							op_type;
	static const equal_to<value_type, arg_type>	op_equal_to;
	static const not_equal_to<value_type, arg_type>	op_not_equal_to;
	static const less<value_type, arg_type>		op_less;
	static const greater<value_type, arg_type>	op_greater;
	static const less_equal<value_type, arg_type>	op_less_equal;
	static const greater_equal<value_type, arg_type>
							op_greater_equal;
private:
	// safe to use naked (never-delete) pointers on static objects
	typedef	default_qmap<string, const op_type*>::type
						op_map_type;
	typedef	default_qmap<const op_type*, string>::type
						reverse_op_map_type;
public:
	static const op_map_type		op_map;
private:
	static const reverse_op_map_type	reverse_op_map;
	static const size_t			op_map_size;
	static void op_map_register(const string&, const op_type* );
	static size_t op_map_init(void);

protected:
	operand_ptr_type			lx;
	operand_ptr_type			rx;
	/**
		Points to the operator functor.  
	 */
	const op_type*                  op;

private:
	int_relational_expr();
public:
	int_relational_expr(const operand_ptr_type&, const string& o,
		const operand_ptr_type&);
	int_relational_expr(const operand_ptr_type& l, const op_type* o,
		const operand_ptr_type& r);

	~int_relational_expr();

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

	GET_UNRESOLVED_DATA_TYPE_REF_PROTO;
	GET_RESOLVED_DATA_TYPE_REF_PROTO;

	UNROLL_RESOLVE_COPY_BOOL_PROTO;
	NONMETA_RESOLVE_RVALUE_BOOL_PROTO;
	NONMETA_RESOLVE_COPY_BOOL_PROTO;

	void
	accept(nonmeta_expr_visitor&) const;

protected:
	using parent_type::unroll_resolve_copy;
	using parent_type::nonmeta_resolve_copy;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};      // end class int_relational_expr

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_INT_RELATIONAL_EXPR_H__

