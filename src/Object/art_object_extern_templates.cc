/**
	\file "art_object_extern_templates.cc"
	Repository of explici template instantiations needed 
	by the object-related library.  
	$Id: art_object_extern_templates.cc,v 1.3.12.1 2005/03/11 01:16:18 fang Exp $
 */

#ifndef	__ART_OBJECT_EXTERN_TEMPLATES_CC__
#define	__ART_OBJECT_EXTERN_TEMPLATES_CC__

#if 0
// don't include own header if using specialization to suppress templates
#include "art_object_extern_templates.h"
#else
#include "art_object_fwd.h"
#endif

#include "multikey.tcc"
#include "packed_array.tcc"

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

#endif	// __ART_OBJECT_EXTERN_TEMPLATES_CC__

