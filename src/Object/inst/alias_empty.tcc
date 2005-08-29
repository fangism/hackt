/**
	\file "Object/inst/alias_empty.tcc"
	$Id: alias_empty.tcc,v 1.1.2.1 2005/08/29 21:32:04 fang Exp $
 */

#ifndef	__OBJECT_INST_ALIAS_EMPTY_TCC__
#define	__OBJECT_INST_ALIAS_EMPTY_TCC__

#include "Object/inst/alias_empty.h"

namespace ART {
namespace entity {
//=============================================================================
// class instance_alias_info_empty method definitions

template <class AliasType>
good_bool
instance_alias_info_empty::create_dependent_types(const AliasType& _alias) {
	typedef	typename AliasType::container_type	container_type;
	typedef typename container_type::instance_collection_parameter_type
				complete_type_type;
	const complete_type_type
		_type(_alias.container->get_canonical_type());
	if (!_type) {
		// already have error message
		_alias.dump_hierarchical_name(cerr << "Failed to instantiate ")
			<< endl;
		return good_bool(false);
	}
	else if (!container_type::collection_type_manager_parent_type
			::create_definition_footprint(_type).good) {
		// have error message already
		_alias.dump_hierarchical_name(cerr << "Instantiated by: ")
			<< endl;
		return good_bool(false);
	}
	return good_bool(true);
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_ALIAS_EMPTY_TCC__

