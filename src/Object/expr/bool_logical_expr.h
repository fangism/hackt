/**
	\file "Object/expr/bool_logical_expr.h"
	Class definitions for boolean logical expressions.
	NOTE: this file was spanwed off of "Object/art_object_data_expr.h"
		for revision history tracking purposes.  
	$Id: bool_logical_expr.h,v 1.11.12.1 2006/12/12 10:17:37 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_BOOL_LOGICAL_EXPR_H__
#define	__HAC_OBJECT_EXPR_BOOL_LOGICAL_EXPR_H__

#include "Object/expr/types.h"
#include "Object/expr/bool_expr.h"
#include "util/string_fwd.h"
#include "util/qmap_fwd.h"
#include "util/operators.h"
#include "util/memory/count_ptr.h"

namespace HAC {
namespace entity {
using std::string;
using util::default_qmap;
USING_UTIL_OPERATIONS
using util::persistent_object_manager;
//=============================================================================
/**
	Logical expressions on boolean data types.  
 */
class bool_logical_expr : public bool_expr {
	typedef	bool_logical_expr			this_type;
	typedef	bool_expr				parent_type;
public:
	typedef bool_value_type				value_type;
	typedef	bool_value_type				arg_type;
	typedef	count_ptr<const bool_expr>		operand_ptr_type;
	typedef	binary_logical_operation<value_type, arg_type>	op_type;
	static const util::logical_and<value_type, arg_type>	op_and;
	static const util::logical_or<value_type, arg_type>	op_or;
	static const util::logical_xor<value_type, arg_type>	op_xor;
	static const util::logical_xnor<value_type, arg_type>	op_xnor;
private:
	// safe to use naked (never-delete) pointers on static objects
	typedef default_qmap<string, const op_type*>::type
							op_map_type;
	typedef default_qmap<const op_type*, string>::type
							reverse_op_map_type;
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
	EXPR_ACCEPT_VISITOR_PROTO;

protected:
	using parent_type::unroll_resolve_copy;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};      // end class bool_logical_expr

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_BOOL_LOGICAL_EXPR_H__

