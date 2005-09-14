/**
	\file "Object/inst/alias_empty.tcc"
	$Id: alias_empty.tcc,v 1.2.2.2 2005/09/14 13:23:14 fang Exp $
 */

#ifndef	__OBJECT_INST_ALIAS_EMPTY_TCC__
#define	__OBJECT_INST_ALIAS_EMPTY_TCC__

#include <iostream>
#include "Object/inst/alias_empty.h"
#include "Object/common/dump_flags.h"

namespace ART {
namespace entity {
#include "util/using_ostream.h"
//=============================================================================
// class instance_alias_info_empty method definitions

#if 0
// need not be defined, never used or called.  
/**
	\param _alias the instance alias to dump type info.  
	\param o the output stream.
	\param f the footprint is just used for consistency checking.  
		However, no meta types without relaxed actuals have
		footprints, so it is just ignored (should be null).
 */
template <class AliasType>
ostream&
instance_alias_info_empty::dump_complete_type(const AliasType& _alias, 
		ostream& o, const footprint* const f) {
	const complete_type_type
		_type(_alias.container->get_canonical_type());
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param _alias the instance alias whose type is to be unrolled
		and created.  
 */
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
		_alias.dump_hierarchical_name(cerr << "Failed to instantiate ", 
			dump_flags::default_value) << endl;
		return good_bool(false);
	}
	else if (!container_type::collection_type_manager_parent_type
			::create_definition_footprint(_type).good) {
		// have error message already
		_alias.dump_hierarchical_name(cerr << "Instantiated by: ", 
			dump_flags::default_value) << endl;
		return good_bool(false);
	}
	return good_bool(true);
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_ALIAS_EMPTY_TCC__

