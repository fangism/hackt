/**
	\file "Object/inst/subinstance_manager.tcc"
	Template method definitions for subinstance_manager.  
	$Id: subinstance_manager.tcc,v 1.3.10.3 2005/08/15 19:58:28 fang Exp $
 */

#ifndef	__OBJECT_INST_SUBINTANCE_MANAGER_TCC__
#define	__OBJECT_INST_SUBINTANCE_MANAGER_TCC__

#include <iostream>
#include "Object/inst/subinstance_manager.h"
#include "Object/inst/instance_collection.h"
#include "Object/devel_switches.h"		// temporary
#include "util/stacktrace.h"

namespace ART {
namespace entity {
#include "util/using_ostream.h"

//=============================================================================
// class subinstance_manager method definitions

/**
	Expand this instance's ports into this subinstance_manager.  
	NOTE: this class is granted friendship from instance_collection<>.
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
	typedef	typename type_ref_ptr_type::element_type
						type_ref_pointee_type;
	STACKTRACE_VERBOSE;
	INVARIANT(this->empty());
	const type_ref_ptr_type unresolved_super_type(inst.get_type());
	NEVER_NULL(unresolved_super_type);
	const type_ref_ptr_type
		resolved_super_type(unresolved_super_type->unroll_resolve(c));
	if (!resolved_super_type) {
		unresolved_super_type->dump(
			cerr << "Error resolving type during "
				"subinstance_manager::unroll_port_instances: ")
				<< endl;
		THROW_EXIT;
	}
#if 0
	unresolved_super_type->dump(cerr << "unresolved type: ") << endl;
	resolved_super_type->dump(cerr << "resolved type:   ") << endl;
#endif
	INVARIANT(resolved_super_type->is_resolved());
	const instance_collection_parameter_type
		canonical_super_type(resolved_super_type->
			make_canonical_type());
	canonical_super_type.unroll_port_instances(c, *this);
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_SUBINTANCE_MANAGER_TCC__

