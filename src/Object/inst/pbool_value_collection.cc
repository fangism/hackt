/**
	\file "Object/inst/pbool_value_collection.cc"
	Method definitions for parameter instance collection classes.
	This file was "Object/art_object_instance_pbool.cc"
		in a previous life.  
 	$Id: pbool_value_collection.cc,v 1.10 2011/04/02 01:46:02 fang Exp $
 */

//=============================================================================
// start of static initializations
#include "util/static_trace.hh"
DEFAULT_STATIC_TRACE_BEGIN

#include <exception>
#include <iostream>
#include <algorithm>

#include "Object/type/param_type_reference.hh"
#include "Object/inst/pbool_value_collection.hh"
#include "Object/ref/simple_meta_value_reference.hh"
#include "Object/ref/simple_nonmeta_value_reference.hh"
#include "Object/expr/pbool_const.hh"
#include "Object/expr/pint_expr.hh"
#include "Object/expr/preal_expr.hh"
#include "Object/expr/pstring_expr.hh"
#include "Object/unroll/param_instantiation_statement.hh"
#include "Object/persistent_type_hash.hh"
#include "Object/traits/pbool_traits.hh"
#include "Object/inst/value_collection.tcc"
#include "Object/inst/value_placeholder.tcc"

//=============================================================================
// specializations in other namespace (local to this file)
// ok to specialize here, ONLY IF nothing else references it externally

namespace util {
using HAC::entity::pbool_instance;

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

SPECIALIZE_UTIL_WHAT(HAC::entity::pbool_scalar, "pbool_scalar")
SPECIALIZE_UTIL_WHAT(HAC::entity::pbool_array_1D, "pbool_array<1>")
SPECIALIZE_UTIL_WHAT(HAC::entity::pbool_array_2D, "pbool_array<2>")
SPECIALIZE_UTIL_WHAT(HAC::entity::pbool_array_3D, "pbool_array<3>")
SPECIALIZE_UTIL_WHAT(HAC::entity::pbool_array_4D, "pbool_array<4>")

template <>
struct persistent_traits<HAC::entity::pbool_instance_collection> {
	static const persistent::hash_key	type_key;
};

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::pbool_value_placeholder, 
	PBOOL_VALUE_PLACEHOLDER_TYPE_KEY, 0)

}	// end namespace util

//=============================================================================
namespace HAC {
namespace entity {
#include "util/using_ostream.hh"
USING_UTIL_COMPOSE
using util::dereference;
using std::mem_fun_ref;
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

template class value_placeholder<pbool_tag>;
template class value_collection<pbool_tag>;
template class value_array<pbool_tag,0>;
template class value_array<pbool_tag,1>;
template class value_array<pbool_tag,2>;
template class value_array<pbool_tag,3>;
template class value_array<pbool_tag,4>;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

DEFAULT_STATIC_TRACE_END

