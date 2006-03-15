/**
	\file "Object/unroll/param_instantiation_statement.h"
	Contains definition of nested, specialized class_traits types.  
	This file came from "Object/art_object_inst_stmt_param.h"
		in a previous life.  
	$Id: param_instantiation_statement.h,v 1.9 2006/03/15 04:38:24 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_PARAM_INSTANTIATION_STATEMENT_H__
#define	__HAC_OBJECT_UNROLL_PARAM_INSTANTIATION_STATEMENT_H__

#include "Object/inst/param_value_collection.h"
#include "Object/inst/value_collection.h"
#include "Object/unroll/empty_instantiation_statement_type_ref_base.h"
#include "Object/traits/pint_traits.h"
#include "Object/traits/pbool_traits.h"
#include "Object/traits/preal_traits.h"
#include "Object/type/param_type_reference.h"
#include "Object/expr/const_param_expr_list.h"

namespace HAC {
namespace entity {
class footprint;
//=============================================================================
/**
	Specialization of type-reference for parameter integers.  
 */
class class_traits<pint_tag>::instantiation_statement_type_ref_base :
	public empty_instantiation_statement_type_ref_base {
	typedef	class_traits<pint_tag>			traits_type;
	// has no type member!
	// consider importing built-in type ref as a static member
public:
	// probably null_parameter_type
	typedef	traits_type::instance_collection_parameter_type
					instance_collection_parameter_type;
protected:
	instantiation_statement_type_ref_base() { }

	/**
		Argument is ignored, but could assert that it is the
		same as pint_type_ptr.
	 */
	explicit
	instantiation_statement_type_ref_base(const type_ref_ptr_type&) { }

	// just ignore the parameters
	instantiation_statement_type_ref_base(const type_ref_ptr_type&, 
		const const_relaxed_args_type&) { }

	~instantiation_statement_type_ref_base() { }

	template <class InstStmtType>
	static
	void
	attach_initial_instantiation_statement(
			value_collection_generic_type& v,
			const never_ptr<const InstStmtType> i) {
		v.attach_initial_instantiation_statement(i);
	}

	const type_ref_ptr_type&
	get_type(void) const { return built_in_type_ptr; }

	const type_ref_ptr_type&
	unroll_type_reference(const unroll_context&) const {
		// trivial unrolling, context independent
		return built_in_type_ptr;
	}

	static
	good_bool
	commit_type_check(const value_collection_generic_type& v, 
			const instance_collection_parameter_type&) {
		// no need to type-check
		return good_bool(true);
	}

	static
	good_bool
	commit_type_first_time(value_collection_generic_type& v, 
			const instance_collection_parameter_type&) {
		// no-op
		return good_bool(true);
	}

	static
	good_bool
	instantiate_indices_with_actuals(value_collection_generic_type& v,
			const const_range_list& crl,
			const unroll_context&, 
			const instance_relaxed_actuals_type& a) {
		INVARIANT(!a);
		return v.instantiate_indices(crl);
	}

};      // end class instantiation_statement_type_ref_base

//=============================================================================
/**
	Specialization of type-reference for parameter integers.  
 */
class class_traits<preal_tag>::instantiation_statement_type_ref_base :
	public empty_instantiation_statement_type_ref_base {
	typedef	class_traits<preal_tag>			traits_type;
	// has no type member!
	// consider importing built-in type ref as a static member
public:
	typedef	count_ptr<const param_expr_list>	const_relaxed_args_type;
	typedef	count_ptr<const const_param_expr_list>
						instance_relaxed_actuals_type;
	// probably null_parameter_type
	typedef	traits_type::instance_collection_parameter_type
					instance_collection_parameter_type;
protected:
	instantiation_statement_type_ref_base() { }

	/**
		Argument is ignored, but could assert that it is the
		same as preal_type_ptr.
	 */
	explicit
	instantiation_statement_type_ref_base(const type_ref_ptr_type&) { }

	// just ignore the parameters
	instantiation_statement_type_ref_base(const type_ref_ptr_type&, 
		const const_relaxed_args_type&) { }

	~instantiation_statement_type_ref_base() { }

	template <class InstStmtType>
	static
	void
	attach_initial_instantiation_statement(
		value_collection_generic_type& v,
		const never_ptr<const InstStmtType> i) {
		v.attach_initial_instantiation_statement(i);
	}

	const type_ref_ptr_type&
	get_type(void) const { return built_in_type_ptr; }

	const type_ref_ptr_type&
	unroll_type_reference(const unroll_context&) const {
		// trivial unrolling, context independent
		return built_in_type_ptr;
	}

	static
	good_bool
	commit_type_check(const value_collection_generic_type& v, 
			const instance_collection_parameter_type&) {
		// no need to type-check
		return good_bool(true);
	}

	static
	good_bool
	commit_type_first_time(value_collection_generic_type& v, 
			const instance_collection_parameter_type&) {
		// no-op
		return good_bool(true);
	}

	static
	good_bool
	instantiate_indices_with_actuals(value_collection_generic_type& v,
			const const_range_list& crl,
			const unroll_context&, 
			const instance_relaxed_actuals_type& a) {
		INVARIANT(!a);
		return v.instantiate_indices(crl);
	}

};      // end class instantiation_statement_type_ref_base

//=============================================================================
/**
	Specialization of type-reference for parameter booleans.  
 */
class class_traits<pbool_tag>::instantiation_statement_type_ref_base :
	public empty_instantiation_statement_type_ref_base {
	typedef	class_traits<pbool_tag>			traits_type;
	// has no type member!
	// consider importing built-in type ref as a static member
public:
	typedef	traits_type::instance_collection_parameter_type
					instance_collection_parameter_type;
	typedef	count_ptr<const param_expr_list>	const_relaxed_args_type;
protected:
	typedef	count_ptr<const const_param_expr_list>
						instance_relaxed_actuals_type;
	instantiation_statement_type_ref_base() { }

	/**
		Argument is ignored, but could assert that it is the
		same as pbool_type_ptr.
	 */
	explicit
	instantiation_statement_type_ref_base(const type_ref_ptr_type&) { }

	// just ignore the parameters
	instantiation_statement_type_ref_base(const type_ref_ptr_type&, 
		const const_relaxed_args_type&) { }

	~instantiation_statement_type_ref_base() { }

	template <class InstStmtType>
	static
	void
	attach_initial_instantiation_statement(
		value_collection_generic_type& v,
		const never_ptr<const InstStmtType> i) {
		v.attach_initial_instantiation_statement(i);
	}

	const type_ref_ptr_type&
	get_type(void) const { return built_in_type_ptr; }

	const type_ref_ptr_type&
	unroll_type_reference(const unroll_context&) const {
		// trivial unrolling, context independent
		return built_in_type_ptr;
	}

	static
	good_bool
	commit_type_check(const value_collection_generic_type& v, 
			const instance_collection_parameter_type&) {
		// no need to type-check
		return good_bool(true);
	}

	static
	good_bool
	commit_type_first_time(value_collection_generic_type&, 
			const instance_collection_parameter_type&) {
		// no-op
		return good_bool(true);
	}

	static
	good_bool
	instantiate_indices_with_actuals(value_collection_generic_type& v,
			const const_range_list& crl,
			const unroll_context&, 
			const instance_relaxed_actuals_type& a) {
		INVARIANT(!a);
		return v.instantiate_indices(crl);
	}

};      // end class instantiation_statement_type_ref_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_PARAM_INSTANTIATION_STATEMENT_H__

