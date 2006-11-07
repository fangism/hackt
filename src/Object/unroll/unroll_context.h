/**
	\file "Object/unroll/unroll_context.h"
	Class for passing context duing unroll-phase.
	This file was reincarnated from "Object/art_object_unroll_context.h".
	$Id: unroll_context.h,v 1.15.2.5 2006/11/07 00:48:10 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_UNROLL_CONTEXT_H__
#define	__HAC_OBJECT_UNROLL_UNROLL_CONTEXT_H__

#include <iosfwd>
#include "util/memory/count_ptr.h"
#include "util/memory/excl_ptr.h"

namespace HAC {
namespace entity {
// forward declarations
class name_space;
class const_param;
class pint_const;
class footprint;
class template_actuals;
class template_formals_manager;
class instance_collection_base;
class param_value_placeholder;
class physical_instance_placeholder;
class physical_instance_collection;
class param_value_collection;
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
		If set to non-NULL, this is used to translate
		from placeholder instance collection reference 
		to definition-footprint's actual instance-collection.  
		This footprint is only used for instantiating.  
		This is propagated to children contexts for convenience
		and efficiently referencing the instantiating context.  
	 */
	footprint*					target_footprint;
	/**
		The lookup-only footprint associated with this context.  
		Failure to find lookup something here will continue
		looking up the parent context.  
		Lookup and value assignments are permitted, just not 
		instantiations.  
		This may point to the same footprint as the target footprint.
	 */
	const footprint*				lookup_footprint;
	/**
		Special top-level footprint for global parameter lookups.  
	 */
	const footprint*				top_footprint;
public:
	/**
		Constructor selection tag for the sake of 
		explicitly redirecting the target footprint, in the case
		of unrolling into an auxiliary footprint.  
	 */
	struct auxiliary_target_tag {};

	// called by top-module
	unroll_context(footprint* const, const footprint* const);
	// read-only footrint, no target
	unroll_context(const footprint* const, const unroll_context&);
	// called by everything else
	unroll_context(footprint* const, const unroll_context&);
	// called for auxiliary footprint unrolling
	unroll_context(footprint* const, const unroll_context&, 
		const auxiliary_target_tag);

	~unroll_context();

	ostream&
	dump(ostream&) const;

	const footprint*
	get_top_footprint(void) const { return top_footprint; }

	footprint&
	get_target_footprint(void) const;

	bool
	in_definition_context(void) const { return target_footprint; }

	this_type
	make_member_context(void) const;

	void
	chain_context(const this_type&);

	never_ptr<physical_instance_collection>
	lookup_instance_collection(const physical_instance_placeholder&) const;

	never_ptr<const physical_instance_collection>
	lookup_port_collection(const physical_instance_placeholder&) const;

	never_ptr<param_value_collection>
	lookup_lvalue_collection(const param_value_placeholder&) const;

	never_ptr<param_value_collection>
	lookup_rvalue_collection(const param_value_placeholder&) const;

	/// overloaded name call-forwarding for the lazy...
	never_ptr<physical_instance_collection>
	lookup_collection(const physical_instance_placeholder& p) const;

	/// overloaded name call-forwarding for the lazy...
	never_ptr<param_value_collection>
	lookup_collection(const param_value_placeholder& p) const;

#if 0
	count_ptr<const const_param>
	lookup_actual(const param_value_placeholder&) const;
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

