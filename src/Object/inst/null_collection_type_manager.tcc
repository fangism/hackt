/**
	\file "Object/inst/null_collection_type_manager.tcc"
	Template class for instance_collection's type manager.  
	$Id: null_collection_type_manager.tcc,v 1.9 2007/04/15 05:52:19 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_NULL_COLLECTION_TYPE_MANAGER_TCC__
#define	__HAC_OBJECT_INST_NULL_COLLECTION_TYPE_MANAGER_TCC__

#include "Object/inst/null_collection_type_manager.hh"
#include <iostream>

namespace HAC {
namespace entity {
#include "util/using_ostream.hh"

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
	operator () (const instance_collection_generic_type&) {
		return os << traits_type::tag_name;
	}
};	// end struct dumper

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#undef NULL_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
#undef NULL_COLLECTION_TYPE_MANAGER_CLASS

#endif	// __HAC_OBJECT_INST_NULL_COLLECTION_TYPE_MANAGER_TCC__

