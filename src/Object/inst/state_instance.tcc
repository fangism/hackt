/**
	\file "Object/inst/state_instance.tcc"
	Class implementation for instance state.  
	$Id: state_instance.tcc,v 1.2.4.2 2005/08/16 20:32:15 fang Exp $
 */

#ifndef	__OBJECT_INST_STATE_INSTANCE_TCC__
#define	__OBJECT_INST_STATE_INSTANCE_TCC__

#include <iostream>
#include "Object/inst/state_instance.h"
#if !USE_MODULE_FOOTPRINT
#include "Object/inst/instance_pool.h"
#endif
#include "util/stacktrace.h"

namespace ART {
namespace entity {
#include "util/using_ostream.h"

//=============================================================================
// class state_instance method definitions

#if !USE_MODULE_FOOTPRINT
/**
	State instance pool, currently with hard-coded pool chunk size.  
	This is the static initialization.  
	TODO: use class_traits to give each pool different chunk size.  
 */
STATE_INSTANCE_TEMPLATE_SIGNATURE
typename STATE_INSTANCE_CLASS::pool_type
STATE_INSTANCE_CLASS::pool(class_traits<Tag>::instance_pool_chunk_size);
#endif

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
	For now, just prints the canonical back-link.  
 */
STATE_INSTANCE_TEMPLATE_SIGNATURE
ostream&
STATE_INSTANCE_CLASS::dump(ostream& o) const {
	NEVER_NULL(back_ref);
	back_ref->dump_alias(o);
	return o;
}

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
	back_ref->write_next_connection(m, o);
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

