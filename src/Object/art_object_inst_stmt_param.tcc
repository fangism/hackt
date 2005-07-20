/**
	\file "Object/art_object_inst_stmt_param.tcc"
	Template method definitions for param_instantiation_statement.  
	$Id: art_object_inst_stmt_param.tcc,v 1.2 2005/07/20 21:00:27 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INST_STMT_PARAM_TCC__
#define	__OBJECT_ART_OBJECT_INST_STMT_PARAM_TCC__

#include "Object/art_object_inst_stmt_param.h"

namespace ART {
namespace entity {
//=============================================================================
/**
	Private empty constructor.
 */
PARAM_INSTANTIATION_STATEMENT_TEMPLATE_SIGNATURE
PARAM_INSTANTIATION_STATEMENT_CLASS::param_instantiation_statement() :
		parent_type() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PARAM_INSTANTIATION_STATEMENT_TEMPLATE_SIGNATURE
PARAM_INSTANTIATION_STATEMENT_CLASS::param_instantiation_statement(
		const type_ref_ptr_type& t, 
		const index_collection_item_ptr_type& i) :
		parent_type(t, i) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PARAM_INSTANTIATION_STATEMENT_TEMPLATE_SIGNATURE
PARAM_INSTANTIATION_STATEMENT_CLASS::~param_instantiation_statement() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Temporary implementation.
 */
PARAM_INSTANTIATION_STATEMENT_TEMPLATE_SIGNATURE
good_bool
PARAM_INSTANTIATION_STATEMENT_CLASS::unroll_meta_evaluate(
		unroll_context& c) const {
	return parent_type::unroll(c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	No-op: params are evaluated in earlier pass.  
 */
PARAM_INSTANTIATION_STATEMENT_TEMPLATE_SIGNATURE
good_bool
PARAM_INSTANTIATION_STATEMENT_CLASS::unroll_meta_instantiate(
		unroll_context& c) const {
	return good_bool(true);
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_INST_STMT_PARAM_TCC__

