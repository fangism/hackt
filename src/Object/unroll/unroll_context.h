/**
	\file "Object/unroll/unroll_context.h"
	Class for passing context duing unroll-phase.
	This file was reincarnated from "Object/art_object_unroll_context.h".
	$Id: unroll_context.h,v 1.8.10.4.2.2 2006/09/11 22:04:14 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_UNROLL_CONTEXT_H__
#define	__HAC_OBJECT_UNROLL_UNROLL_CONTEXT_H__

#include <iosfwd>
#include "util/memory/count_ptr.h"
#include "util/memory/excl_ptr.h"
#include "Object/devel_switches.h"
#if !RESOLVE_VALUES_WITH_FOOTPRINT
#include "Object/type/template_actuals.h"
#endif

namespace HAC {
namespace entity {
// forward declarations
class name_space;
class const_param;
class pint_const;
class footprint;
class template_actuals;
class template_formals_manager;
#if USE_INSTANCE_PLACEHOLDERS
class instance_collection_base;
class param_value_placeholder;
class physical_instance_placeholder;
#endif
class physical_instance_collection;
class param_value_collection;
#if !RESOLVE_VALUES_WITH_FOOTPRINT
struct pint_tag;
struct pbool_tag;
template <class, size_t> class value_array;
#endif
using std::ostream;
using util::memory::never_ptr;
using util::memory::count_ptr;
template <class Tag> class unroll_context_value_resolver;

//=============================================================================
/**
	TODO: 2006-09-01, with introduction of placeholders.  
	Defer all lookups of values to the footprint's instance/value map,
		i.e. copy all values over to footprint, for uniformly
		consistent lookup method.
	Standard lookup will translate placeholder to collection/actual.  
	Thus, remove all references and uses of template formals
		and actuals.  

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
#if !RESOLVE_VALUES_WITH_FOOTPRINT
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
#endif
	template <class Tag> friend class unroll_context_value_resolver;
private:
	/**
		Stack-chain continuation to next context in scope.  
		Should point to a static (stack-allocated) unroll context, 
		such as one created in an outer scope.  
		(Enforce by comparing stack addresses?)
	 */
	never_ptr<const unroll_context>			next;
#if !RESOLVE_VALUES_WITH_FOOTPRINT
	/**
		INVARIANT: template_args and template_formals are either
		both NULL or both valid at all times.  
	 */
	template_args_type				template_args;
	never_ptr<const template_formals_manager>	template_formals;
#endif

	/**
		If set to non-NULL, this is used to translate
		from placeholder instance collection reference 
		to definition-footprint's actual instance-collection.  
	 */
	footprint*					target_footprint;

	/**
		NOTE: 2006-09-10
		TODO: consider a read-only source footprint, because now
		other nested scopes use footprints for unrolling.  
	**/
#if LOOKUP_GLOBAL_META_PARAMETERS
	never_ptr<const name_space>			parent_namespace;
#endif
public:
	// parameterless types and entity::module need this
	unroll_context();

	explicit
	unroll_context(footprint* const);

#if !RESOLVE_VALUES_WITH_FOOTPRINT
	unroll_context(const template_actuals&,
		const template_formals_manager&);

	unroll_context(const template_actuals&,
		const template_formals_manager&, footprint* const
#if LOOKUP_GLOBAL_META_PARAMETERS
		, const never_ptr<const name_space>
#endif
		);

	unroll_context(const template_actuals&,
		const template_formals_manager&, const this_type&);
#endif

	~unroll_context();

#if !RESOLVE_VALUES_WITH_FOOTPRINT
	bool
	empty(void) const;
#endif

	ostream&
	dump(ostream&) const;

	// may bcome obsolete
	const footprint*
	get_target_footprint(void) const;

#if LOOKUP_GLOBAL_META_PARAMETERS
	never_ptr<const name_space>
	get_parent_namespace(void) const;
#endif

	bool
	in_definition_context(void) const { return target_footprint; }

	this_type
	make_member_context(void) const;

	void
	chain_context(const this_type&);

#if !RESOLVE_VALUES_WITH_FOOTPRINT
	bool
	have_template_actuals(void) const { return template_args; }
#endif

#if USE_INSTANCE_PLACEHOLDERS
	void
	instantiate_collection(
		const count_ptr<instance_collection_base>&) const;

	count_ptr<physical_instance_collection>
	lookup_instance_collection(const physical_instance_placeholder&) const;

	count_ptr<param_value_collection>
	lookup_value_collection(const param_value_placeholder&) const;

	/// overloaded name call-forwarding for the lazy...
	count_ptr<physical_instance_collection>
	lookup_collection(const physical_instance_placeholder& p) const;

	/// overloaded name call-forwarding for the lazy...
	count_ptr<param_value_collection>
	lookup_collection(const param_value_placeholder& p) const;
#endif

#if USE_INSTANCE_PLACEHOLDERS
	count_ptr<const const_param>
	lookup_actual(const param_value_placeholder&) const;
#else
	count_ptr<const const_param>
	lookup_actual(const param_value_collection&) const;
#endif

#if !RESOLVE_VALUES_WITH_FOOTPRINT
protected:
	// flagged: OBSOLETE
	count_ptr<const pint_const>
	lookup_loop_var(const pint_scalar&) const;
#endif

private:
	static
	void
	lookup_panic(ostream&);

};	// end class unroll_context

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_UNROLL_CONTEXT_H__

