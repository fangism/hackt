/**
	\file "Object/art_object_inst_stmt_param.h"
	Contains definition of nested, specialized class_traits types.  
	$Id: art_object_inst_stmt_param.h,v 1.5.4.5 2005/07/07 06:02:21 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INST_STMT_PARAM_H__
#define	__OBJECT_ART_OBJECT_INST_STMT_PARAM_H__

#include "Object/art_object_inst_stmt_param_base.h"
#include "Object/art_object_pint_traits.h"
#include "Object/art_object_pbool_traits.h"
#include "Object/art_object_type_ref.h"
#include "Object/art_built_ins.h"
#include "Object/expr/param_expr_list.h"

namespace ART {
namespace entity {

//=============================================================================
/**
	Specialization of type-reference for parameter integers.  
 */
class class_traits<pint_tag>::instantiation_statement_type_ref_base :
	public empty_instantiation_statement_type_ref_base {
	// has no type member!
	// consider importing built-in type ref as a static member
public:
	typedef	count_ptr<const param_expr_list>	const_relaxed_args_type;
protected:
	instantiation_statement_type_ref_base() { }

	/**
		Argument is ignored, but could assert that it is the
		same as pint_type_ptr.
	 */
	explicit
	instantiation_statement_type_ref_base(const type_ref_ptr_type&) { }

	count_ptr<const fundamental_type_reference>
	get_type(void) const { return built_in_type_ptr; }

	/**
		Relaxed parameters to not apply to built-in parameter types.  
	 */
	const_relaxed_args_type
	get_relaxed_actuals(void) const {
		return const_relaxed_args_type(NULL);
	}

	const type_ref_ptr_type&
	unroll_type_reference(const unroll_context&) const {
		// trivial unrolling, context independent
		return built_in_type_ptr;
	}

	static
	good_bool
	commit_type_check(value_collection_generic_type& v, 
		const type_ref_ptr_type& t) {
		// no need to type-check
		return good_bool(true);
	}

};      // end class instantiation_statement_type_ref_base

//=============================================================================
/**
	Specialization of type-reference for parameter booleans.  
 */
class class_traits<pbool_tag>::instantiation_statement_type_ref_base :
	public empty_instantiation_statement_type_ref_base {
	// has no type member!
	// consider importing built-in type ref as a static member
public:
	typedef	count_ptr<const param_expr_list>	const_relaxed_args_type;
protected:
	instantiation_statement_type_ref_base() { }

	/**
		Argument is ignored, but could assert that it is the
		same as pbool_type_ptr.
	 */
	explicit
	instantiation_statement_type_ref_base(const type_ref_ptr_type&) { }

	count_ptr<const fundamental_type_reference>
	get_type(void) const { return built_in_type_ptr; }

	/**
		Relaxed parameters to not apply to built-in parameter types.  
	 */
	const_relaxed_args_type
	get_relaxed_actuals(void) const {
		return const_relaxed_args_type(NULL);
	}

	const type_ref_ptr_type&
	unroll_type_reference(const unroll_context&) const {
		// trivial unrolling, context independent
		return built_in_type_ptr;
	}

	static
	good_bool
	commit_type_check(value_collection_generic_type& v, 
		const type_ref_ptr_type& t) {
		// no need to type-check
		return good_bool(true);
	}

};      // end class instantiation_statement_type_ref_base

//=============================================================================
#define	PARAM_INSTANTIATION_STATEMENT_TEMPLATE_SIGNATURE		\
template <class Tag>

#define	PARAM_INSTANTIATION_STATEMENT_CLASS				\
param_instantiation_statement<Tag>

/**
	Instantiation statement refinement for parameter value instantiations.  
	Inherits everything from regular instantiation_statement template, 
	but implements unroll_meta_evaluate, of instance_management_base.
 */
PARAM_INSTANTIATION_STATEMENT_TEMPLATE_SIGNATURE
class param_instantiation_statement :
	public instantiation_statement<Tag> {
	typedef	PARAM_INSTANTIATION_STATEMENT_CLASS	this_type;
	typedef	instantiation_statement<Tag>		parent_type;
public:
	typedef	typename parent_type::type_ref_ptr_type	type_ref_ptr_type;
private:
	param_instantiation_statement();
public:
	param_instantiation_statement(const type_ref_ptr_type&, 
		const index_collection_item_ptr_type&);

	~param_instantiation_statement();

	UNROLL_META_EVALUATE_PROTO;
	UNROLL_META_INSTANTIATE_PROTO;	// no-op

	FRIEND_PERSISTENT_TRAITS

	using parent_type::collect_transient_info;
	using parent_type::write_object;
	using parent_type::load_object;
};	// end class param_instantiation_statement

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_INST_STMT_PARAM_H__

