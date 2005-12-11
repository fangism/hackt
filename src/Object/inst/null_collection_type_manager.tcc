/**
	\file "Object/inst/null_collection_type_manager.tcc"
	Template class for instance_collection's type manager.  
	$Id: null_collection_type_manager.tcc,v 1.2.34.1 2005/12/11 00:45:38 fang Exp $
 */

#ifndef	__OBJECT_INST_NULL_COLLECTION_TYPE_MANAGER_TCC__
#define	__OBJECT_INST_NULL_COLLECTION_TYPE_MANAGER_TCC__

#include "Object/inst/null_collection_type_manager.h"
#include <iostream>

namespace HAC {
namespace entity {
#include "util/using_ostream.h"

#define NULL_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE			\
template <class Tag>

#define	NULL_COLLECTION_TYPE_MANAGER_CLASS				\
null_collection_type_manager<Tag>

//=============================================================================
/**
	Pretty-print functor.  
 */
NULL_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
struct NULL_COLLECTION_TYPE_MANAGER_CLASS::dumper {
	ostream& os;

	dumper(ostream& o) : os(o) { }

	ostream&
	operator () (const instance_collection_generic_type& c) {
		return os << class_traits<Tag>::tag_name <<
			'^' << c.get_dimensions();
	}
};	// end struct dumper

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns  built in type pointer. 
 */
NULL_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
const typename NULL_COLLECTION_TYPE_MANAGER_CLASS::type_ref_ptr_type&
NULL_COLLECTION_TYPE_MANAGER_CLASS::get_type(void) const {
	return class_traits<Tag>::built_in_type_ptr;
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#undef NULL_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
#undef NULL_COLLECTION_TYPE_MANAGER_CLASS

#endif	// __OBJECT_INST_NULL_COLLECTION_TYPE_MANAGER_TCC__

