/**
	\file "Object/expr/pstring_const.h"
	Classes related to constant string expressions.
	Copied-substituted from pbool_const.h.
	$Id: pstring_const.h,v 1.1.2.1 2010/09/08 21:14:23 fang Exp $
 */

#ifndef __HAC_OBJECT_EXPR_PSTRING_CONST_H__
#define __HAC_OBJECT_EXPR_PSTRING_CONST_H__

#include <string>
#include "Object/expr/pstring_expr.h"
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
class pstring_const : public pstring_expr, public const_param {
private:
	typedef	pstring_const		this_type;
public:
	typedef	pstring_value_type	value_type;
	// this is not a POD-type, and is statically, globally, initialized
	static const value_type&	default_value;
	static const value_type&	safe_default_value(void);
protected:
	// removed const-ness for assignability
	value_type			val;
private:
	pstring_const();

public:
	explicit
	pstring_const(const value_type& v);

	~pstring_const();

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o, const expr_dump_context&) const;

	ostream&
	dump_nonmeta(ostream&) const;

	size_t
	dimensions(void) const;

	bool
	is_true(void) const;

	bool
	has_static_constant_dimensions(void) const;

	const_range_list
	static_constant_dimensions(void) const;

	bool
	is_static_constant(void) const { return true; }

	bool
	is_relaxed_formal_dependent(void) const;

	count_ptr<const const_param>
	static_constant_param(void) const;

	value_type
	static_constant_value(void) const { return val; }

	bool
	must_be_equivalent(const pstring_expr& ) const;

	good_bool
	resolve_value(value_type& i) const;

	good_bool
	unroll_resolve_value(const unroll_context&, value_type& i) const;

	const_index_list
	resolve_dimensions(void) const;

	count_ptr<const pstring_const>
	__unroll_resolve_rvalue(const unroll_context&, 
		const count_ptr<const pstring_expr>&) const;

	count_ptr<const const_param>
	unroll_resolve_rvalues(const unroll_context&, 
		const count_ptr<const pstring_expr>&) const;

	using pstring_expr::unroll_resolve_rvalues;
	using pstring_expr::unroll_resolve_copy;
	using pstring_expr::nonmeta_resolve_copy;

	UNROLL_RESOLVE_COPY_PSTRING_PROTO;
	EXPR_ACCEPT_VISITOR_PROTO;

	SUBSTITUTE_DEFAULT_PARAMETERS_PSTRING_PROTO;
	using pstring_expr::substitute_default_positional_parameters;

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
};	// end class pstring_const

//=============================================================================
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_EXPR_PSTRING_CONST_H__

