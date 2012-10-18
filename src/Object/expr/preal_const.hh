/**
	\file "Object/expr/preal_const.hh"
	Classes related to constant expressions.
	NOTE: this file was spanwed from "Object/art_object_expr_const.h"
		for revision history tracking purposes.  
	$Id: preal_const.hh,v 1.16 2010/09/21 00:18:20 fang Exp $
 */

#ifndef __HAC_OBJECT_EXPR_PREAL_CONST_H__
#define __HAC_OBJECT_EXPR_PREAL_CONST_H__

#include "Object/expr/preal_expr.hh"
#include "Object/expr/const_param.hh"
#include "util/memory/chunk_map_pool_fwd.hh"
#include "util/inttypes.h"

//=============================================================================
namespace HAC {
namespace entity {
class const_range_list;
USING_CONSTRUCT

//=============================================================================
/**
	Constant boolean parameters, true or false.  
 */
class preal_const : public preal_expr, public const_param {
private:
	typedef	preal_const		this_type;
	typedef	preal_expr		parent_type;
public:
	typedef	preal_value_type	value_type;
	/// cannont initialize non-integer static consts in-class
	static const value_type		default_value;
	static
	const value_type&
	safe_default_value(void) { return default_value; }
protected:
	// removed const-ness for assignability
	value_type		val;
private:
	preal_const();

public:
	explicit
	preal_const(const value_type v) :
		preal_expr(), const_param(), val(v) { }

	~preal_const() { }

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
	must_be_equivalent(const preal_expr& ) const;

	good_bool
	resolve_value(value_type& i) const;

	good_bool
	unroll_resolve_value(const unroll_context&, value_type& i) const;

	const_index_list
	resolve_dimensions(void) const;

	count_ptr<const preal_const>
	__unroll_resolve_rvalue(const unroll_context&, 
		const count_ptr<const preal_expr>&) const;

	count_ptr<const const_param>
	unroll_resolve_rvalues(const unroll_context&, 
		const count_ptr<const preal_expr>&) const;

	UNROLL_RESOLVE_COPY_PREAL_PROTO;
	EXPR_ACCEPT_VISITOR_PROTO;

	SUBSTITUTE_DEFAULT_PARAMETERS_PREAL_PROTO;

	preal_value_type
	to_real_const(void) const;

protected:
	using parent_type::unroll_resolve_rvalues;
#if OVERLOAD_VIRTUAL_USING
	using param_expr::unroll_resolve_rvalues;
#endif
	using parent_type::unroll_resolve_copy;
	using parent_type::nonmeta_resolve_copy;
	using parent_type::substitute_default_positional_parameters;
#if OVERLOAD_VIRTUAL_USING
	using param_expr::substitute_default_positional_parameters;
#endif

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
};	// end class preal_const

//=============================================================================
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_EXPR_PREAL_CONST_H__

