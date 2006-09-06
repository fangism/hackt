/**
	\file "Object/inst/subinstance_manager.tcc"
	Template method definitions for subinstance_manager.  
	$Id: subinstance_manager.tcc,v 1.6.50.1 2006/09/06 04:19:49 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_SUBINTANCE_MANAGER_TCC__
#define	__HAC_OBJECT_INST_SUBINTANCE_MANAGER_TCC__

#include <iostream>
#include "Object/inst/subinstance_manager.h"
#include "Object/inst/instance_collection.h"
#include "util/stacktrace.h"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"

//=============================================================================
// class subinstance_manager method definitions

/**
	Expand this instance's ports into this subinstance_manager.  
	NOTE: this class is granted friendship from instance_collection<>.
	TODO: Once we don't need friendship, remove from instance_collection.
	TODO: can't this be greatly simplified after having 
		migrated to canonical_types?
 */
template <class Tag>
void
subinstance_manager::unroll_port_instances(
		const instance_collection<Tag>& inst, 
		const unroll_context& c) {
	typedef	instance_collection<Tag>	collection_type;
	typedef	typename collection_type::collection_type_manager_parent_type
						type_manager_type;
	typedef	typename collection_type::instance_collection_parameter_type
					instance_collection_parameter_type;
	typedef	typename collection_type::type_ref_ptr_type
						type_ref_ptr_type;
#if USE_RESOLVED_DATA_TYPES
	typedef	typename collection_type::resolved_type_ref_type
						resolved_type_ref_type;
#endif
	typedef	typename type_ref_ptr_type::element_type
						type_ref_pointee_type;
	STACKTRACE_VERBOSE;
	INVARIANT(this->empty());
#if USE_RESOLVED_DATA_TYPES
	const resolved_type_ref_type
		resolved_super_type(inst.get_resolved_canonical_type());
		// TODO: this needs to print error message on failure?
#else
	const type_ref_ptr_type unresolved_super_type(inst.get_type());
	NEVER_NULL(unresolved_super_type);
	const type_ref_ptr_type
		resolved_super_type(unresolved_super_type->unroll_resolve(c));
#endif
	if (!resolved_super_type) {
#if USE_RESOLVED_DATA_TYPES
		cerr << "Error resolving type during "
			"subinstance_manager::unroll_port_instances: <type>"
				<< endl;
#else
		unresolved_super_type->dump(
			cerr << "Error resolving type during "
				"subinstance_manager::unroll_port_instances: ")
				<< endl;
#endif
		THROW_EXIT;
	}
#if 0
	unresolved_super_type->dump(cerr << "unresolved type: ") << endl;
	resolved_super_type->dump(cerr << "resolved type:   ") << endl;
#endif
#if USE_RESOLVED_DATA_TYPES
	resolved_super_type.unroll_port_instances(c, *this);
#else
	INVARIANT(resolved_super_type->is_resolved());
	const instance_collection_parameter_type
		canonical_super_type(resolved_super_type->
			make_canonical_type());
	canonical_super_type.unroll_port_instances(c, *this);
#endif
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_SUBINTANCE_MANAGER_TCC__

