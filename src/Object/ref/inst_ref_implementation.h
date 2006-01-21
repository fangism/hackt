/**
	\file "Object/inst/inst_ref_implementation.h"
	Implementation details of instance references.  
 	$Id: inst_ref_implementation.h,v 1.5.2.1 2006/01/21 10:09:21 fang Exp $
 */

#ifndef	__OBJECT_REF_INST_REF_IMPLEMENTATION_H__
#define	__OBJECT_REF_INST_REF_IMPLEMENTATION_H__

#include <iostream>
#include "Object/inst/substructure_alias_base.h"
#include "Object/traits/class_traits_fwd.h"
#include "Object/ref/inst_ref_implementation_fwd.h"
#include "Object/ref/simple_meta_instance_reference_base.h"
// #include "Object/ref/simple_meta_instance_reference.h"
#include "Object/inst/instance_alias.h"
#include "Object/inst/alias_actuals.h"
#include "Object/state_manager.h"
#include "Object/global_entry.h"

#include "util/stacktrace.h"
#include "util/packed_array.h"
#include "util/memory/excl_ptr.h"
#include "util/boolean_types.h"
#include "util/wtf.h"

namespace HAC {
namespace entity {
using util::bad_bool;
using util::memory::never_ptr;
#include "util/using_ostream.h"
template <class> class simple_meta_instance_reference;
// template <bool> struct simple_meta_instance_reference_implementation { };

//=============================================================================
// struct simple_meta_instance_reference_implementation method definitions

/**
	Implementation specializations for meta tyes with substructure.
 */
template <>
struct simple_meta_instance_reference_implementation<true> {
	template <class Tag>
	struct instance_collection_generic_type {
		typedef	typename
			class_traits<Tag>::instance_collection_generic_type
					type;
	};

	typedef	never_ptr<
		const simple_meta_instance_reference_base::index_list_type>
				index_list_ptr_type;
#if 0
/**
	Called by member_instance_reference::unroll_references.
	This implementation should be policy-determined.  
	\return a single instance alias.
 */
template <class Tag>
static
never_ptr<substructure_alias_base<true> >
unroll_generic_scalar_reference(
		const typename
			instance_collection_generic_type<Tag>::type& inst, 
		const index_list_ptr_type ind,
		const unroll_context& c) {
	typedef	simple_meta_instance_reference<Tag>	inst_ref_type;
	typedef	typename inst_ref_type::alias_collection_type
						alias_collection_type;
	typedef	never_ptr<substructure_alias>		return_type;
	STACKTRACE_VERBOSE;
	alias_collection_type aliases;
	const bad_bool
		bad(inst_ref_type::unroll_references_helper(
			c, inst, ind, aliases));
	if (bad.bad) {
		return return_type(NULL);
	} else if (aliases.dimensions()) {
		cerr << "ERROR: got a " << aliases.dimensions() <<
			"-dimension collection where a scalar was required."
			<< endl;
		return return_type(NULL);
	} else {
		// util::wtf_is(aliases.front());
		return aliases.front();
	}
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Called by member_instance_reference.
 */
template <class Tag>
static
never_ptr<substructure_alias>
unroll_generic_scalar_substructure_reference(
		const typename
			instance_collection_generic_type<Tag>::type& inst, 
		const index_list_ptr_type ind,
		const unroll_context& c) {
	return simple_meta_instance_reference<Tag>::
		__unroll_generic_scalar_reference(inst, ind, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Policy-determined: returns the expected instance reference alias. 
	This has a substructure.  
 */
template <class Tag>
static
never_ptr<substructure_alias>
unroll_generic_scalar_substructure_reference(
		const simple_meta_instance_reference<Tag>& _this, 
		const unroll_context& c) {
	return _this.unroll_generic_scalar_reference(c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	\return the unique ID of the top-level allocated instance.  
 */
template <class Tag>
static
size_t
lookup_globally_allocated_index(
		const typename
			instance_collection_generic_type<Tag>::type& inst, 
		const index_list_ptr_type ind) {
	const unroll_context uc;
	const never_ptr<substructure_alias>
		alias(unroll_generic_scalar_reference<Tag>(inst, ind, uc));
	if (!alias) {
		cerr << "Error resolving a single instance alias." << endl;
		return NULL;
	}
	const size_t ret = alias->instance_index;
	INVARIANT(ret);
	return ret;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Appropriate for substructured types only.  
	Do NOT call this from member_instance_references, because
		this uses the instance_index directly, instead
		of a footprint_frame_map to retrieve the footprint_frame*.  
	TODO: optional, accept a context argument passed from the 
		top-level scope -- for meta-expression evaluation.  
 */
template <class Tag>
static
const footprint_frame*
lookup_footprint_frame(
		const typename
			instance_collection_generic_type<Tag>::type& inst, 
		const index_list_ptr_type ind,
		const state_manager& sm) {
	STACKTRACE_VERBOSE;
	const unroll_context uc;
	const never_ptr<substructure_alias>
		alias(unroll_generic_scalar_substructure_reference<Tag>(
			inst, ind, uc));
	if (!alias) {
		cerr << "Error resolving a single instance alias." << endl;
		return NULL;
	}
	const size_t id = alias->instance_index;
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT << "id = " << id << endl;
#endif
	INVARIANT(id);
	// TODO: bounds checking...
	return &sm.template get_pool<Tag>()[id]._frame;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
static
const footprint_frame*
member_lookup_footprint_frame(
		const member_meta_instance_reference<Tag>& _this, 
		const state_manager& sm) {
	STACKTRACE_VERBOSE;
	const size_t id = _this.lookup_globally_allocated_index(sm);
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT << "id = " << id << endl;
#endif
	if (!id) {
		// already have error message
		return NULL;
	}
	return &sm.template get_pool<Tag>()[id]._frame;
}

};	// end struct simple_meta_instance_reference_implementation<true>

//-----------------------------------------------------------------------------
/**
	Implementation specializations for meta tyes without substructure.
 */
template <>
struct simple_meta_instance_reference_implementation<false> {
	template <class Tag>
	struct instance_collection_generic_type {
		typedef	typename
			class_traits<Tag>::instance_collection_generic_type
					type;
	};

	typedef	never_ptr<
		const simple_meta_instance_reference_base::index_list_type>
				index_list_ptr_type;

#if 0
/**
	Technically this should never be called, but is implemented
	merely for interface consistency.  
 */
template <class Tag>
static
never_ptr<substructure_alias_base<false> >
unroll_generic_scalar_reference(
		const typename instance_collection_generic_type<Tag>::type&, 
		const index_list_ptr_type,
		const unroll_context&) {
	STACKTRACE_VERBOSE;
	typedef	simple_meta_instance_reference<Tag>	inst_ref_type;
	typedef	typename inst_ref_type::alias_collection_type
						alias_collection_type;
	typedef	never_ptr<substructure_alias_base<false> >	return_type;
	STACKTRACE_VERBOSE;
	alias_collection_type aliases;
	const bad_bool
		bad(inst_ref_type::unroll_references_helper(
			c, inst, ind, aliases));
	if (bad.bad) {
		return return_type(NULL);
	} else if (aliases.dimensions()) {
		cerr << "ERROR: got a " << aliases.dimensions() <<
			"-dimension collection where a scalar was required."
			<< endl;
		return return_type(NULL);
	} else {
		// util::wtf_is(aliases.front());
		return aliases.front();
	}
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
static
never_ptr<substructure_alias>
unroll_generic_scalar_substructure_reference(
		const typename
			instance_collection_generic_type<Tag>::type& inst, 
		const index_list_ptr_type ind,
		const unroll_context& c) {
	STACKTRACE_VERBOSE;
	return never_ptr<substructure_alias>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Has no substructure.  
 */
template <class Tag>
static
never_ptr<substructure_alias>
unroll_generic_scalar_substructure_reference(
		const simple_meta_instance_reference<Tag>&,
		const unroll_context& c) {
	STACKTRACE_VERBOSE;
	return never_ptr<substructure_alias>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
template <class Tag>
static
size_t
lookup_globally_allocated_index(
		const typename
			instance_collection_generic_type<Tag>::type& inst, 
		const index_list_ptr_type ind) {
	return 0;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Substructure-less types have no footprint frame.  
	This really should never be called.  
 */
template <class Tag>
static
const footprint_frame*
lookup_footprint_frame(
		const typename instance_collection_generic_type<Tag>::type&, 
		const index_list_ptr_type,
		const state_manager&) {
	// ICE?
	return NULL;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Should never be asking for a footprint_frame of a substructureless
	entity.  
 */
template <class Tag>
static
const footprint_frame*
member_lookup_footprint_frame(
		const member_meta_instance_reference<Tag>&, 
		const state_manager&) {
	// ICE?
	return NULL;
}

};	// end struct simple_meta_instance_reference_implementation<false>

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __OBJECT_REF_INST_REF_IMPLEMENTATION_H__

