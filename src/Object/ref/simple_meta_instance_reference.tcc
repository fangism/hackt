/**
	\file "Object/ref/simple_meta_instance_reference.cc"
	Method definitions for the meta_instance_reference family of objects.
	This file was reincarnated from "Object/art_object_inst_ref.cc".
 	$Id: simple_meta_instance_reference.tcc,v 1.17 2006/04/05 22:32:22 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_SIMPLE_META_INSTANCE_REFERENCE_TCC__
#define	__HAC_OBJECT_REF_SIMPLE_META_INSTANCE_REFERENCE_TCC__

#include <iostream>

#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/expr/const_range_list.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/common/dump_flags.h"
#include "Object/unroll/unroll_context.h"
#include "Object/def/footprint.h"
#include "common/TODO.h"
#include "common/ICE.h"
#include "util/what.h"
#include "util/persistent_object_manager.tcc"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/ref/aggregate_meta_instance_reference.h"
#include "Object/inst/substructure_alias_base.h"
#include "Object/inst/physical_instance_collection.h"
#include "Object/ref/inst_ref_implementation.h"
#include "Object/unroll/port_connection.h"
#include "util/stacktrace.h"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
using util::write_value;
using util::read_value;
using util::persistent_traits;

//=============================================================================
// class simple_meta_instance_reference method definitions

/**
	Private empty constructor.  
 */
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
SIMPLE_META_INSTANCE_REFERENCE_CLASS::simple_meta_instance_reference() :
		common_base_type(), 
		parent_type(), inst_collection_ref() {
	// no assert
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
SIMPLE_META_INSTANCE_REFERENCE_CLASS::simple_meta_instance_reference(
		const instance_collection_ptr_type pi) :
		common_base_type(), 
		parent_type(), 
		inst_collection_ref(pi) {
	NEVER_NULL(inst_collection_ref);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
SIMPLE_META_INSTANCE_REFERENCE_CLASS::~simple_meta_instance_reference() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
never_ptr<const instance_collection_base>
SIMPLE_META_INSTANCE_REFERENCE_CLASS::get_inst_base(void) const {
	return inst_collection_ref;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
ostream&
SIMPLE_META_INSTANCE_REFERENCE_CLASS::what(ostream& o) const {
	return o << util::what<this_type>::name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just wrapped around common base class implmentation.  
	Ripped off of the old simple_meta_indexed_reference_base::dump()
 */
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
ostream&
SIMPLE_META_INSTANCE_REFERENCE_CLASS::dump(ostream& o, 
		const expr_dump_context& c) const {
	if (c.include_type_info)
		this->what(o) << " ";
	NEVER_NULL(this->inst_collection_ref);
	if (c.enclosing_scope) {
		this->inst_collection_ref->dump_hierarchical_name(o,
			dump_flags::no_definition_owner);
	} else {
		this->inst_collection_ref->dump_hierarchical_name(o,
			dump_flags::default_value);
	}
	return simple_meta_indexed_reference_base::dump_indices(o, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Simplified from simple_meta_indexed_reference_base::dump_type_size().
 */
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
ostream&
SIMPLE_META_INSTANCE_REFERENCE_CLASS::dump_type_size(ostream& o) const {
	this->get_type_ref()->dump(o);
	const size_t d = this->dimensions();
	if (d) {
		o << '{' << d << "-dim}";
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
size_t
SIMPLE_META_INSTANCE_REFERENCE_CLASS::dimensions(void) const {
	size_t dim = this->inst_collection_ref->get_dimensions();
	if (array_indices) {
		const size_t c = this->array_indices->dimensions_collapsed();
		INVARIANT(c <= dim);
		return dim -c;
	} else	return dim;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
never_ptr<const definition_base>
SIMPLE_META_INSTANCE_REFERENCE_CLASS::get_base_def(void) const {
	return this->inst_collection_ref->get_base_def();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
count_ptr<const fundamental_type_reference>
SIMPLE_META_INSTANCE_REFERENCE_CLASS::get_type_ref(void) const {
	return this->inst_collection_ref->get_type_ref();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Copy-reduced from simple_meta_indexed_reference_base.
 */
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
good_bool
SIMPLE_META_INSTANCE_REFERENCE_CLASS::attach_indices(
		excl_ptr<index_list_type>& i) {
	INVARIANT(!array_indices);
	NEVER_NULL(i);
	// dimension-check:
	// number of indices must be <= dimension of instance collection.  
	const size_t max_dim = dimensions();    // depends on indices
	if (i->size() > max_dim) {
		cerr << "ERROR: instance collection " <<
			this->inst_collection_ref->get_name()
			<< " is " << max_dim << "-dimensional, and thus, "
			"cannot be indexed " << i->size() <<
			"-dimensionally!  ";
			// caller will say where
		return good_bool(false);
	}
	// no static dimension checking
	array_indices = i;
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	If this is called, we're at the top-level of the instance hierarchy.
	This should work regardless of whether this type has substructure.  
 */
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
size_t
SIMPLE_META_INSTANCE_REFERENCE_CLASS::lookup_globally_allocated_index(
		const state_manager& sm) const {
	STACKTRACE_VERBOSE;
	const unroll_context uc;
	const instance_alias_base_ptr_type
		alias(__unroll_generic_scalar_reference(
			*this->inst_collection_ref, this->array_indices,
			uc, true));
	if (!alias) {
		cerr << "Error resolving a single instance alias." << endl;
		return 0;
	}
	const size_t ret = alias->instance_index;
	INVARIANT(ret);
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Since this is a simple_meta_instance_reference, we're 
	at the top of the reference hierarchy.  
	We can just lookup the state_manager with the 
	base instance's index.  
	Implementation depends on whether or not this type
	can have subinstances, so we use a policy class.  
	\pre This should never be called for substructureless types.  
 */
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
const footprint_frame*
SIMPLE_META_INSTANCE_REFERENCE_CLASS::lookup_footprint_frame(
		const state_manager& sm) const {
	STACKTRACE_VERBOSE;
	return substructure_implementation_policy::
		template simple_lookup_footprint_frame<Tag>(
			*this->inst_collection_ref, this->array_indices, sm);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unrolls the reference into aliases.  
	\param c the context of unrolling.
	\param a the destination collection in which to return
		resolved instance aliases.  
	\return true on error, else false.
 */
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
bad_bool
SIMPLE_META_INSTANCE_REFERENCE_CLASS::unroll_references(
		const unroll_context& c, alias_collection_type& a) const {
	STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	this->dump(STACKTRACE_INDENT, expr_dump_context::default_value) << endl;
#endif
	return unroll_references_helper(c, *this->inst_collection_ref,
		this->array_indices, a);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This does the real work.
	(Many other calls are forwarded to this.)
	\param lookup whether or not need to translate reference to
		local instance collection to actual footprint
		instance collection.  
 */
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
typename SIMPLE_META_INSTANCE_REFERENCE_CLASS::instance_alias_base_ptr_type
SIMPLE_META_INSTANCE_REFERENCE_CLASS::__unroll_generic_scalar_reference(
		const instance_collection_generic_type& inst, 
		const never_ptr<const index_list_type> ind, 
		const unroll_context& c, 
		const bool lookup) {
	typedef instance_alias_base_ptr_type 	return_type;
	STACKTRACE_VERBOSE;
	alias_collection_type aliases;
	const bad_bool bad(lookup ? 
		unroll_references_helper(c, inst, ind, aliases) :
		unroll_references_helper_no_lookup(c, inst, ind, aliases));
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Called by member_instance_reference::unroll_references.
	This implementation should be policy-determined.  
	\return a single instance alias to a substructure_alias.
 */
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
typename SIMPLE_META_INSTANCE_REFERENCE_CLASS::instance_alias_base_ptr_type
SIMPLE_META_INSTANCE_REFERENCE_CLASS::unroll_generic_scalar_reference(
		const unroll_context& c) const {
	STACKTRACE_VERBOSE;
	return __unroll_generic_scalar_reference(
			*this->inst_collection_ref,
			this->array_indices, c, true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Called by member_instance_reference::unroll_references.
	This implementation should be policy-determined.  
	\return a single instance alias to a substructure_alias.
 */
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
never_ptr<substructure_alias>
SIMPLE_META_INSTANCE_REFERENCE_CLASS::unroll_scalar_substructure_reference(
		const unroll_context& c) const {
	STACKTRACE_VERBOSE;
	return substructure_implementation_policy::
		template simple_unroll_generic_scalar_substructure_reference<Tag>(
			*this, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
excl_ptr<port_connection_base>
SIMPLE_META_INSTANCE_REFERENCE_CLASS::make_port_connection_private(
		const count_ptr<const meta_instance_reference_base>& r) const {
	INVARIANT(r == this);
	return substructure_implementation_policy::make_port_connection(
		r.template is_a<const this_type>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
void
SIMPLE_META_INSTANCE_REFERENCE_CLASS::collect_transient_info_base(
		persistent_object_manager& m) const {
	common_base_type::collect_transient_info_base(m);
	inst_collection_ref->collect_transient_info(m);
	// instantiation_state has no pointers
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Visits children nodes and register pointers to object manager
	for serialization.  
	\param m the persistent object manager.
 */
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
void
SIMPLE_META_INSTANCE_REFERENCE_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	this->collect_transient_info_base(m);
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Common functionality for this->write_object and 
	member_SIMPLE_META_INSTANCE_REFERENCE_CLASS::write_object.
 */
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
void
SIMPLE_META_INSTANCE_REFERENCE_CLASS::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	m.write_pointer(o, inst_collection_ref);
	common_base_type::write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Writes the instance reference to output stream, translating
	pointers to indices as it goes along.  
	Note: the instantiation base must be written before the
		state information, for reconstruction purposes.  
	\param m the persistent object manager.  
 */
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
void
SIMPLE_META_INSTANCE_REFERENCE_CLASS::write_object(
		const persistent_object_manager& m, ostream& f) const {
	this->write_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Common functionality for this->load_object and
	member_SIMPLE_META_INSTANCE_REFERENCE_CLASS::load_object.
 */
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
void
SIMPLE_META_INSTANCE_REFERENCE_CLASS::load_object_base(
		const persistent_object_manager& m, istream& i) {
	m.read_pointer(i, inst_collection_ref);
	NEVER_NULL(inst_collection_ref);
#if 0
	// necessary? nope, just let the object_manager do it
	m.load_object_once(const_cast<instance_collection_generic_type*>(
		&*inst_collection_ref));
#endif
	common_base_type::load_object_base(m, i);
}
// else already visited

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Loads the instance reference from an input stream, translating
	indices to pointers.  
	Note: the instantiation base must be loaded before the
		state information, because the instantiation state
		depends on the instantiation base being complete.  
	\param m the persistent object manager.  
 */
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
void
SIMPLE_META_INSTANCE_REFERENCE_CLASS::load_object(
		const persistent_object_manager& m, istream& f) {
	this->load_object_base(m, f);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_SIMPLE_META_INSTANCE_REFERENCE_TCC__

