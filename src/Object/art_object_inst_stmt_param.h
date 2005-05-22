/**
	\file "Object/art_object_inst_stmt_param.h"
	Contains definition of nested, specialized class_traits types.  
	$Id: art_object_inst_stmt_param.h,v 1.4 2005/05/22 06:24:17 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INST_STMT_PARAM_H__
#define	__OBJECT_ART_OBJECT_INST_STMT_PARAM_H__

#include "Object/art_object_classification_details.h"
#include "Object/art_object_type_ref.h"
#include "Object/art_built_ins.h"

namespace ART {
namespace entity {

//=============================================================================
class class_traits<pint_tag>::instantiation_statement_type_ref_base :
	public empty_instantiation_statement_type_ref_base {
	// has no type member!
	// consider importing built-in type ref as a static member
protected:
	instantiation_statement_type_ref_base() { }

	/**
		Argument is ignored, but could assert that it is the
		same as pint_type_ptr.
	 */
	explicit
	instantiation_statement_type_ref_base(
		const type_ref_ptr_type&) { }

	count_ptr<const fundamental_type_reference>
	get_type(void) const { return pint_type_ptr; }

	const type_ref_ptr_type&
	unroll_type_reference(const unroll_context&) const {
		// trivial unrolling, context independent
		return pint_type_ptr;
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
class class_traits<pbool_tag>::instantiation_statement_type_ref_base :
	public empty_instantiation_statement_type_ref_base {
	// has no type member!
	// consider importing built-in type ref as a static member
protected:
	instantiation_statement_type_ref_base() { }

	/**
		Argument is ignored, but could assert that it is the
		same as pbool_type_ptr.
	 */
	explicit
	instantiation_statement_type_ref_base(
		const type_ref_ptr_type&) { }

	count_ptr<const fundamental_type_reference>
	get_type(void) const { return pbool_type_ptr; }

	const type_ref_ptr_type&
	unroll_type_reference(const unroll_context&) const {
		// trivial unrolling, context independent
		return pbool_type_ptr;
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
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_INST_STMT_PARAM_H__

