/**
	\file "art_object_inst_stmt_param.h"
	Contains definition of nested, specialized class_traits types.  
	$Id: art_object_inst_stmt_param.h,v 1.1.2.1 2005/03/07 23:28:49 fang Exp $
 */

#ifndef	__ART_OBJECT_INST_STMT_PARAM_H__
#define	__ART_OBJECT_INST_STMT_PARAM_H__

#include "art_object_classification_details.h"
#include "art_object_type_ref_base.h"
#include "art_built_ins.h"

namespace ART {
namespace entity {

//=============================================================================
class class_traits<pint_tag>::instantiation_statement_type_ref_base {
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

};      // end class instantiation_statement_type_ref_base

//=============================================================================
class class_traits<pbool_tag>::instantiation_statement_type_ref_base {
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

};      // end class instantiation_statement_type_ref_base

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INST_STMT_PARAM_H__

