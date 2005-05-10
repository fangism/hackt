/**
	\file "Object/art_object_inst_ref.cc"
	Method definitions for the instance_reference family of objects.
 	$Id: art_object_inst_ref.tcc,v 1.6 2005/05/10 04:51:14 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INST_REF_TCC__
#define	__OBJECT_ART_OBJECT_INST_REF_TCC__

#include <iostream>

#include "Object/art_object_inst_ref.h"
#include "Object/art_object_expr_const.h"	// for const_index_list
#include "util/what.h"
#include "util/packed_array.tcc"	// for packed_array_generic<>::resize()
#include "util/persistent_object_manager.tcc"

//=============================================================================
namespace ART {
namespace entity {
#include "util/using_ostream.h"
using util::write_value;
using util::read_value;
using util::persistent_traits;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.  
 */
INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
INSTANCE_REFERENCE_CLASS::instance_reference() :
		parent_type(), inst_collection_ref() {
	// no assert
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
INSTANCE_REFERENCE_CLASS::instance_reference(
		const instance_collection_ptr_type pi) :
		parent_type(pi->current_collection_state()),
		inst_collection_ref(pi) {
	NEVER_NULL(inst_collection_ref);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
INSTANCE_REFERENCE_CLASS::~instance_reference() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
never_ptr<const instance_collection_base>
INSTANCE_REFERENCE_CLASS::get_inst_base(void) const {
	return inst_collection_ref;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
ostream&
INSTANCE_REFERENCE_CLASS::what(ostream& o) const {
	return o << util::what<this_type>::name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unrolls the reference into aliases.  
	\param c the context of unrolling.
	\param a the destination collection in which to return
		resolved instance aliases.  
	\return true on error, else false.
 */
INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
bad_bool
INSTANCE_REFERENCE_CLASS::unroll_references(unroll_context& c, 
		alias_collection_type& a) const {
	// possibly factor this part out into simple_instance_reference?
if (this->inst_collection_ref->get_dimensions()) {
	const_index_list cil;
	if (this->array_indices) {
		cil = this->array_indices->unroll_resolve(c);
		if (cil.empty()) {
			cerr << "ERROR: Failed to resolve indices at "
				"unroll-time!" << endl;
			return bad_bool(true);
		}
	}
	// else empty, implicitly refer to whole collection if it is dense
	// we have resolve constant indices
	const const_index_list
		full_indices(this->inst_collection_ref->resolve_indices(cil));
	if (full_indices.empty()) {
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
	if (this->inst_collection_ref->unroll_aliases(lower, upper, a)) {
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
	if (this->inst_collection_ref->unroll_aliases(bogus, bogus, a)) {
		cerr << "ERROR: unrolling aliases." << endl;
		return bad_bool(true);
	}
	return bad_bool(false);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
excl_ptr<aliases_connection_base>
INSTANCE_REFERENCE_CLASS::make_aliases_connection_private(void) const {
	return excl_ptr<aliases_connection_base>(new alias_connection_type);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
void
INSTANCE_REFERENCE_CLASS::collect_transient_info_base(
		persistent_object_manager& m) const {
	parent_type::collect_transient_info_base(m);
	inst_collection_ref->collect_transient_info(m);
	// instantiation_state has no pointers
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Visits children nodes and register pointers to object manager
	for serialization.  
	\param m the persistent object manager.
 */
INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
void
INSTANCE_REFERENCE_CLASS::collect_transient_info(
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
	member_INSTANCE_REFERENCE_CLASS::write_object.
 */
INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
void
INSTANCE_REFERENCE_CLASS::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	m.write_pointer(o, inst_collection_ref);
	parent_type::write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Writes the instance reference to output stream, translating
	pointers to indices as it goes along.  
	Note: the instantiation base must be written before the
		state information, for reconstruction purposes.  
	\param m the persistent object manager.  
 */
INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
void
INSTANCE_REFERENCE_CLASS::write_object(
		const persistent_object_manager& m, ostream& f) const {
	this->write_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Common functionality for this->load_object and
	member_INSTANCE_REFERENCE_CLASS::load_object.
 */
INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
void
INSTANCE_REFERENCE_CLASS::load_object_base(
		const persistent_object_manager& m, istream& i) {
	m.read_pointer(i, inst_collection_ref);
	NEVER_NULL(inst_collection_ref);
	m.load_object_once(const_cast<instance_collection_generic_type*>(
		&*inst_collection_ref));
	parent_type::load_object_base(m, i);
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
INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
void
INSTANCE_REFERENCE_CLASS::load_object(const persistent_object_manager& m, 
		istream& f) {
	this->load_object_base(m, f);
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_INST_REF_TCC__

