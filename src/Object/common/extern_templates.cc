/**
	\file "Object/common/extern_templates.cc"
	Repository of explicit template instantiations needed 
	by the object-related library.  
	This file was "Object/common/extern_templates.cc"
		in its previous life.  
	$Id: extern_templates.cc,v 1.2.8.2 2005/08/31 06:19:25 fang Exp $
 */

#ifndef	__OBJECT_COMMON_EXTERN_TEMPLATES_CC__
#define	__OBJECT_COMMON_EXTERN_TEMPLATES_CC__

#include "util/STL/list_fwd.h"
#include "Object/expr/types.h"
#include "util/multikey.tcc"
#include "util/packed_array.tcc"

// since namespace doesn't affect instantiation, this is convenient
namespace util {
using ART::entity::pint_value_type;
using ART::entity::pbool_value_type;
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

INSTANTIATE_MULTIKEY_GENERIC_MULTIKEY_CTOR(1, pint_value_type)
INSTANTIATE_MULTIKEY_GENERIC_MULTIKEY_CTOR(2, pint_value_type)
INSTANTIATE_MULTIKEY_GENERIC_MULTIKEY_CTOR(3, pint_value_type)
INSTANTIATE_MULTIKEY_GENERIC_MULTIKEY_CTOR(4, pint_value_type)

template class packed_array_generic<pint_value_type, pint_value_type>;
template class packed_array_generic<pint_value_type, pbool_value_type>;

}	// end namespace util

#endif	// __OBJECT_COMMON_EXTERN_TEMPLATES_CC__

