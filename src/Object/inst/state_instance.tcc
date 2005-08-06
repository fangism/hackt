/**
	\file "Object/inst/state_instance.tcc"
	Class implementation for instance state.  
	$Id: state_instance.tcc,v 1.1.2.1 2005/08/06 15:42:29 fang Exp $
 */

#ifndef	__OBJECT_INST_STATE_INSTANCE_TCC__
#define	__OBJECT_INST_STATE_INSTANCE_TCC__

#include <iostream>
#include "Object/inst/state_instance.h"
#include "Object/inst/instance_pool.tcc"
// #include "util/persistent_object_manager.h"

namespace ART {
namespace entity {
//=============================================================================
// class state_instance method definitions

/**
	State instance pool, currently with hard-coded pool chunk size.  
	This is the static initialization.  
 */
STATE_INSTANCE_TEMPLATE_SIGNATURE
instance_pool<STATE_INSTANCE_CLASS >
STATE_INSTANCE_CLASS::pool(1024);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STATE_INSTANCE_TEMPLATE_SIGNATURE
STATE_INSTANCE_CLASS::state_instance() :
		state_instance_base(), back_ref(NULL) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STATE_INSTANCE_TEMPLATE_SIGNATURE
STATE_INSTANCE_CLASS::state_instance(const alias_info_type& a) :
		state_instance_base(), back_ref(&a) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STATE_INSTANCE_TEMPLATE_SIGNATURE
STATE_INSTANCE_CLASS::~state_instance() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Don't actually register the back-reference because it is
	not directly managed by the persistent object manager.  
 */
STATE_INSTANCE_TEMPLATE_SIGNATURE
void
STATE_INSTANCE_CLASS::collect_transient_info_base(
		persistent_object_manager&) const {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Saves the canonical alias back-reference.
 */
STATE_INSTANCE_TEMPLATE_SIGNATURE
void
STATE_INSTANCE_CLASS::write_object_base(const persistent_object_manager& m,
		ostream& o) const {
	NEVER_NULL(back_ref);
	this->back_ref->write_next_connection(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Restores the canonical alias back-reference.
 */
STATE_INSTANCE_TEMPLATE_SIGNATURE
void
STATE_INSTANCE_CLASS::load_object_base(const persistent_object_manager& m,
		istream& i) {
	this->back_ref = never_ptr<const alias_info_type>(
		&alias_info_type::load_alias_reference(m, i));
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_STATE_INSTANCE_TCC__

