/**
	\file "Object/inst/pbool_value_collection.cc"
	Method definitions for parameter instance collection classes.
	This file was "Object/art_object_instance_pbool.cc"
		in a previous life.  
 	$Id: pbool_value_collection.cc,v 1.1.2.1 2005/07/22 00:25:13 fang Exp $
 */

#ifndef	__OBJECT_INST_PBOOL_VALUE_COLLECTION_CC__
#define	__OBJECT_INST_PBOOL_VALUE_COLLECTION_CC__

//=============================================================================
// start of static initializations
#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include <exception>
#include <iostream>
#include <algorithm>

#include "Object/type/param_type_reference.h"
#include "Object/inst/pbool_value_collection.h"
#include "Object/art_object_inst_ref.h"
#include "Object/art_object_inst_stmt.h"
#include "Object/expr/pbool_const.h"
#include "Object/art_object_type_hash.h"
#include "Object/art_object_value_reference.h"
#include "Object/art_object_nonmeta_value_reference.h"
#include "Object/traits/pbool_traits.h"
#include "Object/inst/value_collection.tcc"

//=============================================================================
// specializations in other namespace (local to this file)
// ok to specialize here, ONLY IF nothing else references it externally

namespace util {
using ART::entity::pbool_instance;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Write out pbool_instance binary after compressing bits into char.
	Consider replacing with value_writer functor.  
 */
template <>
void
write_value(ostream& o, const pbool_instance& b) {
	char c;		// sign doesn't matter
	c = b.valid;
	c <<= 1;
	c |= b.instantiated;
	c <<= 1;
	c |= b.value;
	write_value(o, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reads in pbool_instance binary, decompressing char to bits.
	Yeah, I know, this could be more efficient.  
	Consider replacing with value_reader functor.  
 */
template <>
void
read_value(istream& i, pbool_instance& b) {
	char c;
	read_value(i, c);
	// read off bitmask
	b.value = c & 1;
	b.instantiated = c & 2;
	b.valid = c & 4;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

SPECIALIZE_UTIL_WHAT(ART::entity::pbool_scalar, "pbool_scalar")
SPECIALIZE_UTIL_WHAT(ART::entity::pbool_array_1D, "pbool_array<1>")
SPECIALIZE_UTIL_WHAT(ART::entity::pbool_array_2D, "pbool_array<2>")
SPECIALIZE_UTIL_WHAT(ART::entity::pbool_array_3D, "pbool_array<3>")
SPECIALIZE_UTIL_WHAT(ART::entity::pbool_array_4D, "pbool_array<4>")

template <>
struct persistent_traits<ART::entity::pbool_instance_collection> {
	static const persistent::hash_key	type_key;
};

const persistent::hash_key
persistent_traits<ART::entity::pbool_instance_collection>::type_key(
	PBOOL_INSTANCE_COLLECTION_TYPE_KEY);

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::pbool_scalar, PBOOL_INSTANCE_COLLECTION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::pbool_array_1D, PBOOL_INSTANCE_COLLECTION_TYPE_KEY, 1)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::pbool_array_2D, PBOOL_INSTANCE_COLLECTION_TYPE_KEY, 2)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::pbool_array_3D, PBOOL_INSTANCE_COLLECTION_TYPE_KEY, 3)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::pbool_array_4D, PBOOL_INSTANCE_COLLECTION_TYPE_KEY, 4)

}	// end namespace util

//=============================================================================
namespace ART {
namespace entity {
#include "util/using_ostream.h"
USING_UTIL_COMPOSE
using util::dereference;
using std::mem_fun_ref;
USING_STACKTRACE
using util::write_value;
using util::read_value;
using util::indent;
using util::auto_indent;
using util::persistent_traits;

//=============================================================================
// struct pbool_instance method definitions
// not really methods...

/**
	Note: when both are uninstantiated, consider them equal.  
 */
bool
operator == (const pbool_instance& p, const pbool_instance& q) {
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
operator << (ostream& o, const pbool_instance& p) {
	INVARIANT(p.instantiated);
	if (p.valid) {
		return o << p.value;
	} else	return o << "?";
}

//=============================================================================
// class pbool_instance_collection method definitions

template class value_collection<pbool_tag>;
template class value_array<pbool_tag,0>;
template class value_array<pbool_tag,1>;
template class value_array<pbool_tag,2>;
template class value_array<pbool_tag,3>;
template class value_array<pbool_tag,4>;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

DEFAULT_STATIC_TRACE_END

#endif	// __OBJECT_INST_PBOOL_VALUE_COLLECTION_CC__

