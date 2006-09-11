/**
	\file "Object/expr/pbool_logical_expr.h"
	Boolean relations between integer parameters.  
	NOTE: this file was spawned from the old
		"Object/art_object_expr.h" for revision history tracking.  
	$Id: pbool_logical_expr.h,v 1.12.8.1 2006/09/11 02:39:05 fang Exp $
 */

#ifndef __HAC_OBJECT_EXPR_PBOOL_LOGICAL_EXPR_H__
#define __HAC_OBJECT_EXPR_PBOOL_LOGICAL_EXPR_H__

#include "Object/expr/pbool_expr.h"
#include "util/memory/count_ptr.h"
#include "util/qmap.h"
#include "util/operators.h"

namespace HAC {
namespace entity {
using std::string;
using std::ostream;
USING_UTIL_OPERATIONS
using util::default_qmap;
using util::memory::count_ptr;

//=============================================================================
/**
	Binary logical expression accepts bools and returns a bool.  
	This also covers binary boolean relational operators ==, !=.
 */
class pbool_logical_expr : public pbool_expr {
	typedef	pbool_logical_expr			this_type;
	typedef	pbool_expr				parent_type;
public:
	typedef	pbool_value_type			value_type;
	typedef	pbool_value_type			arg_type;
	typedef	count_ptr<const pbool_expr>		operand_ptr_type;
	typedef	binary_logical_operation<value_type, arg_type>	op_type;
	static const util::logical_and<value_type, arg_type>	op_and;
	static const util::logical_or<value_type, arg_type>	op_or;
	static const util::logical_xor<value_type, arg_type>	op_xor;
	static const util::logical_xnor<value_type, arg_type>	op_xnor;
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
	count_ptr<const pbool_expr>	lx;
	count_ptr<const pbool_expr>	rx;
/**
	Pointer to the binary logical functor.  
 */
	const op_type*			op;

private:
	pbool_logical_expr();
public:
	pbool_logical_expr(const operand_ptr_type& l, const string& o, 
		const operand_ptr_type& r);
	pbool_logical_expr(const operand_ptr_type& l, const op_type* o, 
		const operand_ptr_type& r);

	~pbool_logical_expr();

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o, const expr_dump_context&) const;

	size_t
	dimensions(void) const { return 0; }

#if ENABLE_STATIC_ANALYSIS
	bool
	may_be_initialized(void) const
		{ return lx->may_be_initialized() && rx->may_be_initialized(); }

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
	evaluate(const string&, const value_type, const value_type);

	static
	value_type
	evaluate(const op_type*, const value_type, const value_type);

	bool
	must_be_equivalent(const pbool_expr& ) const;

	good_bool
	resolve_value(value_type& i) const;

	good_bool
	unroll_resolve_value(const unroll_context&, value_type& i) const;

	const_index_list
	resolve_dimensions(void) const;

	count_ptr<const pbool_const>
	__unroll_resolve_rvalue(const unroll_context&, 
		const count_ptr<const pbool_expr>&) const;

	count_ptr<const const_param>
	unroll_resolve_rvalues(const unroll_context&, 
		const count_ptr<const pbool_expr>&) const;

	UNROLL_RESOLVE_COPY_PBOOL_PROTO;
protected:
	using parent_type::unroll_resolve_rvalues;
	using parent_type::unroll_resolve_copy;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};	// end class pbool_logical_expr

//=============================================================================
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_EXPR_PBOOL_LOGICAL_EXPR_H__

