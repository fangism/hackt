/**
	\file "Object/expr/bool_logical_expr.h"
	Class definitions for boolean logical expressions.
	NOTE: this file was spanwed off of "Object/art_object_data_expr.h"
		for revision history tracking purposes.  
	$Id: bool_logical_expr.h,v 1.2 2005/07/20 21:00:38 fang Exp $
 */

#ifndef	__OBJECT_EXPR_BOOL_LOGICAL_EXPR_H__
#define	__OBJECT_EXPR_BOOL_LOGICAL_EXPR_H__

#include "Object/expr/types.h"
#include "Object/expr/bool_expr.h"
#include "util/string_fwd.h"
#include "util/qmap_fwd.h"
#include "util/operators.h"
#include "util/memory/count_ptr.h"

namespace ART {
namespace entity {
using std::string;
using util::qmap;
USING_UTIL_OPERATIONS
using util::persistent_object_manager;
//=============================================================================
/**
	Logical expressions on boolean data types.  
 */
class bool_logical_expr : public bool_expr {
	typedef	bool_logical_expr			this_type;
public:
	typedef bool_value_type				value_type;
	typedef	bool_value_type				arg_type;
	typedef	count_ptr<const bool_expr>		operand_ptr_type;
	typedef	binary_logical_operation<value_type, arg_type>	op_type;
	static const util::logical_and<value_type, arg_type>	op_and;
	static const util::logical_or<value_type, arg_type>	op_or;
	static const util::logical_xor<value_type, arg_type>	op_xor;
private:
	// safe to use naked (never-delete) pointers on static objects
	typedef qmap<string, const op_type*>	op_map_type;
	typedef qmap<const op_type*, string>	reverse_op_map_type;
public:
	static const op_map_type		op_map;
private:
	static const reverse_op_map_type	reverse_op_map;
	static const size_t			op_map_size;
	static void op_map_register(const string&, const op_type* );
	static size_t op_map_init(void);

protected:
	operand_ptr_type		lx;
	operand_ptr_type		rx;
/**
	Pointer to the binary logical functor.  
 */
	const op_type*                  op;

private:
	bool_logical_expr();
public:
	bool_logical_expr(const operand_ptr_type& l, const string& o,
		const operand_ptr_type& r);
	bool_logical_expr(const operand_ptr_type& l, const op_type* o,
		const operand_ptr_type& r);

	~bool_logical_expr();

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
};      // end class bool_logical_expr

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_EXPR_BOOL_LOGICAL_EXPR_H__

