/**
	\file "art_object_instance_pint.cc"
	Method definitions for parameter instance collection classes.
 	$Id: art_object_instance_pint.cc,v 1.17.8.3 2005/03/11 01:05:29 fang Exp $
 */

#ifndef	__ART_OBJECT_INSTANCE_PINT_CC__
#define	__ART_OBJECT_INSTANCE_PINT_CC__

#define	DEBUG_LIST_VECTOR_POOL				0
#define	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE		0
#define ENABLE_STACKTRACE				0

#include <exception>
#include <iostream>
#include <algorithm>

#include "art_object_type_ref.h"
#include "art_object_instance_param.h"
#include "art_object_inst_ref.h"
#include "art_object_inst_stmt.h"
#include "art_object_expr_param_ref.h"	// for pint/pbool_instance_reference
#include "art_built_ins.h"
#include "art_object_type_hash.h"

// experimental: suppressing automatic template instantiation
#include "art_object_extern_templates.h"

#include "art_object_value_reference.h"
#include "art_object_value_collection.tcc"
#include "art_object_classification_details.h"

#include "static_trace.h"

//=============================================================================
// start of static initializations
STATIC_TRACE_BEGIN("instance_pint")

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
#if 0
#include "using_ostream.h"
USING_UTIL_COMPOSE
using util::dereference;
using std::mem_fun_ref;
using util::indent;
using util::auto_indent;
USING_STACKTRACE
using util::write_value;
using util::read_value;
using util::persistent_traits;
#endif

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

STATIC_TRACE_END("instance_pint")

#endif	// __ART_OBJECT_INSTANCE_PINT_CC__

