/**
	\file "Object/inst/pint_value_collection.cc"
	Method definitions for parameter instance collection classes.
	This file used to be "Object/art_object_instance_pint.cc"
		in a previous life.  
 	$Id: pint_value_collection.cc,v 1.12 2011/04/02 01:46:02 fang Exp $
 */

#define ENABLE_STACKTRACE				0
#define	DEBUG_LIST_VECTOR_POOL				0
#define	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE		0

//=============================================================================
// start of static initializations
#include "util/static_trace.hh"
DEFAULT_STATIC_TRACE_BEGIN

#include <exception>
#include <iostream>
#include <algorithm>

#include "Object/type/param_type_reference.hh"
#include "Object/inst/pint_value_collection.hh"
#include "Object/ref/simple_meta_value_reference.hh"
#include "Object/ref/simple_nonmeta_value_reference.hh"
#include "Object/expr/pint_const.hh"
#include "Object/expr/pbool_expr.hh"
#include "Object/expr/preal_expr.hh"
#include "Object/expr/pstring_expr.hh"
#include "Object/persistent_type_hash.hh"
#include "Object/unroll/param_instantiation_statement.hh"
#include "Object/inst/value_collection.tcc"
#include "Object/inst/value_placeholder.tcc"
#include "Object/traits/pint_traits.hh"
#include "Object/traits/int_traits.hh"

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

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::pint_value_placeholder, 
	PINT_VALUE_PLACEHOLDER_TYPE_KEY, 0)

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

template class value_placeholder<pint_tag>;
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

