/**
	\file "Object/expr/pbool_const.h"
	Classes related to constant expressions.
	NOTE: this file was spanwed from "Object/art_object_expr_const.h"
		for revision history tracking purposes.  
	$Id: pbool_const.h,v 1.20.40.1 2010/09/08 21:14:21 fang Exp $
 */

#ifndef __HAC_OBJECT_EXPR_PBOOL_CONST_H__
#define __HAC_OBJECT_EXPR_PBOOL_CONST_H__

#include "Object/expr/pbool_expr.h"
#include "Object/expr/const_param.h"
#include "util/memory/chunk_map_pool_fwd.h"
#include "util/inttypes.h"

//=============================================================================
namespace HAC {
namespace entity {
class const_range_list;
USING_CONSTRUCT

//=============================================================================
/**
	Constant boolean parameters, true or false.  
	TODO: static const count_ptrs to be shared *everywhere*.
 */
class pbool_const : public pbool_expr, public const_param {
private:
	typedef	pbool_const		this_type;
public:
	typedef	pbool_value_type	value_type;
	static const value_type		default_value = false;
	static
	value_type
	safe_default_value(void) { return default_value; }
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
	is_true(void) const;

	bool
	has_static_constant_dimensions(void) const { return true; }

	const_range_list
	static_constant_dimensions(void) const;

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

	count_ptr<const pbool_const>
	__unroll_resolve_rvalue(const unroll_context&, 
		const count_ptr<const pbool_expr>&) const;

	count_ptr<const const_param>
	unroll_resolve_rvalues(const unroll_context&, 
		const count_ptr<const pbool_expr>&) const;

	using pbool_expr::unroll_resolve_rvalues;
	using pbool_expr::unroll_resolve_copy;
	using pbool_expr::nonmeta_resolve_copy;

	UNROLL_RESOLVE_COPY_PBOOL_PROTO;
	EXPR_ACCEPT_VISITOR_PROTO;

	SUBSTITUTE_DEFAULT_PARAMETERS_PBOOL_PROTO;
	using pbool_expr::substitute_default_positional_parameters;

	LESS_OPERATOR_PROTO;

private:
	MAKE_PARAM_EXPRESSION_ASSIGNMENT_PROTO;
public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS

	enum {
#if	0 && defined(HAVE_UINT64_TYPE)
		pool_chunk_size = 64
#else
		pool_chunk_size = 32
#endif
	};

	CHUNK_MAP_POOL_ESSENTIAL_FRIENDS(pool_chunk_size)
	CHUNK_MAP_POOL_ROBUST_STATIC_DECLARATIONS(pool_chunk_size)
};	// end class pbool_const

//=============================================================================
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_EXPR_PBOOL_CONST_H__

