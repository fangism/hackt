/**
	\file "Object/common/extern_templates.cc"
	Repository of explicit template instantiations needed 
	by the object-related library.  
	This file was "Object/common/extern_templates.cc"
		in its previous life.  
	$Id: extern_templates.cc,v 1.15 2011/04/02 01:45:54 fang Exp $
 */

#include <list>
#include <iostream>
#include "Object/expr/types.hh"
#include "util/multikey.tcc"
#include "util/packed_array.tcc"
#include "util/IO_utils.tcc"

// since namespace doesn't affect instantiation, this is convenient
namespace util {
using HAC::entity::pint_value_type;
using HAC::entity::pbool_value_type;
using HAC::entity::preal_value_type;
using HAC::entity::pstring_value_type;
using std::list;
using std::istream;
using std::ostream;

// template class multikey_base<pint_value_type>;
template class multikey_generic<pint_value_type>;
INSTANTIATE_MULTIKEY(1, pint_value_type)
INSTANTIATE_MULTIKEY(2, pint_value_type)
INSTANTIATE_MULTIKEY(3, pint_value_type)
INSTANTIATE_MULTIKEY(4, pint_value_type)
INSTANTIATE_MULTIKEY_LIST_CTOR(2, pint_value_type, list)
INSTANTIATE_MULTIKEY_LIST_CTOR(3, pint_value_type, list)
INSTANTIATE_MULTIKEY_LIST_CTOR(4, pint_value_type, list)

template class multikey_generator_generic<pint_value_type>;
INSTANTIATE_MULTIKEY_GENERATOR(1, pint_value_type)
INSTANTIATE_MULTIKEY_GENERATOR(2, pint_value_type)
INSTANTIATE_MULTIKEY_GENERATOR(3, pint_value_type)
INSTANTIATE_MULTIKEY_GENERATOR(4, pint_value_type)

INSTANTIATE_MULTIKEY_GENERIC_MULTIKEY_CTOR(1, pint_value_type)
INSTANTIATE_MULTIKEY_GENERIC_MULTIKEY_CTOR(2, pint_value_type)
INSTANTIATE_MULTIKEY_GENERIC_MULTIKEY_CTOR(3, pint_value_type)
INSTANTIATE_MULTIKEY_GENERIC_MULTIKEY_CTOR(4, pint_value_type)

template
ostream& operator << (ostream&, const multikey_generic<pint_value_type>&);

template multikey<2, pint_value_type>::multikey(
	const multikey_generator_generic<pint_value_type>&, pint_value_type);
template multikey<3, pint_value_type>::multikey(
	const multikey_generator_generic<pint_value_type>&, pint_value_type);
template multikey<4, pint_value_type>::multikey(
	const multikey_generator_generic<pint_value_type>&, pint_value_type);

// for passing around and collecting subindices
// see "Object/ref/meta_instance_reference_subtypes.h": subindex_collection_type
template class packed_array_generic<pint_value_type, size_t>;

// for passing around dense arrays/collections of values
template class packed_array_generic<pint_value_type, pint_value_type>;
template class packed_array_generic<pint_value_type, pbool_value_type>;
template class packed_array_generic<pint_value_type, preal_value_type>;
template class packed_array_generic<pint_value_type, pstring_value_type>;

// some of these are needed for higher optimizations that
// drop inline instantiations...
template struct value_reader<multikey_generic<pint_value_type> >;
template struct value_writer<multikey_generic<pint_value_type> >;
template struct value_reader<size_t>;
template struct value_writer<size_t>;

template void read_value(istream&, size_t&);
template void write_value(ostream&, const size_t&);
template void read_value(istream&, double&);
template void write_value(ostream&, const double&);

}	// end namespace util

