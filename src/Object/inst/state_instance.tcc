/**
	\file "Object/inst/state_instance.tcc"
	Class implementation for instance state.  
	$Id: state_instance.tcc,v 1.8.24.2 2010/01/15 04:13:10 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_STATE_INSTANCE_TCC__
#define	__HAC_OBJECT_INST_STATE_INSTANCE_TCC__

#include <iostream>
#include "Object/inst/state_instance.h"
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
#include "Object/lang/cflat_visitor.h"
#include "Object/global_channel_entry.h"
#endif
#include "util/stacktrace.h"

namespace HAC {
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
	back_ref->dump_hierarchical_name(o);
	back_ref->dump_actuals(o);
	back_ref->dump_attributes(o);	// new: print non-default attributes
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
STATE_INSTANCE_TEMPLATE_SIGNATURE
void
STATE_INSTANCE_CLASS::accept(PRS::cflat_visitor& v) const {
	STACKTRACE_VERBOSE;
	v.visit(*this);
}
#endif

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
STATE_INSTANCE_CLASS::write_object_base(const collection_pool_bundle_type& m, 
		ostream& o) const {
	NEVER_NULL(back_ref);
	back_ref->write_next_connection(m, o);
#if 0 && MEMORY_MAPPED_GLOBAL_ALLOCATION
	// can this information be reconstructed?
	entry_type::write_oject_base(m, o);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Restores the canonical alias back-reference.
 */
STATE_INSTANCE_TEMPLATE_SIGNATURE
void
STATE_INSTANCE_CLASS::load_object_base(const collection_pool_bundle_type& m, 
		istream& i) {
	this->back_ref = never_ptr<const alias_info_type>(
		&alias_info_type::load_alias_reference(m, i));
#if 0 && MEMORY_MAPPED_GLOBAL_ALLOCATION
	// can this information be reconstructed?
	entry_type::load_oject_base(m, i);
#endif
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_STATE_INSTANCE_TCC__

