/**
	\file "Object/inst/inst_ref_implementation.hh"
	Implementation details of instance references.  
 	$Id: inst_ref_implementation.hh,v 1.25 2010/08/24 21:05:48 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_INST_REF_IMPLEMENTATION_H__
#define	__HAC_OBJECT_REF_INST_REF_IMPLEMENTATION_H__

#include <iostream>
#include "Object/inst/substructure_alias_base.hh"
#include "Object/traits/class_traits_fwd.hh"
#include "Object/ref/inst_ref_implementation_fwd.hh"
#include "Object/ref/simple_meta_indexed_reference_base.hh"
#include "Object/ref/meta_instance_reference_base.hh"
#include "Object/inst/instance_alias_info.hh"
#include "Object/inst/alias_actuals.hh"
#include "Object/def/footprint.hh"
#include "Object/global_entry.hh"
#include "Object/unroll/unroll_context.hh"
#include "common/ICE.hh"
#include "util/stacktrace.hh"
#include "util/packed_array.hh"
#include "util/memory/excl_ptr.hh"
#include "util/boolean_types.hh"

namespace HAC {
namespace entity {
using util::bad_bool;
using util::memory::never_ptr;
using util::memory::count_ptr;
#include "util/using_ostream.hh"
template <class> class simple_meta_instance_reference;
template <class> class member_meta_instance_reference;
template <class> class port_connection;

//=============================================================================
// struct simple_meta_instance_reference_implementation method definitions

/**
	Implementation specializations for meta tyes with substructure.
 */
template <>
struct simple_meta_instance_reference_implementation<true> {
	template <class Tag>
	struct instance_placeholder_type {
		typedef	typename
			class_traits<Tag>::instance_placeholder_type
					type;
	};

	template <class Tag>
	struct collection_interface_type {
		typedef	collection_interface<Tag>	type;
	};

	typedef	const count_ptr<
		const simple_meta_indexed_reference_base::index_list_type>&
				index_list_ptr_arg_type;

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
		const typename instance_placeholder_type<Tag>::type& inst, 
		index_list_ptr_arg_type ind,
		const unroll_context& c) {
	return simple_meta_instance_reference<Tag>::
		__unroll_generic_scalar_reference(inst, ind, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// rename or overload?
template <class Tag>
static
never_ptr<substructure_alias>
unroll_generic_scalar_substructure_reference(
		const typename
			collection_interface_type<Tag>::type& inst, 
		index_list_ptr_arg_type ind,
		const unroll_context& c) {
	return simple_meta_instance_reference<Tag>::
		__unroll_generic_scalar_reference_no_lookup(inst, ind, c);
}

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
	template <class Tag>
	struct instance_placeholder_type {
		typedef	typename
			class_traits<Tag>::instance_placeholder_type
					type;
	};

	template <class Tag>
	struct collection_interface_type {
		typedef	collection_interface<Tag>	type;
	};

	typedef	const count_ptr<
		const simple_meta_indexed_reference_base::index_list_type>&
				index_list_ptr_arg_type;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
static
never_ptr<substructure_alias>
unroll_generic_scalar_substructure_reference(
		const typename instance_placeholder_type<Tag>::type& inst, 
		index_list_ptr_arg_type ind,
		const unroll_context&) {
	STACKTRACE_VERBOSE;
	return never_ptr<substructure_alias>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// rename? or overload?
template <class Tag>
static
never_ptr<substructure_alias>
unroll_generic_scalar_substructure_reference(
		const typename
			collection_interface_type<Tag>::type&, 
		index_list_ptr_arg_type,
		const unroll_context&) {
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
simple_unroll_generic_scalar_substructure_reference(
		const simple_meta_instance_reference<Tag>&,
		const unroll_context&) {
	STACKTRACE_VERBOSE;
	return never_ptr<substructure_alias>(NULL);
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

