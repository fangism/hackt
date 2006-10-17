/**
	\file "Object/inst/inst_ref_implementation.h"
	Implementation details of instance references.  
 	$Id: inst_ref_implementation.h,v 1.13.16.6 2006/10/17 04:46:58 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_INST_REF_IMPLEMENTATION_H__
#define	__HAC_OBJECT_REF_INST_REF_IMPLEMENTATION_H__

#include <iostream>
#include "Object/devel_switches.h"
#include "Object/inst/substructure_alias_base.h"
#include "Object/traits/class_traits_fwd.h"
#include "Object/ref/inst_ref_implementation_fwd.h"
#include "Object/ref/simple_meta_indexed_reference_base.h"
#if REF_COUNT_INSTANCE_MANAGEMENT
#include "Object/ref/meta_instance_reference_base.h"
#endif
#include "Object/inst/instance_alias.h"
#include "Object/inst/alias_actuals.h"
#include "Object/state_manager.h"
#include "Object/global_entry.h"
#include "Object/unroll/unroll_context.h"
#include "common/ICE.h"
#include "util/stacktrace.h"
#include "util/packed_array.h"
#include "util/memory/excl_ptr.h"
#include "util/boolean_types.h"

namespace HAC {
namespace entity {
using util::bad_bool;
using util::memory::never_ptr;
using util::memory::count_ptr;
#include "util/using_ostream.h"
template <class> class simple_meta_instance_reference;

//=============================================================================
// struct simple_meta_instance_reference_implementation method definitions

/**
	Implementation specializations for meta tyes with substructure.
 */
template <>
struct simple_meta_instance_reference_implementation<true> {
#if USE_INSTANCE_PLACEHOLDERS
	template <class Tag>
	struct instance_placeholder_type {
		typedef	typename
			class_traits<Tag>::instance_placeholder_type
					type;
	};
#endif
	template <class Tag>
	struct instance_collection_generic_type {
		typedef	typename
			class_traits<Tag>::instance_collection_generic_type
					type;
	};

#if REF_COUNT_ARRAY_INDICES
	typedef	const count_ptr<
		const simple_meta_indexed_reference_base::index_list_type>&
				index_list_ptr_arg_type;
#else
	typedef	const never_ptr<
		const simple_meta_indexed_reference_base::index_list_type>
				index_list_ptr_arg_type;
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Called by member_instance_reference.
	\param lookup whether this performs a lookup translation of the 
		instance collection passed.  
 */
template <class Tag>
static
never_ptr<substructure_alias>
unroll_generic_scalar_substructure_reference(
		const typename
#if USE_INSTANCE_PLACEHOLDERS
			instance_placeholder_type<Tag>::type& inst, 
#else
			instance_collection_generic_type<Tag>::type& inst, 
#endif
		index_list_ptr_arg_type ind,
		const unroll_context& c
#if !USE_INSTANCE_PLACEHOLDERS
		, const bool lookup
#endif
		) {
#if USE_INSTANCE_PLACEHOLDERS
	return simple_meta_instance_reference<Tag>::
		__unroll_generic_scalar_reference(inst, ind, c);
#else
	return simple_meta_instance_reference<Tag>::
		__unroll_generic_scalar_reference(inst, ind, c, lookup);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if USE_INSTANCE_PLACEHOLDERS
// rename or overload?
template <class Tag>
static
never_ptr<substructure_alias>
unroll_generic_scalar_substructure_reference(
		const typename
			instance_collection_generic_type<Tag>::type& inst, 
		index_list_ptr_arg_type ind,
		const unroll_context& c) {
	return simple_meta_instance_reference<Tag>::
		__unroll_generic_scalar_reference_no_lookup(inst, ind, c);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Policy-determined: returns the expected instance reference alias. 
	This has a substructure.  
 */
template <class Tag>
static
never_ptr<substructure_alias>
simple_unroll_generic_scalar_substructure_reference(
		const simple_meta_instance_reference<Tag>& _this, 
		const unroll_context& c) {
	return _this.unroll_generic_scalar_reference(c);
}

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
simple_lookup_footprint_frame(
		const typename
#if USE_INSTANCE_PLACEHOLDERS
			instance_placeholder_type<Tag>::type& inst, 
#else
			instance_collection_generic_type<Tag>::type& inst, 
#endif
		index_list_ptr_arg_type ind,
		const state_manager& sm
#if SRC_DEST_UNROLL_CONTEXT_FOOTPRINTS
		, footprint& top
#endif
		) {
	STACKTRACE_VERBOSE;
#if SRC_DEST_UNROLL_CONTEXT_FOOTPRINTS
	const unroll_context uc(&top, &top);
#else
	const unroll_context uc(NULL, NULL);
#endif
#if USE_INSTANCE_PLACEHOLDERS
	const never_ptr<substructure_alias>
		alias(unroll_generic_scalar_substructure_reference<Tag>(
			inst, ind, uc));
#else
	const never_ptr<substructure_alias>
		alias(unroll_generic_scalar_substructure_reference<Tag>(
			inst, ind, uc, true));
#endif
	if (!alias) {
		cerr << "Error resolving a single instance alias." << endl;
		return NULL;
	}
	const size_t id = alias->instance_index;
	STACKTRACE_INDENT_PRINT("id = " << id << endl);
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
		const state_manager& sm
#if SRC_DEST_UNROLL_CONTEXT_FOOTPRINTS
		, footprint& top
#endif
		) {
	STACKTRACE_VERBOSE;
#if SRC_DEST_UNROLL_CONTEXT_FOOTPRINTS
	const size_t id = _this.lookup_globally_allocated_index(sm, top);
#else
	const size_t id = _this.lookup_globally_allocated_index(sm);
#endif
	STACKTRACE_INDENT_PRINT("id = " << id << endl);
	if (!id) {
		// already have error message
		return NULL;
	}
	return &sm.template get_pool<Tag>()[id]._frame;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
static
meta_instance_reference_base::port_connection_ptr_type
make_port_connection(
	const count_ptr<const simple_meta_instance_reference<Tag> >& r) {
	NEVER_NULL(r);
	return meta_instance_reference_base::port_connection_ptr_type(
		new port_connection<Tag>(r));
}

};	// end struct simple_meta_instance_reference_implementation<true>

//-----------------------------------------------------------------------------
/**
	Implementation specializations for meta tyes without substructure.
 */
template <>
struct simple_meta_instance_reference_implementation<false> {
#if USE_INSTANCE_PLACEHOLDERS
	template <class Tag>
	struct instance_placeholder_type {
		typedef	typename
			class_traits<Tag>::instance_placeholder_type
					type;
	};
#endif
	template <class Tag>
	struct instance_collection_generic_type {
		typedef	typename
			class_traits<Tag>::instance_collection_generic_type
					type;
	};

#if REF_COUNT_ARRAY_INDICES
	typedef	const count_ptr<
		const simple_meta_indexed_reference_base::index_list_type>&
				index_list_ptr_arg_type;
#else
	typedef	const never_ptr<
		const simple_meta_indexed_reference_base::index_list_type>
				index_list_ptr_arg_type;
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
static
never_ptr<substructure_alias>
unroll_generic_scalar_substructure_reference(
		const typename
#if USE_INSTANCE_PLACEHOLDERS
			instance_placeholder_type<Tag>::type& inst, 
#else
			instance_collection_generic_type<Tag>::type& inst, 
#endif
		index_list_ptr_arg_type ind,
		const unroll_context&
#if !USE_INSTANCE_PLACEHOLDERS
		, const bool
#endif
		) {
	STACKTRACE_VERBOSE;
	return never_ptr<substructure_alias>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if USE_INSTANCE_PLACEHOLDERS
// rename? or overload?
template <class Tag>
static
never_ptr<substructure_alias>
unroll_generic_scalar_substructure_reference(
		const typename
			instance_collection_generic_type<Tag>::type& inst, 
		index_list_ptr_arg_type ind,
		const unroll_context&) {
	STACKTRACE_VERBOSE;
	return never_ptr<substructure_alias>(NULL);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Has no substructure.  
 */
template <class Tag>
static
never_ptr<substructure_alias>
simple_unroll_generic_scalar_substructure_reference(
		const simple_meta_instance_reference<Tag>&,
		const unroll_context&) {
	STACKTRACE_VERBOSE;
	return never_ptr<substructure_alias>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Substructure-less types have no footprint frame.  
	This really should never be called.  
 */
template <class Tag>
static
const footprint_frame*
simple_lookup_footprint_frame(
#if USE_INSTANCE_PLACEHOLDERS
		const typename instance_placeholder_type<Tag>::type&, 
#else
		const typename instance_collection_generic_type<Tag>::type&, 
#endif
		index_list_ptr_arg_type,
		const state_manager&
#if SRC_DEST_UNROLL_CONTEXT_FOOTPRINTS
		, footprint& top
#endif
		) {
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
		const state_manager&
#if SRC_DEST_UNROLL_CONTEXT_FOOTPRINTS
		, footprint&
#endif
		) {
	// ICE?
	return NULL;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
static
meta_instance_reference_base::port_connection_ptr_type
make_port_connection(
	const count_ptr<const simple_meta_instance_reference<Tag> >& r) {
	NEVER_NULL(r);
	ICE_NEVER_CALL(cerr);
	return meta_instance_reference_base::port_connection_ptr_type(NULL);
}

};	// end struct simple_meta_instance_reference_implementation<false>

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_INST_REF_IMPLEMENTATION_H__

