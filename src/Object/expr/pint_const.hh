/**
	\file "Object/expr/pint_const.hh"
	Classes related to constant expressions.
	NOTE: this file was spanwed from "Object/art_object_expr_const.h"
		for revision history tracking purposes.  
	$Id: pint_const.hh,v 1.21 2010/09/21 00:18:20 fang Exp $
 */

#ifndef __HAC_OBJECT_EXPR_PINT_CONST_H__
#define __HAC_OBJECT_EXPR_PINT_CONST_H__

#include "Object/expr/pint_expr.hh"
#include "Object/expr/const_index.hh"
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
	static
	value_type
	safe_default_value(void) { return default_value; }
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
	dump(ostream& o, const expr_dump_context&) const;

	size_t
	dimensions(void) const { return 0; }

	bool
	is_true(void) const;

	bool
	negative(void) const;

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

	// may chop '_int' off for templating
	value_type
	static_constant_value(void) const { return val; }

	bool
	must_be_equivalent(const pint_expr& ) const;

	bool
	operator == (const const_range& c) const;

	this_type&
	operator = (const value_type v) { val = v; return *this; }

	/// prefix incerment
	this_type&
	operator ++ () { ++val; return *this; }

	/// postfix increment (less efficient)
	this_type
	operator ++ (int) { const this_type ret(val); ++val; return ret; }

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

	count_ptr<const pint_const>
	__unroll_resolve_rvalue(const unroll_context&, 
		const count_ptr<const pint_expr>&) const;

	count_ptr<const const_param>
	unroll_resolve_rvalues(const unroll_context&, 
		const count_ptr<const pint_expr>&) const;

protected:
	using pint_expr::unroll_resolve_rvalues;
#if OVERLOAD_VIRTUAL_USING
	using param_expr::unroll_resolve_rvalues;
#endif

public:
	count_ptr<const_index>
	unroll_resolve_index(const unroll_context&) const;

protected:
	using pint_expr::unroll_resolve_copy;
	using pint_expr::nonmeta_resolve_copy;
#if OVERLOAD_VIRTUAL_USING
	using meta_index_expr::unroll_resolve_copy;
#endif

public:
	UNROLL_RESOLVE_COPY_PINT_PROTO;
	EXPR_ACCEPT_VISITOR_PROTO;

	SUBSTITUTE_DEFAULT_PARAMETERS_PINT_PROTO;
protected:
	using pint_expr::substitute_default_positional_parameters;
#if OVERLOAD_VIRTUAL_USING
	using param_expr::substitute_default_positional_parameters;
	using meta_index_expr::substitute_default_positional_parameters;
#endif

public:
	LESS_OPERATOR_PROTO;

	preal_value_type
	to_real_const(void) const;

private:
	MAKE_PARAM_EXPRESSION_ASSIGNMENT_PROTO;
public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS

	// would like to be as high as 1024
	enum {
#if	0 && defined(HAVE_UINT64_TYPE)
		pool_chunk_size = 64
#else
		pool_chunk_size = 32
#endif
	};

	CHUNK_MAP_POOL_ROBUST_STATIC_DECLARATIONS(pool_chunk_size)
	CHUNK_MAP_POOL_ESSENTIAL_FRIENDS(pool_chunk_size)
};	// end class pint_const

//=============================================================================
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_EXPR_PINT_CONST_H__

