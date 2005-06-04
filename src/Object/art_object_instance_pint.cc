/**
	\file "Object/art_object_instance_pint.cc"
	Method definitions for parameter instance collection classes.
 	$Id: art_object_instance_pint.cc,v 1.23.4.1 2005/06/04 04:48:01 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INSTANCE_PINT_CC__
#define	__OBJECT_ART_OBJECT_INSTANCE_PINT_CC__

#define	DEBUG_LIST_VECTOR_POOL				0
#define	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE		0
#define ENABLE_STACKTRACE				0

//=============================================================================
// start of static initializations
#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include <exception>
#include <iostream>
#include <algorithm>

#include "Object/art_object_type_ref.h"
#include "Object/art_object_instance_param.h"
#include "Object/art_object_inst_ref.h"
#include "Object/art_object_inst_stmt.h"
#include "Object/art_object_expr_param_ref.h"	// for pint/pbool_meta_instance_reference
#include "Object/art_built_ins.h"
#include "Object/art_object_type_hash.h"

// experimental: suppressing automatic template instantiation
#include "Object/art_object_extern_templates.h"

#include "Object/art_object_value_reference.h"
#include "Object/art_object_value_collection.tcc"
#include "Object/art_object_classification_details.h"

#include "util/memory/count_ptr.tcc"

//=============================================================================
namespace util {
	SPECIALIZE_UTIL_WHAT(ART::entity::pint_scalar, "pint_scalar")
	SPECIALIZE_UTIL_WHAT(ART::entity::pint_array_1D, "pint_array<1>")
	SPECIALIZE_UTIL_WHAT(ART::entity::pint_array_2D, "pint_array<2>")
	SPECIALIZE_UTIL_WHAT(ART::entity::pint_array_3D, "pint_array<3>")
	SPECIALIZE_UTIL_WHAT(ART::entity::pint_array_4D, "pint_array<4>")

template <>
struct persistent_traits<ART::entity::pint_instance_collection> {
	static const persistent::hash_key	type_key;
};

const persistent::hash_key
persistent_traits<ART::entity::pint_instance_collection>::type_key(
	PINT_INSTANCE_COLLECTION_TYPE_KEY);

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::pint_scalar, PINT_INSTANCE_COLLECTION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::pint_array_1D, PINT_INSTANCE_COLLECTION_TYPE_KEY, 1)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::pint_array_2D, PINT_INSTANCE_COLLECTION_TYPE_KEY, 2)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::pint_array_3D, PINT_INSTANCE_COLLECTION_TYPE_KEY, 3)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::pint_array_4D, PINT_INSTANCE_COLLECTION_TYPE_KEY, 4)

namespace memory {
	LIST_VECTOR_POOL_LAZY_DESTRUCTION(ART::entity::pint_scalar)
}	// end namespace memory
}	// end namespace util

//=============================================================================
namespace ART {
namespace entity {

#if DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE && ENABLE_STACKTRACE
REQUIRES_STACKTRACE_STATIC_INIT
#endif

//=============================================================================
// struct pint_instance method definitions
// not really methods...

bool
operator == (const pint_instance& p, const pint_instance& q) {
	INVARIANT(p.instantiated && q.instantiated);
	if (p.valid && q.valid) {
		return p.value == q.value;
	} else return (p.valid == q.valid); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
operator << (ostream& o, const pint_instance& p) {
	INVARIANT(p.instantiated);
	if (p.valid) {
		return o << p.value;
	} else	return o << "?";
}

//=============================================================================
// class pint_instance_collection method definitions

template class value_collection<pint_tag>;
template class value_array<pint_tag,0>;
template class value_array<pint_tag,1>;
template class value_array<pint_tag,2>;
template class value_array<pint_tag,3>;
template class value_array<pint_tag,4>;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

DEFAULT_STATIC_TRACE_END

#endif	// __OBJECT_ART_OBJECT_INSTANCE_PINT_CC__

