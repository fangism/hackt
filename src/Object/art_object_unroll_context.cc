/**
	\file "Object/art_object_unroll_context.cc"
	$Id: art_object_unroll_context.cc,v 1.3.14.2 2005/07/15 03:49:18 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_UNROLL_CONTEXT_CC__
#define	__OBJECT_ART_OBJECT_UNROLL_CONTEXT_CC__

#include "Object/art_object_unroll_context.h"

namespace ART {
namespace entity {
//=============================================================================
// class unroll_context method definitions

unroll_context::unroll_context() : template_args(), template_formals() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
unroll_context::~unroll_context() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
unroll_context::empty(void) const {
	return (!template_args && !template_formals);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
unroll_context::set_transform_context(const template_actuals& a, 
		const template_formals_manager& m) {
	INVARIANT(!template_args && !template_formals);
	template_args = never_ptr<const template_actuals>(&a);
	template_formals = never_ptr<const template_formals_manager>(&m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
unroll_context::reset_transform_context(void) {
	INVARIANT(template_args && template_formals);
	template_args = never_ptr<const template_actuals>();
	template_formals = never_ptr<const template_formals_manager>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_UNROLL_CONTEXT_CC__

