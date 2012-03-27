/**
	\file "Object/expr/param_defined.h"
	Implementation is defined in "Object/expr/operators.cc"
	$Id: $
 */

#ifndef	__HAC_OBJECT_EXPR_PARAM_DEFINED_H__
#define	__HAC_OBJECT_EXPR_PARAM_DEFINED_H__

#include "Object/expr/pbool_expr.h"
#include "util/memory/count_ptr.h"

namespace HAC {
namespace entity {
class meta_value_reference_base;
using util::memory::count_ptr;

/**
	Class that acts as a macro that returns whether 
	its argument is defined (value already set).  
	Was condering overloading pbool_unary_expr, but it makes
	more sense to restrict the operand to a param *reference*,
	as opposed to a general pbool_expr.  
	Constants are not suitable references.  
 */
class param_defined : public pbool_expr {
private:
	typedef	param_defined			this_type;
	typedef	pbool_expr			parent_type;
public:
	typedef	pbool_value_type		value_type;
	typedef	count_ptr<const meta_value_reference_base>
						operand_ptr_type;
protected:
	operand_ptr_type			arg;
private:
	param_defined();
public:
	explicit
	param_defined(const operand_ptr_type&);

	const operand_ptr_type&
	get_operand(void) const { return arg; }

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o, const expr_dump_context&) const;

	size_t
	dimensions(void) const { return 0; }

	bool
	is_static_constant(void) const;

	bool
	is_relaxed_formal_dependent(void) const;

	value_type
	static_constant_value(void) const;

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
	EXPR_ACCEPT_VISITOR_PROTO;

	SUBSTITUTE_DEFAULT_PARAMETERS_PBOOL_PROTO;
	using parent_type::substitute_default_positional_parameters;
protected:
	using parent_type::unroll_resolve_rvalues;
	using parent_type::unroll_resolve_copy;
	using parent_type::nonmeta_resolve_copy;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};	// end class param_defined

}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_PARAM_DEFINED_H__
