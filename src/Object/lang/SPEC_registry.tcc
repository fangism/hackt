/**
	\file "Object/lang/SPEC_registry.tcc"
	$Id: SPEC_registry.tcc,v 1.1.2.1 2006/04/20 03:34:52 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_SPEC_REGISTRY_TCC__
#define	__HAC_OBJECT_LANG_SPEC_REGISTRY_TCC__

#include "Object/lang/SPEC_registry.h"
#include "Object/expr/const_param_expr_list.h"

namespace HAC {
namespace entity {
namespace SPEC {
//=============================================================================
// class directive_definition method definitions

template <class VisitorType>
void
spec_visitor_entry<VisitorType>::main(visitor_type& c,
		const param_args_type& p, const node_args_type& n) const {
	NEVER_NULL(this->_main);
	(*this->_main)(c, p, n);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class VisitorType>
void
spec_visitor_entry<VisitorType>::main(visitor_type& c,
		const node_args_type& n) const {
	NEVER_NULL(this->_main);
	const param_args_type empty;
	(*this->_main)(c, empty, n);
}

//=============================================================================
}	// end namespace SPEC
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_SPEC_REGISTRY_TCC__

