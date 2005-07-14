/**
	\file "Object/inst/subinstance_manager.cc"
	Class implementation of the subinstance_manager.
	$Id: subinstance_manager.cc,v 1.1.2.4 2005/07/14 03:15:40 fang Exp $
 */

#include <iostream>
#include "Object/inst/subinstance_manager.h"
#include "Object/art_object_instance.h"
#include "Object/art_object_type_ref_base.h"
#include "util/persistent_object_manager.tcc"
#include "util/memory/count_ptr.tcc"
#include "util/reserve.h"

namespace ART {
namespace entity {
#include "util/using_ostream.h"
//=============================================================================
// class subinstance_manager method definitions

subinstance_manager::subinstance_manager() : subinstance_array() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: not your everyday copy-constructor, needs to 
	re-establish local connections.  
 */
subinstance_manager::subinstance_manager(const this_type& s) :
	subinstance_array() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
subinstance_manager::~subinstance_manager() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Recursively creates hierarchy of public ports.  
	TODO: template this for general physical types
		to avoid up/down-casting.  
	\param parent_instance the parent instance, from which we acquire type information.  
 */
void
subinstance_manager::unroll_port_instances(
		const physical_instance_collection& parent_instance) {
	INVARIANT(subinstance_array.empty());	// must be the first time
	// need strict type information of the parent instance
	// from the parent's type's definition, create the number of ports.
	// util::reserve(subinstance_array, N);
	// instantiate each port formal/actual entry
#if 0
	const count_ptr<const fundamental_type_reference>
		super_instance_type(parent_instance.get_type_ref());
	NEVER_NULL(super_instance_type);
	const count_ptr<const fundamental_type_reference>
		resolved_super_instance_type(
			super_instance_type->unroll_resolve());
	if (!super_instance_type) {
		super_instance_type->dump(
			cerr << "Error resolving port type: ") << endl;
		THROW_EXIT;
	}
#if 0
	// make sure it is resolved! at least the strict parameters
	// TODO: this won't work for built-in channel types
	//	later, split this out into a different policy or virtualize.
	const never_ptr<const definition_base>
		super_instance_def(super_instance_type->get_base_def());
	// may need to create an unroll_context
#else
#if 0
	super_instance_type->dump(cerr << "type: ") << endl;
#endif
	INVARIANT(resolved_super_instance_type->is_resolved());
	INVARIANT(resolved_super_instance_type->is_canonical());
	resolved_super_instance_type->unroll_port_instances(*this);
#endif
#else

#endif
	// relink_super_instance_alias(...);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Re-links all sub-instances to a parent instance.  
	Needed to maintain the hierarchy.  
 */
void
subinstance_manager::relink_super_instance_alias(
		const substructure_alias& p) {
	iterator i(subinstance_array.begin());
	const iterator e(subinstance_array.end());
	for ( ; i!=e; i++) {
		NEVER_NULL(*i);
		(*i)->relink_super_instance(p);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
subinstance_manager::collect_transient_info_base(
		persistent_object_manager& m) const {
	m.collect_pointer_list(subinstance_array);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
subinstance_manager::write_object_base(const persistent_object_manager& m, 
		ostream& o) const {
	m.write_pointer_list(o, subinstance_array);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
subinstance_manager::load_object_base(const persistent_object_manager& m,
		istream& i) {
	m.read_pointer_list(i, subinstance_array);
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

