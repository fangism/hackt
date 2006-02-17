/**
	\file "Object/unroll/param_instantiation_statement.tcc"
	Template method definitions for param_instantiation_statement.  
	This file came from "Object/art_object_inst_stmt_param.tcc"
		in a previous life.  
	$Id: param_instantiation_statement.tcc,v 1.5.18.1 2006/02/17 05:07:52 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_PARAM_INSTANTIATION_STATEMENT_TCC__
#define	__HAC_OBJECT_UNROLL_PARAM_INSTANTIATION_STATEMENT_TCC__

#include "Object/unroll/param_instantiation_statement.h"

#if WANT_PARAM_INSTANTIATION_STATEMENT_BASE

namespace HAC {
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
#if !UNIFY_UNROLL_PASS
/**
	Temporary implementation.
 */
PARAM_INSTANTIATION_STATEMENT_TEMPLATE_SIGNATURE
good_bool
PARAM_INSTANTIATION_STATEMENT_CLASS::unroll_meta_evaluate(
		const unroll_context& c) const {
	return parent_type::unroll(c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	No-op: params are evaluated in earlier pass.  
 */
PARAM_INSTANTIATION_STATEMENT_TEMPLATE_SIGNATURE
good_bool
PARAM_INSTANTIATION_STATEMENT_CLASS::unroll_meta_instantiate(
		const unroll_context& c) const {
	return good_bool(true);
}
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// WANT_PARAM_INSTANTIATION_STATEMENT_BASE

#endif	// __HAC_OBJECT_UNROLL_PARAM_INSTANTIATION_STATEMENT_TCC__

