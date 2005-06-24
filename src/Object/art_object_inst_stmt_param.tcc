/**
	\file "Object/art_object_inst_stmt_param.tcc"
	Template method definitions for param_instantiation_statement.  
	$Id: art_object_inst_stmt_param.tcc,v 1.1.2.1 2005/06/24 19:02:57 fang Exp $
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
	unroll(c);
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
PARAM_INSTANTIATION_STATEMENT_TEMPLATE_SIGNATURE
void
PARAM_INSTANTIATION_STATEMENT_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	parent_type::collect_transient_info_base(m);
}
}
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_INST_STMT_PARAM_TCC__

