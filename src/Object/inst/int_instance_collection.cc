/**
	\file "Object/inst/int_instance_collection.cc"
	Method definitions for integer data type instance classes.
	Hint: copied from the bool counterpart, and text substituted.  
	This file came from "Object/art_object_instance_int.cc"
		in a previous life.  
	$Id: int_instance_collection.cc,v 1.2.4.2 2005/08/06 01:32:20 fang Exp $
 */

#ifndef	__OBJECT_INST_INT_INSTANCE_COLLECTION_CC__
#define	__OBJECT_INST_INT_INSTANCE_COLLECTION_CC__

#define	ENABLE_STACKTRACE		0
#define	STACKTRACE_DESTRUCTORS		0 && ENABLE_STACKTRACE
#define	STACKTRACE_PERSISTENTS		0 && ENABLE_STACKTRACE

//=============================================================================
#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include <exception>
#include <iostream>
#include <algorithm>

#include "Object/inst/int_instance_collection.h"
#include "Object/inst/alias_empty.h"
#include "Object/ref/simple_datatype_meta_instance_reference_base.h"
#include "Object/ref/member_meta_instance_reference.h"
#include "Object/ref/simple_nonmeta_value_reference.h"
#include "Object/expr/int_expr.h"
#include "Object/expr/pint_const.h"
#include "Object/expr/const_param_expr_list.h"
#include "Object/def/built_in_datatype_def.h"
#include "Object/type/data_type_reference.h"
#include "Object/persistent_type_hash.h"
#include "Object/inst/instance_collection.tcc"
#include "Object/inst/int_collection_type_manager.tcc"

//=============================================================================
// module-local specializations

namespace util {
	SPECIALIZE_UTIL_WHAT(ART::entity::int_instance_collection,
		"int_instance_collection")
	SPECIALIZE_UTIL_WHAT(ART::entity::int_scalar, "int_scalar")
	SPECIALIZE_UTIL_WHAT(ART::entity::int_array_1D, "int_array_1D")
	SPECIALIZE_UTIL_WHAT(ART::entity::int_array_2D, "int_array_2D")
	SPECIALIZE_UTIL_WHAT(ART::entity::int_array_3D, "int_array_3D")
	SPECIALIZE_UTIL_WHAT(ART::entity::int_array_4D, "int_array_4D")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::int_scalar, DINT_INSTANCE_COLLECTION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::int_array_1D, DINT_INSTANCE_COLLECTION_TYPE_KEY, 1)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::int_array_2D, DINT_INSTANCE_COLLECTION_TYPE_KEY, 2)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::int_array_3D, DINT_INSTANCE_COLLECTION_TYPE_KEY, 3)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::int_array_4D, DINT_INSTANCE_COLLECTION_TYPE_KEY, 4)
}	// end namespace util


namespace ART {
namespace entity {
//=============================================================================
// class int_instance method definitions

int_instance::int_instance() : back_ref() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int_instance::int_instance(const alias_info_type& i) : back_ref(&i) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int_instance::~int_instance() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
ostream&
int_instance::what(ostream& o) const {
	return o << "int-state";
	if (this->back_ref) {
		this->back_ref->dump_hierarchical_name(o << " = ");
	}
	return o;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_instance::collect_transient_info_base(persistent_object_manager& m) const {
	// register me!
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_instance::write_object_base(const persistent_object_manager& m, 
		ostream& o) const {
#if USE_INSTANCE_INDEX
	NEVER_NULL(back_ref);
	back_ref->write_next_connection(m, o);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_instance::load_object_base(const persistent_object_manager& m, 
		istream& i) {
#if USE_INSTANCE_INDEX
	back_ref = never_ptr<const alias_info_type>(
		&alias_info_type::load_alias_reference(m, i));
#endif
}

//=============================================================================
// typedef int_instance_alias_base function definitions

ostream&
operator << (ostream& o, const int_instance_alias_base& i) {
	return o << "int-alias @ " << &i;
}

//=============================================================================
// explicit template instantiations

template class instance_alias_info<int_tag>;
template class instance_collection<int_tag>;
template class instance_array<int_tag, 0>;
template class instance_array<int_tag, 1>;
template class instance_array<int_tag, 2>;
template class instance_array<int_tag, 3>;
template class instance_array<int_tag, 4>;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

DEFAULT_STATIC_TRACE_END

#endif	// __OBJECT_INST_INT_INSTANCE_COLLECTION_CC__

