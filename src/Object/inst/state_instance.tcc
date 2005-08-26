/**
	\file "Object/inst/state_instance.tcc"
	Class implementation for instance state.  
	$Id: state_instance.tcc,v 1.2.4.5 2005/08/26 00:49:21 fang Exp $
 */

#ifndef	__OBJECT_INST_STATE_INSTANCE_TCC__
#define	__OBJECT_INST_STATE_INSTANCE_TCC__

#include <iostream>
#include "Object/inst/state_instance.h"
#include "util/stacktrace.h"

namespace ART {
namespace entity {
#include "util/using_ostream.h"

//=============================================================================
// class state_instance method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STATE_INSTANCE_TEMPLATE_SIGNATURE
STATE_INSTANCE_CLASS::state_instance() :
		back_ref(NULL) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STATE_INSTANCE_TEMPLATE_SIGNATURE
STATE_INSTANCE_CLASS::state_instance(const alias_info_type& a) :
		back_ref(&a) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STATE_INSTANCE_TEMPLATE_SIGNATURE
STATE_INSTANCE_CLASS::~state_instance() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For now, just prints the canonical back-link.  
	Also print relaxed actuals if applicable!
	No need to check type, a strict type will never have relaxed actuals, 
		as guaranteed by prior type-checking.  
 */
STATE_INSTANCE_TEMPLATE_SIGNATURE
ostream&
STATE_INSTANCE_CLASS::dump(ostream& o) const {
	NEVER_NULL(back_ref);
	back_ref->dump_alias(o);
	back_ref->dump_actuals(o);
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

