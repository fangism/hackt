/**
	\file "Object/art_object_nonmeta_inst_ref.tcc"
	$Id: art_object_nonmeta_inst_ref.tcc,v 1.2 2005/06/19 01:58:46 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_NONMETA_INST_REF_TCC__
#define	__OBJECT_ART_OBJECT_NONMETA_INST_REF_TCC__

#include <iostream>
#include "Object/art_object_nonmeta_inst_ref.h"
#include "util/what.h"
#include "util/persistent_object_manager.tcc"

// might as well include this, will be needed
#include "Object/art_object_nonmeta_inst_ref.h"
#include "Object/art_object_nonmeta_inst_ref_subtypes.h"

namespace ART {
namespace entity {
using util::persistent_traits;

//=============================================================================
// class simple_nonmeta_instance_reference method definitions

/// private empty constructor
SIMPLE_NONMETA_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
SIMPLE_NONMETA_INSTANCE_REFERENCE_CLASS::simple_nonmeta_instance_reference() :
		common_base_type(), parent_type(), inst_collection_ref() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - _
SIMPLE_NONMETA_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
SIMPLE_NONMETA_INSTANCE_REFERENCE_CLASS::simple_nonmeta_instance_reference(
		const instance_collection_ptr_type i) :
		common_base_type(), parent_type(), inst_collection_ref(i) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - _
SIMPLE_NONMETA_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
SIMPLE_NONMETA_INSTANCE_REFERENCE_CLASS::~simple_nonmeta_instance_reference() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - _
SIMPLE_NONMETA_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
typename SIMPLE_NONMETA_INSTANCE_REFERENCE_CLASS::instance_collection_ptr_type
SIMPLE_NONMETA_INSTANCE_REFERENCE_CLASS::get_inst_base_subtype(void) const {
	return inst_collection_ref;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - _
SIMPLE_NONMETA_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
never_ptr<const instance_collection_base>
SIMPLE_NONMETA_INSTANCE_REFERENCE_CLASS::get_inst_base(void) const {
	return inst_collection_ref;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - _
SIMPLE_NONMETA_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
ostream&
SIMPLE_NONMETA_INSTANCE_REFERENCE_CLASS::what(ostream& o) const {
	return o << util::what<this_type>::name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - _
SIMPLE_NONMETA_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
void
SIMPLE_NONMETA_INSTANCE_REFERENCE_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	common_base_type::collect_transient_info_base(m);
	inst_collection_ref->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - _
SIMPLE_NONMETA_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
void
SIMPLE_NONMETA_INSTANCE_REFERENCE_CLASS::write_object(
		const persistent_object_manager& m, ostream& o) const {
	m.write_pointer(o, inst_collection_ref);
	common_base_type::write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - _
SIMPLE_NONMETA_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
void
SIMPLE_NONMETA_INSTANCE_REFERENCE_CLASS::load_object(
		const persistent_object_manager& m, istream& i) {
	m.read_pointer(i, inst_collection_ref);
	// need to load inst_collection_ref?
	common_base_type::load_object_base(m, i);
}

//=============================================================================
}	// end namespace entity
}	// end namepace ART

#endif	// __OBJECT_ART_OBJECT_NONMETA_INST_REF_TCC__

