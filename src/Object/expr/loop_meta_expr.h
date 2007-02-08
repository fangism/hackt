/**
	\file "Object/expr/loop_meta_expr.h"
	$Id: loop_meta_expr.h,v 1.2 2007/02/08 02:11:04 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_LOOP_META_EXPR_H__
#define	__HAC_OBJECT_EXPR_LOOP_META_EXPR_H__

#include "Object/unroll/meta_loop_base.h"
#include "Object/expr/expr_fwd.h"
#include "util/boolean_types.h"
#include "util/persistent.h"

namespace HAC {
namespace entity {
class template_formals_manager;
class unroll_context;
class dynamic_param_expr_list;
struct expr_dump_context;
class nonmeta_expr_visitor;
using util::good_bool;

//=============================================================================
/**
	The pattern for this class is based on its non-loop binary
	operator counterpart.  
 */
template <class E>
class loop_meta_expr : 
		public meta_loop_base, 
		public E::parent_type {
	typedef	loop_meta_expr<E>		this_type;
	typedef	E				binary_expr_type;
	typedef	typename binary_expr_type::parent_type	parent_type;
public:
	typedef	parent_type				expr_base_type;
	typedef	typename parent_type::const_expr_type	const_expr_type;
	typedef	typename binary_expr_type::arg_type	arg_type;
	typedef	typename binary_expr_type::value_type	value_type;
	typedef	typename binary_expr_type::operand_ptr_type
							operand_ptr_type;
	typedef	typename binary_expr_type::op_type	op_type;
	typedef	typename binary_expr_type::op_key_type	op_key_type;
	typedef	meta_loop_base::ind_var_ptr_type	ind_var_ptr_type;
	typedef	meta_loop_base::range_ptr_type		range_ptr_type;
private:
	operand_ptr_type				ex;
	const op_type*					op;
private:
	loop_meta_expr();
public:
	loop_meta_expr(const ind_var_ptr_type&, const range_ptr_type&, 
		const operand_ptr_type&, const op_type*);
	~loop_meta_expr();


	const operand_ptr_type&
	get_expr(void) const { return this->ex; }

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
	must_be_equivalent(const expr_base_type& ) const;

	good_bool
	resolve_value(value_type& i) const;

	good_bool
	unroll_resolve_value(const unroll_context&, value_type& i) const;

	const_index_list
	resolve_dimensions(void) const;

	count_ptr<const const_expr_type>
	__unroll_resolve_rvalue(const unroll_context&, 
		const count_ptr<const expr_base_type>&) const;

	count_ptr<const const_param>
	unroll_resolve_rvalues(const unroll_context&, 
		const count_ptr<const expr_base_type>&) const;

	count_ptr<const expr_base_type>
	unroll_resolve_copy(const unroll_context&,
		const count_ptr<const expr_base_type>&) const;

	void
	accept(nonmeta_expr_visitor&) const;

	count_ptr<const expr_base_type>
	substitute_default_positional_parameters(       
		const template_formals_manager&,
		const dynamic_param_expr_list&,         
		const count_ptr<const expr_base_type>&) const;
	
	using parent_type::substitute_default_positional_parameters;
protected:
	using parent_type::unroll_resolve_rvalues;
	using parent_type::unroll_resolve_copy;
	using parent_type::nonmeta_resolve_copy;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS

};	// end class loop_meta_expr

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_LOOP_META_EXPR_H__

