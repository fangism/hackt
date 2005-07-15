/**
	\file "Object/art_object_unroll_context.h"
	Class for passing context duing unroll-phase.
	$Id: art_object_unroll_context.h,v 1.3.14.3 2005/07/15 03:49:18 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_UNROLL_CONTEXT_H__
#define	__OBJECT_ART_OBJECT_UNROLL_CONTEXT_H__

#include "util/memory/count_ptr.h"
#include "util/memory/excl_ptr.h"

namespace ART {
namespace entity {
// forward declarations
class const_param;
class template_actuals;
class template_formals_manager;
using util::memory::never_ptr;
using util::memory::count_ptr;

//=============================================================================
/**
	The context object used to pass around values in different scopes
	during the unroll phases.  
	TODO: add flow control stack, etc...
	TODO: obey lookup rules.  
 */
class unroll_context {
private:
	/**
		INVARIANT: template_args and template_formals are either
		both NULL or both valid at all times.  
	 */
	never_ptr<const template_actuals>		template_args;
	never_ptr<const template_formals_manager>	template_formals;
public:
	unroll_context();
	~unroll_context();

	bool
	empty(void) const;

	void
	set_transform_context(const template_actuals&,
		const template_formals_manager&);

	void
	reset_transform_context(void);

	bool
	have_template_actuals(void) const { return template_args; }

	template <class C>
	count_ptr<const const_param>
	resolve_meta_value_reference(const C&) const;

};	// end class unroll_context

//=============================================================================
/**
	Helper class for managing template actuals transformation.  
 */
class template_actuals_transformer {
	unroll_context& uc;
public:
	explicit
	template_actuals_transformer(unroll_context& c, 
		const template_actuals& a, 
		const template_formals_manager& m) : uc(c) {
		uc.set_transform_context(a, m);
	}

	~template_actuals_transformer() {
		uc.reset_transform_context();
	}

};	// end class template_actuals_transformer

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_UNROLL_CONTEXT_H__

