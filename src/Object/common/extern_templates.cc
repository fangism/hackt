/**
	\file "Object/common/extern_templates.cc"
	Repository of explicit template instantiations needed 
	by the object-related library.  
	This file was "Object/common/extern_templates.cc"
		in its previous life.  
	$Id: extern_templates.cc,v 1.9 2006/02/10 21:50:35 fang Exp $
 */

#ifndef	__HAC_OBJECT_COMMON_EXTERN_TEMPLATES_CC__
#define	__HAC_OBJECT_COMMON_EXTERN_TEMPLATES_CC__

#include <list>
#include "Object/expr/types.h"
#include "util/multikey.tcc"
#include "util/packed_array.tcc"

// since namespace doesn't affect instantiation, this is convenient
namespace util {
using HAC::entity::pint_value_type;
using HAC::entity::pbool_value_type;
using HAC::entity::preal_value_type;
using std::list;

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

#if 0
INSTANTIATE_MULTIKEY_GENERIC_MULTIKEY_CTOR(1, pint_value_type)
INSTANTIATE_MULTIKEY_GENERIC_MULTIKEY_CTOR(2, pint_value_type)
INSTANTIATE_MULTIKEY_GENERIC_MULTIKEY_CTOR(3, pint_value_type)
INSTANTIATE_MULTIKEY_GENERIC_MULTIKEY_CTOR(4, pint_value_type)
#endif

template multikey<2, pint_value_type>::multikey(
	const multikey_generator_generic<pint_value_type>&, pint_value_type);
template multikey<3, pint_value_type>::multikey(
	const multikey_generator_generic<pint_value_type>&, pint_value_type);
template multikey<4, pint_value_type>::multikey(
	const multikey_generator_generic<pint_value_type>&, pint_value_type);

template class packed_array_generic<pint_value_type, pint_value_type>;
template class packed_array_generic<pint_value_type, pbool_value_type>;
template class packed_array_generic<pint_value_type, preal_value_type>;

}	// end namespace util

#endif	// __HAC_OBJECT_COMMON_EXTERN_TEMPLATES_CC__

