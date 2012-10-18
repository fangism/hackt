/**
	\file "Object/expr/pstring_expr.hh"
	$Id: pstring_expr.hh,v 1.2 2010/09/21 00:18:21 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_PSTRING_EXPR_H__
#define	__HAC_OBJECT_EXPR_PSTRING_EXPR_H__

#include "Object/expr/string_expr.hh"
#include "Object/expr/param_expr.hh"
#include "Object/expr/types.hh"
#include "util/boolean_types.hh"

//=============================================================================
namespace HAC {
namespace entity {
struct pstring_tag;
class pstring_const;
class const_param;
class const_index_list;
class unroll_context;
class template_actuals;
using util::good_bool;
using util::bad_bool;
using util::memory::excl_ptr;
using util::memory::never_ptr;

//=============================================================================
/**
	Abstract expression checked to be a single boolean.  
 */
class pstring_expr : virtual public param_expr, public string_expr {
	typedef	pstring_expr			this_type;
public:
	typedef	pstring_tag			tag_type;
	/**
		The global boolean value type, set in "Object/expr/types.h".
		We bother with this typedef for the future potential of 
		using templates to extend to other parameter types.  
	 */
	typedef	pstring_value_type		value_type;
	typedef	pstring_const			const_expr_type;
public:
	pstring_expr() : param_expr(), string_expr() { }

	// temporary de-inline for debugging
virtual	~pstring_expr() { }

virtual	ostream&
	what(ostream& o) const = 0;

virtual	ostream&
	dump(ostream& o, const expr_dump_context&) const = 0;

virtual	size_t
	dimensions(void) const = 0;

	GET_UNRESOLVED_DATA_TYPE_REF_PROTO;
	GET_RESOLVED_DATA_TYPE_REF_PROTO;

	bool
	may_be_equivalent_generic(const param_expr& p) const;

	bool
	must_be_equivalent_generic(const param_expr& p) const;

virtual	bool
	must_be_equivalent(const pstring_expr& ) const = 0;

virtual bool
	is_static_constant(void) const = 0;

virtual	count_ptr<const const_param>
	static_constant_param(void) const;

virtual value_type
	static_constant_value(void) const = 0;

virtual	good_bool
	unroll_resolve_value(const unroll_context&, value_type&) const = 0;

virtual	count_ptr<const const_expr_type>
	__unroll_resolve_rvalue(const unroll_context&, 
		const count_ptr<const pstring_expr>&) const = 0;

	count_ptr<const const_param>
	unroll_resolve_rvalues(const unroll_context&, 
		const count_ptr<const param_expr>&) const;

virtual	count_ptr<const const_param>
	unroll_resolve_rvalues(const unroll_context&, 
		const count_ptr<const pstring_expr>&) const = 0;

#define	UNROLL_RESOLVE_COPY_PSTRING_PROTO				\
	count_ptr<const pstring_expr>					\
	unroll_resolve_copy(const unroll_context&, 			\
		const count_ptr<const pstring_expr>&) const

	UNROLL_RESOLVE_COPY_STRING_PROTO;

virtual	UNROLL_RESOLVE_COPY_PSTRING_PROTO = 0;

	NONMETA_RESOLVE_RVALUE_STRING_PROTO;
	NONMETA_RESOLVE_COPY_STRING_PROTO;

#define SUBSTITUTE_DEFAULT_PARAMETERS_PSTRING_PROTO			\
	count_ptr<const pstring_expr>					\
	substitute_default_positional_parameters(			\
		const template_formals_manager&,			\
		const dynamic_param_expr_list&,				\
		const count_ptr<const pstring_expr>&) const

	SUBSTITUTE_DEFAULT_PARAMETERS_PROTO;

virtual SUBSTITUTE_DEFAULT_PARAMETERS_PSTRING_PROTO = 0;

protected:
	using string_expr::unroll_resolve_copy;
	using string_expr::nonmeta_resolve_copy;

protected:
	MAKE_PARAM_EXPRESSION_ASSIGNMENT_PROTO;
	MAKE_AGGREGATE_META_VALUE_REFERENCE_PROTO;

};	// end class pstring_expr

//=============================================================================
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_EXPR_PSTRING_EXPR_H__
