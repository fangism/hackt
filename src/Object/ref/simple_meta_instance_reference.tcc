/**
	\file "Object/ref/simple_meta_instance_reference.cc"
	Method definitions for the meta_instance_reference family of objects.
	This file was reincarnated from "Object/art_object_inst_ref.cc".
 	$Id: simple_meta_instance_reference.tcc,v 1.9 2006/01/22 18:20:29 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_SIMPLE_META_INSTANCE_REFERENCE_TCC__
#define	__HAC_OBJECT_REF_SIMPLE_META_INSTANCE_REFERENCE_TCC__

#include <iostream>

#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/expr/const_index_list.h"
#include "Object/expr/const_range_list.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/unroll/unroll_context.h"
#include "Object/def/footprint.h"
#include "util/what.h"
#include "util/packed_array.tcc"	// for packed_array_generic<>::resize()
	// will explicitly instantiate
#include "util/persistent_object_manager.tcc"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/inst/substructure_alias_base.h"
#include "Object/ref/inst_ref_implementation.h"
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
		common_base_type(pi->current_collection_state()), 
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
 */
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
ostream&
SIMPLE_META_INSTANCE_REFERENCE_CLASS::dump(ostream& o, 
		const expr_dump_context& c) const {
	return simple_meta_instance_reference_base::dump(o, c);
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
	typedef	simple_meta_instance_reference_implementation<
			class_traits<Tag>::has_substructure>
				substructure_implementation_policy;
	STACKTRACE_VERBOSE;
	const unroll_context uc;
	const instance_alias_base_ptr_type
		alias(__unroll_generic_scalar_reference(
			*this->inst_collection_ref, this->array_indices, uc));
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
	typedef	simple_meta_instance_reference_implementation<
			class_traits<Tag>::has_substructure>
				substructure_implementation_policy;
	STACKTRACE_VERBOSE;
	return substructure_implementation_policy::
		template lookup_footprint_frame<Tag>(
			*this->inst_collection_ref, this->array_indices, sm);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Re-usable helper function (also used by member_instance_reference).  
	TODO: what about global instance references?
	\param _inst a resolved actual instance (not formal).  
 */
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
bad_bool
SIMPLE_META_INSTANCE_REFERENCE_CLASS::unroll_references_helper(
		const unroll_context& c,
		const instance_collection_generic_type& _inst, 
		const never_ptr<const index_list_type> ind, 
		alias_collection_type& a) {
	// possibly factor this part out into simple_meta_instance_reference_base?
	STACKTRACE_VERBOSE;
	const footprint* const f(c.get_target_footprint());
#if 0
	if (f) {
		INVARIANT((*f)[_inst.get_name()]);
	}
#endif
	const instance_collection_generic_type&
		inst(f ? IS_A(const instance_collection_generic_type&, 
				*(*f)[_inst.get_name()])
			: _inst);
if (inst.get_dimensions()) {
	STACKTRACE("is array");
	const_index_list cil;
	if (ind) {
		cil = ind->unroll_resolve(c);
		if (cil.empty()) {
			cerr << "ERROR: Failed to resolve indices at "
				"unroll-time!" << endl;
			return bad_bool(true);
		}
	}
	// else empty, implicitly refer to whole collection if it is dense
	// we have resolve constant indices
	const const_index_list
		full_indices(inst.resolve_indices(cil));
	if (full_indices.empty()) {
		// might fail because implicit slice reference is not packed
		// more descriptive error message later...
		cerr << "ERROR: failed to resolve indices." << endl;
		return bad_bool(true);
	}
	// resize the array according to the collapsed dimensions, 
	// before passing it to unroll_aliases.
	{
	const const_range_list
		crl(full_indices.collapsed_dimension_ranges());
	const multikey_index_type
		array_sizes(crl.resolve_sizes());
	a.resize(array_sizes);
	// a.resize(upper -lower +ones);
	}

	// construct the range of aliases to collect
	const multikey_index_type lower(full_indices.lower_multikey());
	const multikey_index_type upper(full_indices.upper_multikey());
	// this will set the size and dimensions of packed_array a
	if (inst.unroll_aliases(lower, upper, a).bad) {
		cerr << "ERROR: unrolling aliases." << endl;
		return bad_bool(true);
	}
	// success!
	return bad_bool(false);
} else {
	STACKTRACE("is scalar");
	// is a scalar instance
	// size the alias_collection_type appropriately
	a.resize();		// empty
	const multikey_index_type bogus;
	if (inst.unroll_aliases(bogus, bogus, a).bad) {
		cerr << "ERROR: unrolling aliases." << endl;
		return bad_bool(true);
	}
	return bad_bool(false);
}
}	// end method unroll_references_helper

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
	return unroll_references_helper(c, *this->inst_collection_ref,
		this->array_indices, a);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This does the real work.
	(Many other calls are forwarded to this.)
 */
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
typename SIMPLE_META_INSTANCE_REFERENCE_CLASS::instance_alias_base_ptr_type
SIMPLE_META_INSTANCE_REFERENCE_CLASS::__unroll_generic_scalar_reference(
		const instance_collection_generic_type& inst, 
		const never_ptr<const index_list_type> ind, 
		const unroll_context& c) {
	typedef instance_alias_base_ptr_type 	return_type;
	STACKTRACE_VERBOSE;
	alias_collection_type aliases;
	const bad_bool
		bad(unroll_references_helper(c, inst, ind, aliases));
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
			this->array_indices, c);
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
	typedef	simple_meta_instance_reference_implementation<
			class_traits<Tag>::has_substructure>
				substructure_implementation_policy;
	STACKTRACE_VERBOSE;
	return substructure_implementation_policy::
		template unroll_generic_scalar_substructure_reference<Tag>(
			*this, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Expand both the port and the instance reference (this) into
		alias_collection_types, populated by resolved aliases, 
		and then connect them all if dimensions match.  
	NOTE: this need not be virtual because it already calls
		this->unroll_reference, which is virtual, and will give
		the desired result.  
	\param cl is a port member, a reference to a collection.
		Since c is a port, it must be densely packed if it's an array.
	\param c the unroll context.
 */
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
bad_bool
SIMPLE_META_INSTANCE_REFERENCE_CLASS::connect_port(
		instance_collection_base& cl, 
		const unroll_context& c) const {
	STACKTRACE_VERBOSE;
	// assert checked-cast, will throw bad_cast upon error
	instance_collection_generic_type&
		coll(IS_A(instance_collection_generic_type&, cl));

	alias_collection_type this_aliases;
	const bad_bool unroll_err(this->unroll_references(c, this_aliases));
		// calls unroll_reference virtually, thus
		// automatically handling member instance references.  
		// will automatically size the array
	if (unroll_err.bad) {
		cerr << "ERROR unrolling port actual reference "
			"during port connection: ";
		this->dump(cerr, expr_dump_context::default_value) << endl;
		return bad_bool(true);
	}
	// alternative, create a local temporary instance reference to coll?
	alias_collection_type port_aliases;
#if 0
	// parser interprets this as a function prototype! shown by util::wtf.
	// is this a parser bug?
	const this_type temp_ref(instance_collection_ptr_type(&coll));
#else
	const instance_collection_ptr_type temp_ptr(&coll);
	const this_type temp_ref(temp_ptr);
#endif
	// just like member_instance_reference::unroll
	// we suppress the footprint of the unroll context
	// when looking up ports.
	const unroll_context cc(c.make_member_context());
	// reference the whole port if it is collective (array)
	// by not attaching indices
	const bad_bool port_err(temp_ref.unroll_references(cc, port_aliases));
		// will automatically size the array
	if (unroll_err.bad) {
		cerr << "ERROR unrolling member instance reference "
			"during port connection: ";
		coll.dump(cerr) << endl;
		return bad_bool(true);
	}
	typedef typename alias_collection_type::key_type	key_type;
	const key_type t_size(this_aliases.size());
	const key_type p_size(port_aliases.size());
	if (t_size != p_size) {
		cerr << "ERROR sizes mismatch in port connection: " << endl;
		cerr << "\texpected: " << p_size << endl;
		cerr << "\tgot: " << t_size << endl;
		return bad_bool(true);
	}
	// else attempt to make connections, type-checking along the way
	typedef	typename alias_collection_type::iterator	alias_iterator;
	alias_iterator li(this_aliases.begin());
	const alias_iterator le(this_aliases.end());
	alias_iterator ri(port_aliases.begin());
	// the following copied from alias_connection::unroll's do-loop
	for ( ; li!=le; li++, ri++) {
		const never_ptr<instance_alias_base_type> lp(*li);
		const never_ptr<instance_alias_base_type> rp(*ri);
		NEVER_NULL(lp);
		NEVER_NULL(rp);
		if (!instance_alias_base_type::checked_connect_port(
				*lp, *rp).good) {
			// already have error message
			return bad_bool(true);
		}
	}
	INVARIANT(ri == port_aliases.end());
	return bad_bool(false);
}	// end method connect_port

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
excl_ptr<aliases_connection_base>
SIMPLE_META_INSTANCE_REFERENCE_CLASS::make_aliases_connection_private(void) const {
	return excl_ptr<aliases_connection_base>(new alias_connection_type);
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

