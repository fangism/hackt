/**
	\file "Object/art_object_unroll_context.tcc"
	Template method implementation for unroll_context class.  
	$Id: art_object_unroll_context.tcc,v 1.1.4.2 2005/06/30 23:22:27 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_UNROLL_CONTEXT_TCC__
#define	__OBJECT_ART_OBJECT_UNROLL_CONTEXT_TCC__

#include "Object/art_object_unroll_context.h"
#include "Object/art_object_template_formals_manager.h"

namespace ART {
namespace entity {
//=============================================================================
// class unroll_context method definitions

/**
	\param C a const_collection type (pint, pbool, etc...)
	\param c the reference to the formal parameter, to be resolved
		as an actual.  
	\pre if c.is_template_formal() then this->have_template_actuals.
 */
template <class C>
const C&
unroll_context::resolve_meta_value_reference(const C& c) const {
	if (c.is_template_formal()) {
		INVARIANT(this->have_template_actuals());
		const param_instance_collection&
			ret(template_formals->resolve_template_actual(
				c, *template_args));
		// dynamic cast assertion
		return IS_A(const C&, ret);
	} else	return c;
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_UNROLL_CONTEXT_TCC__

