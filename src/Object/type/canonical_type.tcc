/**
	\file "Object/type/canonical_type.tcc"
	Implementation of canonical_type template class.  
	$Id: canonical_type.tcc,v 1.1.2.1 2005/08/10 20:30:56 fang Exp $
 */

#ifndef	__OBJECT_TYPE_CANONICAL_TYPE_TCC__
#define	__OBJECT_TYPE_CANONICAL_TYPE_TCC__

#include "Object/type/canonical_type.h"
#include <iostream>
#include "util/persistent_object_manager.tcc"

namespace ART {
namespace entity {
//=============================================================================
// class canonical_type method definitions

CANONICAL_TYPE_TEMPLATE_SIGNATURE
CANONICAL_TYPE_CLASS::canonical_type() :
		canonical_definition_ptr(NULL), param_list_ptr(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CANONICAL_TYPE_TEMPLATE_SIGNATURE
CANONICAL_TYPE_CLASS::canonical_type(const canonical_definition_ptr_type d) :
		canonical_definition_ptr(d), param_list_ptr(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CANONICAL_TYPE_TEMPLATE_SIGNATURE
CANONICAL_TYPE_CLASS::canonical_type(const canonical_definition_ptr_type d,
		const param_list_ptr_type& p) :
		canonical_definition_ptr(d), param_list_ptr(p) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CANONICAL_TYPE_TEMPLATE_SIGNATURE
void
CANONICAL_TYPE_CLASS::collect_transient_info_base(
		persistent_object_manager& m) const {
	if (canonical_definition_ptr)
		canonical_definition_ptr->collect_transient_info(m);
	if (param_list_ptr)
		param_list_ptr->collect_transient_info(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CANONICAL_TYPE_TEMPLATE_SIGNATURE
void
CANONICAL_TYPE_CLASS::write_object_base(const persistent_object_manager& m, 
		ostream& o) const {
	m.write_pointer(o, canonical_definition_ptr);
	m.write_pointer(o, param_list_ptr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CANONICAL_TYPE_TEMPLATE_SIGNATURE
void
CANONICAL_TYPE_CLASS::load_object_base(const persistent_object_manager& m, 
		istream& i) {
	m.read_pointer(i, canonical_definition_ptr);
	m.read_pointer(i, param_list_ptr);
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_TYPE_CANONICAL_TYPE_TCC__

