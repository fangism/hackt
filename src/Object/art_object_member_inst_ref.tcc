/**
	\file "art_object_member_inst_ref.tcc"
	Method definitions for the instance_reference family of objects.
 	$Id: art_object_member_inst_ref.tcc,v 1.1.2.1 2005/02/20 06:36:32 fang Exp $
 */

#ifndef	__ART_OBJECT_MEMBER_INST_REF_TCC__
#define	__ART_OBJECT_MEMBER_INST_REF_TCC__

#include <iostream>

#include "art_object_inst_ref_base.h"

#if 0
#include "STL/list.tcc"
#include "multidimensional_sparse_set.tcc"

#include "art_object_type_ref_base.h"
#include "art_object_instance.h"
#include "art_object_instance_param.h"
#include "art_object_namespace.h"
#include "art_object_inst_ref.h"

#if 1
// this really needs to be moved to a separate file...
#include "art_object_inst_ref_data.h"
	// for datatype_member_instance_reference::make_aliases_connection_private

	// to complete types, ugh...
#include "art_object_instance_bool.h"
#include "art_object_instance_int.h"
#include "art_object_instance_enum.h"
#include "art_object_instance_struct.h"
#endif

#include "art_object_inst_stmt_base.h"
#include "art_object_expr.h"		// for dynamic_range_list
#include "art_object_control.h"
#include "art_object_connect.h"		// for aliases_connection_base
#include "persistent_object_manager.tcc"
#include "art_built_ins.h"
#include "art_object_type_hash.h"
#endif

//=============================================================================
namespace ART {
namespace entity {
#include "using_ostream.h"
using util::write_value;
using util::read_value;
using util::persistent_traits;

//=============================================================================
// class member_instance_reference method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
MEMBER_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
MEMBER_INSTANCE_REFERENCE_CLASS::member_instance_reference() :
		parent_type(), base_inst_ref() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MEMBER_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
MEMBER_INSTANCE_REFERENCE_CLASS::member_instance_reference(
		const base_inst_ptr_type& b, 
		const never_ptr<const instance_collection_type> m) :
		parent_type(m), base_inst_ref(b) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MEMBER_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
MEMBER_INSTANCE_REFERENCE_CLASS::~member_instance_reference() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MEMBER_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
ostream&
MEMBER_INSTANCE_REFERENCE_CLASS::what(ostream& o) const {
	return o << "member-instance-ref";
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
	Just allocates with bogus contents, first pass of reconstruction.  
 */
MEMBER_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
persistent*
MEMBER_INSTANCE_REFERENCE_CLASS::construct_empty(const int) {
	return new this_type;
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

#endif	// __ART_OBJECT_MEMBER_INST_REF_TCC__

