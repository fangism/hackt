/**
	\file "Object/inst/pint_value_collection.cc"
	Method definitions for parameter instance collection classes.
	This file used to be "Object/art_object_instance_pint.cc"
		in a previous life.  
 	$Id: pint_value_collection.cc,v 1.8.28.3 2006/09/02 00:46:05 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_PINT_VALUE_COLLECTION_CC__
#define	__HAC_OBJECT_INST_PINT_VALUE_COLLECTION_CC__

#define ENABLE_STACKTRACE				0
#define	DEBUG_LIST_VECTOR_POOL				0
#define	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE		0

//=============================================================================
// start of static initializations
#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include <exception>
#include <iostream>
#include <algorithm>

#include "Object/type/param_type_reference.h"
#include "Object/inst/pint_value_collection.h"
#include "Object/ref/simple_meta_value_reference.h"
#include "Object/ref/simple_nonmeta_value_reference.h"
#include "Object/expr/pint_const.h"
#include "Object/persistent_type_hash.h"
#include "Object/unroll/param_instantiation_statement.h"
#include "Object/inst/value_collection.tcc"
#if USE_INSTANCE_PLACEHOLDERS
#include "Object/inst/value_placeholder.tcc"
#endif
#include "Object/traits/pint_traits.h"
#include "Object/traits/int_traits.h"

//=============================================================================
namespace util {
	SPECIALIZE_UTIL_WHAT(HAC::entity::pint_scalar, "pint_scalar")
	SPECIALIZE_UTIL_WHAT(HAC::entity::pint_array_1D, "pint_array<1>")
	SPECIALIZE_UTIL_WHAT(HAC::entity::pint_array_2D, "pint_array<2>")
	SPECIALIZE_UTIL_WHAT(HAC::entity::pint_array_3D, "pint_array<3>")
	SPECIALIZE_UTIL_WHAT(HAC::entity::pint_array_4D, "pint_array<4>")

template <>
struct persistent_traits<HAC::entity::pint_instance_collection> {
	static const persistent::hash_key	type_key;
};

#if USE_INSTANCE_PLACEHOLDERS
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::pint_value_placeholder, 
	PINT_VALUE_PLACEHOLDER_TYPE_KEY, 0)
#endif
const persistent::hash_key
persistent_traits<HAC::entity::pint_instance_collection>::type_key(
	PINT_INSTANCE_COLLECTION_TYPE_KEY);

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::pint_scalar, PINT_INSTANCE_COLLECTION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::pint_array_1D, PINT_INSTANCE_COLLECTION_TYPE_KEY, 1)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::pint_array_2D, PINT_INSTANCE_COLLECTION_TYPE_KEY, 2)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::pint_array_3D, PINT_INSTANCE_COLLECTION_TYPE_KEY, 3)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::pint_array_4D, PINT_INSTANCE_COLLECTION_TYPE_KEY, 4)

// TODO: specialize value_reader and value_writer
#if 0
namespace memory {
	LIST_VECTOR_POOL_LAZY_DESTRUCTION(HAC::entity::pint_scalar)
}	// end namespace memory
#endif
}	// end namespace util

//=============================================================================
namespace HAC {
namespace entity {

#if DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE && ENABLE_STACKTRACE
REQUIRES_STACKTRACE_STATIC_INIT
#endif

//=============================================================================
// struct pint_instance method definitions
// not really methods...

/**
	Compares a pair of pint instances.  
	If neither party is instantiated, consider them equal.
 */
bool
operator == (const pint_instance& p, const pint_instance& q) {
	// INVARIANT(p.instantiated && q.instantiated);
	// multikey_assoc will need to compare against a default
	// uninstantiated value, so we can't assert.  
if (p.instantiated && q.instantiated) {
	if (p.valid && q.valid) {
		return p.value == q.value;
	} else return (p.valid == q.valid); 
} else {
	return p.instantiated == q.instantiated;
}
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

#if USE_INSTANCE_PLACEHOLDERS
template class value_placeholder<pint_tag>;
#endif
template class value_collection<pint_tag>;
template class value_array<pint_tag,0>;
template class value_array<pint_tag,1>;
template class value_array<pint_tag,2>;
template class value_array<pint_tag,3>;
template class value_array<pint_tag,4>;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

DEFAULT_STATIC_TRACE_END

#endif	// __HAC_OBJECT_INST_PINT_VALUE_COLLECTION_CC__

