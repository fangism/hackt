/**
	\file "Object/art_object_extern_templates.cc"
	Repository of explici template instantiations needed 
	by the object-related library.  
	$Id: art_object_extern_templates.cc,v 1.5.14.1 2005/06/24 19:02:55 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_EXTERN_TEMPLATES_CC__
#define	__OBJECT_ART_OBJECT_EXTERN_TEMPLATES_CC__

#if 0
// don't include own header if using specialization to suppress templates
#include "Object/art_object_extern_templates.h"
#else
#include "Object/art_object_expr_types.h"
#endif

#include "util/multikey.tcc"
#include "util/packed_array.tcc"

// since namespace doesn't affect instantiation, this is convenient
namespace util {
using ART::entity::pint_value_type;
using ART::entity::pbool_value_type;

// template class multikey_base<pint_value_type>;
template class multikey_generic<pint_value_type>;
template class multikey<1, pint_value_type>;
template class multikey<2, pint_value_type>;
template class multikey<3, pint_value_type>;
template class multikey<4, pint_value_type>;
template class packed_array_generic<pint_value_type, pint_value_type>;
template class packed_array_generic<pint_value_type, pbool_value_type>;
template class multikey_generator_generic<pint_value_type>;

}	// end namespace util

#endif	// __OBJECT_ART_OBJECT_EXTERN_TEMPLATES_CC__

