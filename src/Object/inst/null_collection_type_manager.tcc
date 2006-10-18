/**
	\file "Object/inst/null_collection_type_manager.tcc"
	Template class for instance_collection's type manager.  
	$Id: null_collection_type_manager.tcc,v 1.7 2006/10/18 01:19:37 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_NULL_COLLECTION_TYPE_MANAGER_TCC__
#define	__HAC_OBJECT_INST_NULL_COLLECTION_TYPE_MANAGER_TCC__

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
		return os << traits_type::tag_name;
	}
};	// end struct dumper

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !USE_INSTANCE_PLACEHOLDERS
/**
	Returns  built in type pointer. 
 */
NULL_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
const typename NULL_COLLECTION_TYPE_MANAGER_CLASS::type_ref_ptr_type&
NULL_COLLECTION_TYPE_MANAGER_CLASS::get_type(void) const {
	return traits_type::built_in_type_ptr;
}
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#undef NULL_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
#undef NULL_COLLECTION_TYPE_MANAGER_CLASS

#endif	// __HAC_OBJECT_INST_NULL_COLLECTION_TYPE_MANAGER_TCC__

