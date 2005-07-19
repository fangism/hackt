/**
	\file "Object/art_object_inst_ref.cc"
	Method definitions for the meta_instance_reference family of objects.
 	$Id: art_object_inst_ref.tcc,v 1.8.4.4 2005/07/19 23:28:24 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INST_REF_TCC__
#define	__OBJECT_ART_OBJECT_INST_REF_TCC__

#include <iostream>

#include "Object/art_object_inst_ref.h"
#include "Object/expr/const_index_list.h"
#include "Object/expr/const_range_list.h"
#include "util/what.h"
#include "util/packed_array.tcc"	// for packed_array_generic<>::resize()
	// will explicitly instantiate
#include "util/persistent_object_manager.tcc"
#include "Object/art_object_inst_ref_subtypes.h"
#include "Object/inst/substructure_alias_base.h"
#include "Object/ref/inst_ref_implementation.h"

namespace ART {
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
	Re-usable helper function (also used by member_instance_reference).  
	\param inst a resolved actual instance (not formal).  
 */
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
bad_bool
SIMPLE_META_INSTANCE_REFERENCE_CLASS::unroll_references_helper(
		const unroll_context& c,
		const instance_collection_generic_type& inst, 
		const never_ptr<const index_list_type> ind, 
		alias_collection_type& a) {
	// possibly factor this part out into simple_meta_instance_reference_base?
if (inst.get_dimensions()) {
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
	return unroll_references_helper(c, *this->inst_collection_ref,
		this->array_indices, a);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Called by member_instance_reference::unroll_references.
	This implementation should be policy-determined.  
	\return a single instance alias.
 */
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
never_ptr<substructure_alias>
SIMPLE_META_INSTANCE_REFERENCE_CLASS::unroll_generic_scalar_reference(
		const unroll_context& c) const {
	typedef	simple_meta_instance_reference_implementation<
			class_traits<Tag>::has_substructure>
				substructure_implementation_policy;
	return substructure_implementation_policy::
		template unroll_generic_scalar_reference<Tag>(
			*this->inst_collection_ref, this->array_indices, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 1
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
		this->dump(cerr) << endl;
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
		// reference the whole port if it is collective (array)
		// by not attaching indices
	const bad_bool port_err(temp_ref.unroll_references(c, port_aliases));
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
		if (!lp->must_match_type(*rp)) {
			// already have error message
			return bad_bool(true);
		}
		typedef	typename instance_alias_base_type::relaxed_actuals_type
					relaxed_actuals_type;
		const relaxed_actuals_type& la(lp->find_relaxed_actuals());
		const relaxed_actuals_type& ra(rp->find_relaxed_actuals());
		// no need to update a cnonical actuals
		if (!instance_alias_base_type::compare_actuals(la, ra).good) {
			// already have error message
			return bad_bool(true);
		}
		// else safe to connect
		lp->merge(*rp);
	}
	INVARIANT(ri == port_aliases.end());
	return bad_bool(false);
}
#endif

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
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_INST_REF_TCC__

