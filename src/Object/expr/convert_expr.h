/**
	\file "Object/expr/convert_expr.h"
	Classes to allow explicit or implicit conversions between
	expression types, such as pint_expr -> preal_expr.  
	$Id: convert_expr.h,v 1.2 2007/02/26 22:00:45 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_CONVERT_EXPR_H__
#define	__HAC_OBJECT_EXPR_CONVERT_EXPR_H__

#include "Object/expr/param_expr.h"
#include "util/memory/count_ptr.h"
#include "util/boolean_types.h"

namespace HAC {
namespace entity {
using util::memory::count_ptr;
using util::good_bool;

#define	CONVERT_EXPR_TEMPLATE_SIGNATURE	template <class To, class From>
#define	CONVERT_EXPR_CLASS		convert_expr<To,From>

//=============================================================================
/**
	Class to convert between meta-parameter rvalue expression types
	where it makes sense to.  (Like cast expressions in C++)
	For example: pint_expr -> preal_expr.  
	Alternative: remove the From type parameter and use a dynamic type.
	\param To the base class type.
	\param From the rvalue expression type.  
 */
CONVERT_EXPR_TEMPLATE_SIGNATURE
class convert_expr : public To {
	typedef	convert_expr<To, From>			this_type;
public:
	typedef	To		parent_type;
	typedef	From		rvalue_expr_type;
	typedef	typename parent_type::tag_type		tag_type;
	typedef	typename parent_type::value_type	value_type;
	typedef	typename parent_type::const_expr_type	const_expr_type;
	typedef	count_ptr<const rvalue_expr_type>	rvalue_ptr_type;
	typedef	typename rvalue_expr_type::const_expr_type
							const_rvalue_expr_type;
	typedef	typename rvalue_expr_type::value_type
							rvalue_value_type;
protected:
	rvalue_ptr_type					rvalue_expr;

	convert_expr();
public:
	explicit
	convert_expr(const rvalue_ptr_type&);

	~convert_expr();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	size_t
	dimensions(void) const;

	bool
	must_be_equivalent(const parent_type&) const;

	bool
	is_static_constant(void) const;

	bool
	is_relaxed_formal_dependent(void) const;

	count_ptr<const const_param>
	static_constant_param(void) const;

	value_type
	static_constant_value(void) const;

	good_bool
	unroll_resolve_value(const unroll_context&, value_type&) const;

	count_ptr<const const_expr_type>
	__unroll_resolve_rvalue(const unroll_context&, 
		const count_ptr<const parent_type>&) const;

	count_ptr<const const_param>
	unroll_resolve_rvalues(const unroll_context&, 
		const count_ptr<const parent_type>&) const;

	count_ptr<const parent_type>
	unroll_resolve_copy(const unroll_context&, 
		const count_ptr<const parent_type>&) const;

	count_ptr<const parent_type>
	substitute_default_positional_parameters(
		const template_formals_manager&, 
		const dynamic_param_expr_list&,
		const count_ptr<const parent_type>&) const;

	void
	accept(nonmeta_expr_visitor&) const;
protected:
	using parent_type::unroll_resolve_rvalues;
	using parent_type::unroll_resolve_copy;
	using parent_type::substitute_default_positional_parameters;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};	// end class convert_expr

//=============================================================================

}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_CONVERT_EXPR_H__

