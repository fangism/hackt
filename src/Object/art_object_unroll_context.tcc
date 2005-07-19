/**
	\file "Object/art_object_unroll_context.tcc"
	Template method implementation for unroll_context class.  
	$Id: art_object_unroll_context.tcc,v 1.1.4.6 2005/07/19 04:17:17 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_UNROLL_CONTEXT_TCC__
#define	__OBJECT_ART_OBJECT_UNROLL_CONTEXT_TCC__

#include "Object/art_object_unroll_context.h"
#include "Object/art_object_template_formals_manager.h"
#include "util/stacktrace.h"

namespace ART {
namespace entity {
//=============================================================================
// class unroll_context method definitions

#if 0
// OBSOLETE: replaced by non-template method lookup_actual() const
/**
	\param C a const_collection type (pint, pbool, etc...)
	\param c the reference to the formal parameter, to be resolved
		as an actual.  
	\pre if c.is_template_formal() then this->have_template_actuals.
 */
template <class C>
count_ptr<const const_param>
unroll_context::resolve_meta_value_reference(const C& c) const {
	STACKTRACE_VERBOSE;
#if 0
	this->dump(cerr) << endl;
#endif
#if 0
	if (c.is_template_formal()) {
		// TODO: this is not true
		// when template parameter depends on another parameter.  
		// I believe the correct thing to do is	fill in
		// template-dependent arguments at declaration time
#if 0
		c.dump(cerr << "c = ") << endl;
#endif
		INVARIANT(this->have_template_actuals());
		// could be const_param in general, 
		// not necessarily param_instance_collection
		// TODO: same code, factor out
		const count_ptr<const const_param>
			ret(template_formals->resolve_template_actual(
				c, *template_args));
#if 0
#if 1
		if (!IS_A(const C*, &ret)) {
			cerr << "Internal compiler error: "
				"expected C, but got: ";
			ret.what(cerr) << ": ";
			ret.dump(cerr) << endl;
			THROW_EXIT;
		}
#endif
		// dynamic cast assertion
		return IS_A(const C&, ret);
#else
		return ret;
#endif
	} else {
//		return IS_A(const const_param&, c);	// cross-cast assertion
		// same thing, will handle non-formals automatically
		return template_formals->resolve_template_actual(
			c, *template_args);
	}
#else
		return this->lookup_actual(c);
#endif
}
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_UNROLL_CONTEXT_TCC__

