/**
	\file "Object/type/canonical_type.tcc"
	Implementation of canonical_type template class.  
	$Id: canonical_type.tcc,v 1.1.2.2 2005/08/11 03:40:55 fang Exp $
 */

#ifndef	__OBJECT_TYPE_CANONICAL_TYPE_TCC__
#define	__OBJECT_TYPE_CANONICAL_TYPE_TCC__

#include "Object/type/canonical_type.h"
#include <iostream>
#include "util/persistent_object_manager.tcc"
#include "common/TODO.h"

namespace ART {
namespace entity {
#include "util/using_ostream.h"
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
/**
	Registers the complete type as a used type in the
	definition's footprint map.  
	Q: should it unroll 'instantiate' that definition?
	A: IMHO, yes, but it is not truly required.  
	A: could use a compiler flag to conditionally postpone... oooh.
 */
CANONICAL_TYPE_TEMPLATE_SIGNATURE
void
CANONICAL_TYPE_CLASS::register_definition_footprint(void) const {
#if 1
	FINISH_ME(Fang);
#else
	NEVER_NULL(canonical_definition_ptr);
	canonical_definition_ptr->register_type(param_list_ptr);
#endif
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

