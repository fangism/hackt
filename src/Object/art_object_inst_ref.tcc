/**
	\file "art_object_inst_ref.cc"
	Method definitions for the instance_reference family of objects.
 	$Id: art_object_inst_ref.tcc,v 1.1.4.1 2005/02/20 20:59:19 fang Exp $
 */

#ifndef	__ART_OBJECT_INST_REF_TCC__
#define	__ART_OBJECT_INST_REF_TCC__

#include <iostream>

#include "art_object_inst_ref.h"
#include "what.h"
#include "persistent_object_manager.tcc"

//=============================================================================
namespace ART {
namespace entity {
#include "using_ostream.h"
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
	Just allocates with bogus contents, first pass of reconstruction.  
 */
INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
persistent*
INSTANCE_REFERENCE_CLASS::construct_empty(const int) {
	return new this_type;
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
	m.load_object_once(const_cast<instance_collection_type*>(
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

#endif	// __ART_OBJECT_INST_REF_TCC__

