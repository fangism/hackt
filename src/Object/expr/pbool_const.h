/**
	\file "Object/expr/pbool_const.h"
	Classes related to constant expressions.
	NOTE: this file was spanwed from "Object/art_object_expr_const.h"
		for revision history tracking purposes.  
	$Id: pbool_const.h,v 1.7.2.1 2006/02/19 03:52:54 fang Exp $
 */

#ifndef __HAC_OBJECT_EXPR_PBOOL_CONST_H__
#define __HAC_OBJECT_EXPR_PBOOL_CONST_H__

#include "Object/expr/pbool_expr.h"
#include "Object/expr/const_param.h"
#include "util/memory/list_vector_pool_fwd.h"

//=============================================================================
namespace HAC {
namespace entity {
class const_range_list;
USING_CONSTRUCT

//=============================================================================
/**
	Constant boolean parameters, true or false.  
 */
class pbool_const : public pbool_expr, public const_param {
private:
	typedef	pbool_const		this_type;
public:
	typedef	pbool_value_type	value_type;
	static const value_type		default_value = false;
protected:
	// removed const-ness for assignability
	value_type		val;
private:
	pbool_const();

public:
	explicit
	pbool_const(const value_type v) :
		pbool_expr(), const_param(), val(v) { }

	~pbool_const() { }

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o, const expr_dump_context&) const;

	size_t
	dimensions(void) const { return 0; }

	bool
	has_static_constant_dimensions(void) const { return true; }

	const_range_list
	static_constant_dimensions(void) const;

	bool
	may_be_initialized(void) const { return true; }

	bool
	must_be_initialized(void) const { return true; }

	bool
	is_static_constant(void) const { return true; }

	bool
	is_relaxed_formal_dependent(void) const { return false; }

	count_ptr<const const_param>
	static_constant_param(void) const;

	value_type
	static_constant_value(void) const { return val; }

	bool
	must_be_equivalent(const pbool_expr& ) const;

	good_bool
	resolve_value(value_type& i) const;

	good_bool
	unroll_resolve_value(const unroll_context&, value_type& i) const;

	const_index_list
	resolve_dimensions(void) const;

	count_ptr<const_param>
	unroll_resolve(const unroll_context&) const;

	LESS_OPERATOR_PROTO;

private:
	excl_ptr<param_expression_assignment>
	make_param_expression_assignment_private(
		const count_ptr<const param_expr>& p) const;
public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS

	LIST_VECTOR_POOL_ESSENTIAL_FRIENDS
	LIST_VECTOR_POOL_ROBUST_STATIC_DECLARATIONS
};	// end class pbool_const

//=============================================================================
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_EXPR_PBOOL_CONST_H__

