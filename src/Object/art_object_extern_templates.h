/**
	\file "art_object_extern_templates.h"
	Declarations to suppress the automatic instantiation of certain
	template classes.  
	Useful for common template classes that are shared all over the place, 
	explicitly or implicitly.  
	$Id: art_object_extern_templates.h,v 1.2.2.1 2005/02/09 04:14:08 fang Exp $
 */

#ifndef	__ART_OBJECT_EXTERN_TEMPLATES_H__
#define	__ART_OBJECT_EXTERN_TEMPLATES_H__

// never include this in the .cc file
#ifndef	__ART_OBJECT_EXTERN_TEMPLATES_CC__

#include "art_object_fwd.h"
#include "multikey_fwd.h"
#include "packed_array_fwd.h"

// two methods, fake specialization, or extern template

// namespace is just for convenience
namespace util {
using ART::entity::pint_value_type;

//=============================================================================
#if 1
// in the section, use "extern" prefixing to suppress instantiation
// extern template
// class multikey_base<pint_value_type>;

extern template
class multikey_generic<pint_value_type>;

extern template
class multikey<1, pint_value_type>;
extern template
class multikey<2, pint_value_type>;
extern template
class multikey<3, pint_value_type>;
extern template
class multikey<4, pint_value_type>;

extern template
class packed_array_generic<pint_value_type>;

extern template
class multikey_generator_generic<pint_value_type>;


//=============================================================================
#else
// in this section, use specialization to suppress instantiation
// highly discourage this technique, because depends on the 
// compiler and linker charateristics with name mangling.  

// for gcc, will result in use of incomplete specialized types

template <>
class multikey_base<pint_value_type>;

template <>
class packed_array_generic<pint_value_type>;

template <>
class multikey_generator_generic<pint_value_type>;

//=============================================================================
#endif

}	// end namespace util

#endif	// __ART_OBJECT_EXTERN_TEMPLATES_CC__
#endif	// __ART_OBJECT_EXTERN_TEMPLATES_H__

