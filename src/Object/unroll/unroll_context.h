/**
	\file "Object/unroll/unroll_context.h"
	Class for passing context duing unroll-phase.
	This file was reincarnated from "Object/art_object_unroll_context.h".
	$Id: unroll_context.h,v 1.5.2.1 2006/01/17 20:55:26 fang Exp $
 */

#ifndef	__OBJECT_UNROLL_UNROLL_CONTEXT_H__
#define	__OBJECT_UNROLL_UNROLL_CONTEXT_H__

#include <iosfwd>
#include "util/memory/count_ptr.h"
#include "util/memory/excl_ptr.h"
#include "Object/type/template_actuals.h"

namespace HAC {
namespace entity {
// forward declarations
class const_param;
class pint_const;
class footprint;
class template_actuals;
class template_formals_manager;
class param_value_collection;
struct pint_tag;
struct pbool_tag;
template <class, size_t> class value_array;
using std::ostream;
using util::memory::never_ptr;
using util::memory::count_ptr;
template <class Tag> class unroll_context_value_resolver;

//=============================================================================
/**
	The context object used to pass around values in different scopes
	during the unroll phases.  
	TODO: add flow control stack, etc...
	TODO: obey lookup rules.  
	TODO: be able to fake actuals and formal to do loop-context!
	TODO: add footprint pointer for definition-scope instance
		reference translation.  
 */
class unroll_context {
	typedef	unroll_context				this_type;
	/**
		If true, then the template args are a copy of the 
		template actuals, not just a pointer hitherto.  
		This is a critical bug fix required by the 
		canonical_type's implementation of make_unroll_context, 
		because it creates a temporary list from a single
		const_param_expr_list from canonical_type_base.  

		The other option is to replace the pointer to 
		template actuals with a pointer to const_param_expr_list, 
		or a const_param_expr_list.  

		PLAN: first conver pointer to template_actuals, 
		then experiment around with const_param_expr_list.  
	 */
	typedef	template_actuals			template_args_type;
	typedef	value_array<pint_tag, 0>		pint_scalar;
	template <class Tag> friend class unroll_context_value_resolver;
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
	template_args_type				template_args;
	never_ptr<const template_formals_manager>	template_formals;

	/**
		If set to non-NULL, this is used to translate
		from placeholder instance collection reference 
		to definition-footprint's actual instance-collection.  
	 */
	footprint*					target_footprint;
public:
	// parameterless types and entity::module need this
	unroll_context();

	explicit
	unroll_context(footprint* const);

	unroll_context(const template_actuals&,
		const template_formals_manager&);

	unroll_context(const template_actuals&,
		const template_formals_manager&, footprint* const);

	unroll_context(const template_actuals&,
		const template_formals_manager&, const this_type&);

	~unroll_context();

	bool
	empty(void) const;

	ostream&
	dump(ostream&) const;

	// may bcome obsolete
	const footprint*
	get_target_footprint(void) const;

	bool
	in_definition_context(void) const { return target_footprint; }

	this_type
	make_member_context(void) const;

	void
	chain_context(const this_type&);

	bool
	have_template_actuals(void) const { return template_args; }

	count_ptr<const const_param>
	lookup_actual(const param_value_collection&) const;

protected:
	count_ptr<const pint_const>
	lookup_loop_var(const pint_scalar&) const;

private:
	static
	void
	lookup_panic(ostream&);

};	// end class unroll_context

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __OBJECT_UNROLL_UNROLL_CONTEXT_H__

