/**
	\file "Object/unroll/unroll_context.h"
	Class for passing context duing unroll-phase.
	This file was reincarnated from "Object/art_object_unroll_context.h".
	$Id: unroll_context.h,v 1.2 2005/07/23 06:53:04 fang Exp $
 */

#ifndef	__OBJECT_UNROLL_UNROLL_CONTEXT_H__
#define	__OBJECT_UNROLL_UNROLL_CONTEXT_H__

#include <iosfwd>
#include "util/memory/count_ptr.h"
#include "util/memory/excl_ptr.h"

namespace ART {
namespace entity {
// forward declarations
class const_param;
class template_actuals;
class template_formals_manager;
class param_value_collection;
using std::ostream;
using util::memory::never_ptr;
using util::memory::count_ptr;

//=============================================================================
/**
	The context object used to pass around values in different scopes
	during the unroll phases.  
	TODO: add flow control stack, etc...
	TODO: obey lookup rules.  
	TODO: be able to fake actuals and formal to do loop-context!
 */
class unroll_context {
	typedef	unroll_context				this_type;
private:
	/**
		Stack-chain continuation to next context in scope.  
		Should point to a static (stack-allocated) unroll context, 
		such as one created in an outer scope.  
		(Enforce by comparing stack addresses?)
	 */
	never_ptr<const unroll_context>			next;
	/**
		INVARIANT: template_args and template_formals are either
		both NULL or both valid at all times.  
	 */
	never_ptr<const template_actuals>		template_args;
	never_ptr<const template_formals_manager>	template_formals;
public:
	// parameterless types and entity::modul need this
	unroll_context();

	unroll_context(const template_actuals&,
		const template_formals_manager&);
	unroll_context(const template_actuals&,
		const template_formals_manager&, const this_type&);
#if 0
	// so I don't have to think about which order of arguments
	unroll_context(const template_formals_manager&,
		const template_actuals&);
#endif
	~unroll_context();

	bool
	empty(void) const;

	ostream&
	dump(ostream&) const;

	void
	chain_context(const this_type&);

	bool
	have_template_actuals(void) const { return template_args; }

	count_ptr<const const_param>
	lookup_actual(const param_value_collection&) const;

private:
	static
	void
	lookup_panic(ostream&);

};	// end class unroll_context

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_UNROLL_UNROLL_CONTEXT_H__

