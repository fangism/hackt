/**
	\file "Object/inst/preal_value_collection.cc"
	Method definitions for parameter instance collection classes.
	This file was "Object/art_object_instance_preal.cc"
		in a previous life.  
 	$Id: preal_value_collection.cc,v 1.3.32.2 2006/09/01 05:17:40 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_PREAL_VALUE_COLLECTION_CC__
#define	__HAC_OBJECT_INST_PREAL_VALUE_COLLECTION_CC__

//=============================================================================
// start of static initializations
#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include <exception>
#include <iostream>
#include <algorithm>

#include "Object/type/param_type_reference.h"
#include "Object/inst/preal_value_collection.h"
#include "Object/ref/simple_meta_value_reference.h"
#include "Object/ref/simple_nonmeta_value_reference.h"
#include "Object/expr/preal_const.h"
#include "Object/persistent_type_hash.h"
#include "Object/unroll/param_instantiation_statement.h"
#include "Object/traits/preal_traits.h"
#include "Object/inst/value_collection.tcc"
#if USE_INSTANCE_PLACEHOLDERS
#include "Object/inst/value_placeholders.tcc"
#endif

//=============================================================================
// specializations in other namespace (local to this file)
// ok to specialize here, ONLY IF nothing else references it externally

namespace util {
using HAC::entity::preal_instance;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Write out preal_instance binary after compressing bits into char.
	TODO: Consider replacing with value_writer functor.  
 */
template <>
void
write_value(ostream& o, const preal_instance& b) {
	write_value(o, b.value);
	char c;		// sign doesn't matter
	c = b.valid;
	c <<= 1;
	c |= b.instantiated;
	write_value(o, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reads in preal_instance binary, decompressing char to bits.
	Yeah, I know, this could be more efficient.  
	TODO: Consider replacing with value_reader functor.  
 */
template <>
void
read_value(istream& i, preal_instance& b) {
	read_value(i, b.value);
	char c;
	read_value(i, c);
	// read off bitmask
	b.instantiated = c & 1;
	b.valid = c & 2;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

SPECIALIZE_UTIL_WHAT(HAC::entity::preal_scalar, "preal_scalar")
SPECIALIZE_UTIL_WHAT(HAC::entity::preal_array_1D, "preal_array<1>")
SPECIALIZE_UTIL_WHAT(HAC::entity::preal_array_2D, "preal_array<2>")
SPECIALIZE_UTIL_WHAT(HAC::entity::preal_array_3D, "preal_array<3>")
SPECIALIZE_UTIL_WHAT(HAC::entity::preal_array_4D, "preal_array<4>")

template <>
struct persistent_traits<HAC::entity::preal_instance_collection> {
	static const persistent::hash_key	type_key;
};

#if USE_INSTANCE_PLACEHOLDERS
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::preal_value_placeholder, 
	PREAL_INSTANCE_PLACEHOLDER_TYPE_KEY, 0)
#endif
const persistent::hash_key
persistent_traits<HAC::entity::preal_instance_collection>::type_key(
	PREAL_INSTANCE_COLLECTION_TYPE_KEY);

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::preal_scalar, PREAL_INSTANCE_COLLECTION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::preal_array_1D, PREAL_INSTANCE_COLLECTION_TYPE_KEY, 1)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::preal_array_2D, PREAL_INSTANCE_COLLECTION_TYPE_KEY, 2)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::preal_array_3D, PREAL_INSTANCE_COLLECTION_TYPE_KEY, 3)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::preal_array_4D, PREAL_INSTANCE_COLLECTION_TYPE_KEY, 4)

}	// end namespace util

//=============================================================================
namespace HAC {
namespace entity {
#include "util/using_ostream.h"
USING_UTIL_COMPOSE
using util::dereference;
using std::mem_fun_ref;
using util::write_value;
using util::read_value;
using util::indent;
using util::auto_indent;
using util::persistent_traits;

//=============================================================================
// struct preal_instance method definitions
// not really methods...

/**
	Note: when both are uninstantiated, consider them equal.  
 */
bool
operator == (const preal_instance& p, const preal_instance& q) {
//	INVARIANT(p.instantiated && q.instantiated);
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
operator << (ostream& o, const preal_instance& p) {
	INVARIANT(p.instantiated);
	if (p.valid) {
		return o << p.value;
	} else	return o << "?";
}

//=============================================================================
// class preal_instance_collection method definitions

#if USE_INSTANCE_PLACEHOLDERS
template class value_placeholder<preal_tag>;
#endif
template class value_collection<preal_tag>;
template class value_array<preal_tag,0>;
template class value_array<preal_tag,1>;
template class value_array<preal_tag,2>;
template class value_array<preal_tag,3>;
template class value_array<preal_tag,4>;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

DEFAULT_STATIC_TRACE_END

#endif	// __HAC_OBJECT_INST_PREAL_VALUE_COLLECTION_CC__

