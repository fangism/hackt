/**
	\file "Object/art_object_member_inst_ref.tcc"
	Method definitions for the meta_instance_reference family of objects.
 	$Id: art_object_member_inst_ref.tcc,v 1.7.2.1 2005/06/08 19:13:30 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_MEMBER_INST_REF_TCC__
#define	__OBJECT_ART_OBJECT_MEMBER_INST_REF_TCC__

#include <iostream>
#include "util/what.h"
#include "util/persistent_object_manager.h"
#include "Object/art_object_member_inst_ref.h"
#include "util/memory/count_ptr.tcc"

//=============================================================================
namespace ART {
namespace entity {
#include "util/using_ostream.h"
using util::write_value;
using util::read_value;
using util::persistent_traits;

//=============================================================================
// class member_meta_instance_reference method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
MEMBER_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
MEMBER_INSTANCE_REFERENCE_CLASS::member_meta_instance_reference() :
		parent_type(), base_inst_ref() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MEMBER_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
MEMBER_INSTANCE_REFERENCE_CLASS::member_meta_instance_reference(
		const base_inst_ptr_type& b, 
		const instance_collection_ptr_type m) :
		parent_type(m), base_inst_ref(b) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MEMBER_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
MEMBER_INSTANCE_REFERENCE_CLASS::~member_meta_instance_reference() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MEMBER_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
ostream&
MEMBER_INSTANCE_REFERENCE_CLASS::what(ostream& o) const {
	return o << util::what<this_type>::name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true on error.  
 */
MEMBER_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
bad_bool
MEMBER_INSTANCE_REFERENCE_CLASS::unroll_references(
		unroll_context& c, alias_collection_type& a) const {
	cerr << "FANG: write member_instanace_reference<>::unroll_references()!" << endl;
	return bad_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Visits children nodes and register pointers to object manager
	for serialization.  
	\param m the persistent object manager.
 */
MEMBER_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
void
MEMBER_INSTANCE_REFERENCE_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	parent_type::collect_transient_info_base(m);
	NEVER_NULL(base_inst_ref);
	base_inst_ref->collect_transient_info(m);
	// instantiation_state has no pointers
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Writes the instance reference to output stream, translating
	pointers to indices as it goes along.  
	Note: the instantiation base must be written before the
		state information, for reconstruction purposes.  
	\param m the persistent object manager.  
 */
MEMBER_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
void
MEMBER_INSTANCE_REFERENCE_CLASS::write_object(
		const persistent_object_manager& m, ostream& f) const {
	m.write_pointer(f, base_inst_ref);
	parent_type::write_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Loads the instance reference from an input stream, translating
	indices to pointers.  
	Note: the instantiation base must be loaded before the
		state information, because the instantiation state
		depends on the instantiation base being complete.  
	\param m the persistent object manager.  
 */
MEMBER_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
void
MEMBER_INSTANCE_REFERENCE_CLASS::load_object(
		const persistent_object_manager& m, istream& f) {
	m.read_pointer(f, base_inst_ref);
	NEVER_NULL(base_inst_ref);
	m.load_object_once(const_cast<base_inst_type*>(&*base_inst_ref));
	parent_type::load_object_base(m, f);
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_MEMBER_INST_REF_TCC__

