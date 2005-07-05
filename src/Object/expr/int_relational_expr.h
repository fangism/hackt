/**
	\file "Object/expr/int_relational_expr.h"
	Class definitions for relational int expressions.
	NOTE: this file was spanwed off of "Object/art_object_data_expr.h"
		for revision history tracking purposes.  
	$Id: int_relational_expr.h,v 1.1.2.1 2005/07/05 01:16:29 fang Exp $
 */

#ifndef	__OBJECT_EXPR_INT_RELATIONAL_EXPR_H__
#define	__OBJECT_EXPR_INT_RELATIONAL_EXPR_H__

#include "Object/expr/bool_expr.h"
#include "Object/art_object_expr_types.h"
#include "util/string_fwd.h"
#include "util/qmap_fwd.h"
#include "util/operators.h"
#include "util/memory/count_ptr.h"

namespace ART {
namespace entity {
class int_expr;
using std::string;
using util::qmap;
USING_UTIL_OPERATIONS
using util::persistent_object_manager;
//=============================================================================
/**
	Integer arithmetic on integer data types.  
	(Non-meta language component)
 */
class int_relational_expr : public bool_expr {
	typedef	int_relational_expr		this_type;
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
	typedef	qmap<string, const op_type*>	op_map_type;
	typedef	qmap<const op_type*, string>	reverse_op_map_type;
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

	ostream&
	what(ostream& o) const;

	ostream&
	dump_brief(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	size_t
	dimensions(void) const { return 0; }

	GET_DATA_TYPE_REF_PROTO;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};      // end class int_relational_expr

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_EXPR_INT_RELATIONAL_EXPR_H__

