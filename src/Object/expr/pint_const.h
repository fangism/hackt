/**
	\file "Object/expr/pint_const.h"
	Classes related to constant expressions.
	NOTE: this file was spanwed from "Object/art_object_expr_const.h"
		for revision history tracking purposes.  
	$Id: pint_const.h,v 1.3 2005/09/04 21:14:47 fang Exp $
 */

#ifndef __OBJECT_EXPR_PINT_CONST_H__
#define __OBJECT_EXPR_PINT_CONST_H__

#include "Object/expr/pint_expr.h"
#include "Object/expr/const_index.h"
#include "Object/expr/const_param.h"
#include "util/memory/list_vector_pool_fwd.h"
// switch over to chunk_map_pool

//=============================================================================
namespace ART {
namespace entity {
class const_range_list;
USING_CONSTRUCT

//=============================================================================
/**
	Constant integer parameters.  
	Currently limited in width by the machine's long size.  
	Going to need a pool allocator...
 */
class pint_const : public pint_expr, public const_index, public const_param {
private:
	typedef	pint_const		this_type;
public:
	typedef pint_expr::value_type	value_type;
	static const value_type		default_value = 0;
protected:
	// removed constness for assignability
	value_type			val;
private:
	pint_const();
public:
	explicit
	pint_const(const value_type v) :
		pint_expr(), const_index(), const_param(), val(v) { }

	// change back to inline later
	~pint_const();

	ostream&
	what(ostream& o) const;

	ostream&
	dump_brief(ostream& o) const;

	ostream&
	dump(ostream& o) const;

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

#if 0
	bool
	may_be_equivalent(const param_expr& e) const
		{ return pint_expr::may_be_equivalent_generic(e); }

	bool
	must_be_equivalent(const param_expr& e) const
		{ return pint_expr::must_be_equivalent_generic(e); }
#endif

	bool
	is_static_constant(void) const { return true; }

	bool
	is_relaxed_formal_dependent(void) const { return false; }

	bool
	is_template_dependent(void) const { return false; }

	count_ptr<const const_param>
	static_constant_param(void) const;

	// may chop '_int' off for templating
	value_type
	static_constant_value(void) const { return val; }

	bool
	must_be_equivalent(const pint_expr& ) const;

	bool
	is_loop_independent(void) const { return true; }

	bool
	is_unconditional(void) const { return true; }

	bool
	operator == (const const_range& c) const;

	bool
	range_size_equivalent(const const_index& i) const;

	value_type
	lower_bound(void) const;

	value_type
	upper_bound(void) const;

	good_bool
	resolve_value(value_type& i) const;

	good_bool
	unroll_resolve_value(const unroll_context&, value_type& i) const;

	count_ptr<const_index>
	resolve_index(void) const;

	const_index_list
	resolve_dimensions(void) const;

	count_ptr<const_param>
	unroll_resolve(const unroll_context&) const;

	count_ptr<const_index>
	unroll_resolve_index(const unroll_context&) const;

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
};	// end class pint_const

//=============================================================================
}	// end namespace ART
}	// end namespace entity

#endif	// __OBJECT_EXPR_PINT_CONST_H__

