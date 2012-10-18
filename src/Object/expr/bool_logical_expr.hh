/**
	\file "Object/expr/bool_logical_expr.hh"
	Class definitions for boolean logical expressions.
	NOTE: this file was spanwed off of "Object/art_object_data_expr.h"
		for revision history tracking purposes.  
	$Id: bool_logical_expr.hh,v 1.13 2007/02/08 02:11:03 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_BOOL_LOGICAL_EXPR_H__
#define	__HAC_OBJECT_EXPR_BOOL_LOGICAL_EXPR_H__

#include <map>
#include "Object/expr/types.hh"
#include "Object/expr/bool_expr.hh"
#include "util/string_fwd.hh"
#include "util/operators.hh"
#include "util/memory/count_ptr.hh"

namespace HAC {
namespace entity {
using std::string;
using std::map;
USING_UTIL_OPERATIONS
using util::persistent_object_manager;
//=============================================================================
/**
	Logical expressions on boolean data types.  
 */
class bool_logical_expr : public bool_expr {
	typedef	bool_logical_expr			this_type;
public:
	typedef	bool_expr				parent_type;
	typedef bool_value_type				value_type;
	typedef	bool_value_type				arg_type;
	typedef	count_ptr<const bool_expr>		operand_ptr_type;
	typedef	binary_logical_operation<value_type, arg_type>	op_type;
	static const util::logical_and<value_type, arg_type>	op_and;
	static const util::logical_or<value_type, arg_type>	op_or;
	static const util::logical_xor<value_type, arg_type>	op_xor;
	static const util::logical_xnor<value_type, arg_type>	op_xnor;
	typedef	string					op_key_type;
public:
	// safe to use naked (never-delete) pointers on static objects
	typedef map<op_key_type, const op_type*>	op_map_type;
	typedef map<const op_type*, op_key_type>	reverse_op_map_type;
	static const op_map_type		op_map;
	static const reverse_op_map_type	reverse_op_map;
private:
	static const size_t			op_map_size;
	static void op_map_register(const op_key_type&, const op_type* );
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
	bool_logical_expr(const operand_ptr_type& l, const op_key_type& o,
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
	NONMETA_RESOLVE_RVALUE_BOOL_PROTO;
	NONMETA_RESOLVE_COPY_BOOL_PROTO;
	EXPR_ACCEPT_VISITOR_PROTO;

protected:
	using parent_type::unroll_resolve_copy;
	using parent_type::nonmeta_resolve_copy;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};      // end class bool_logical_expr

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_BOOL_LOGICAL_EXPR_H__

