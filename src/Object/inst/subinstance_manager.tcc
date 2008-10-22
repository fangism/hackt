/**
	\file "Object/inst/subinstance_manager.tcc"
	Template method definitions for subinstance_manager.  
	$Id: subinstance_manager.tcc,v 1.12 2008/10/22 22:16:58 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_SUBINSTANCE_MANAGER_TCC__
#define	__HAC_OBJECT_INST_SUBINSTANCE_MANAGER_TCC__

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
good_bool
subinstance_manager::__unroll_port_instances(
		const collection_interface<Tag>& inst, 
#if ENABLE_RELAXED_TEMPLATE_PARAMETERS
		const relaxed_actuals_type& a,
#endif
		const unroll_context& c) {
	typedef	collection_interface<Tag>	collection_type;
	typedef	instance_collection<Tag>	canonical_collection_type;
	typedef	typename canonical_collection_type::collection_type_manager_parent_type
						type_manager_type;
	typedef	typename canonical_collection_type::instance_collection_parameter_type
					instance_collection_parameter_type;
	typedef	typename canonical_collection_type::type_ref_ptr_type
						type_ref_ptr_type;
	typedef	typename canonical_collection_type::resolved_type_ref_type
						resolved_type_ref_type;
	typedef	typename type_ref_ptr_type::element_type
						type_ref_pointee_type;
	STACKTRACE_VERBOSE;
#if ENABLE_RELAXED_TEMPLATE_PARAMETERS
	if (!this->empty()) {
		cerr << "Error: ports have already been instantiated!" << endl;
		return good_bool(false);
	}
#else
	INVARIANT(this->empty());
#endif
#if !ENABLE_RELAXED_TEMPLATE_PARAMETERS
	const
#endif
	resolved_type_ref_type
		resolved_super_type(inst.get_canonical_collection()
			.get_resolved_canonical_type());
	if (!resolved_super_type) {
		cerr << "Error resolving type during "
			"subinstance_manager::unroll_port_instances: <type>"
				<< endl;
		return good_bool(false);
	}
#if ENABLE_RELAXED_TEMPLATE_PARAMETERS
	if (a) {
		resolved_super_type.combine_relaxed_actuals(a);
	}
#endif
#if 0
	unresolved_super_type->dump(cerr << "unresolved type: ") << endl;
	resolved_super_type->dump(cerr << "resolved type:   ") << endl;
#endif
	// NOTE: this resolved type may be incomplete (relaxed)
#if ENABLE_STACKTRACE
	resolved_super_type.dump(STACKTRACE_INDENT << "super type: ") << endl;
#endif
	return resolved_super_type.unroll_port_instances(c, *this);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_SUBINSTANCE_MANAGER_TCC__

