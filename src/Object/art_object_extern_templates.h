/**
	\file "Object/art_object_extern_templates.h"
	Declarations to suppress the automatic instantiation of certain
	template classes.  
	Useful for common template classes that are shared all over the place, 
	explicitly or implicitly.  
	$Id: art_object_extern_templates.h,v 1.7 2005/06/21 21:26:34 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_EXTERN_TEMPLATES_H__
#define	__OBJECT_ART_OBJECT_EXTERN_TEMPLATES_H__

// never include this in the .cc file
#ifndef	__OBJECT_ART_OBJECT_EXTERN_TEMPLATES_CC__

#include "Object/art_object_expr_types.h"
#include "util/multikey_fwd.h"
#include "util/packed_array_fwd.h"

// two methods, fake specialization, or extern template

// namespace is just for convenience
namespace util {
using ART::entity::pint_value_type;
using ART::entity::pbool_value_type;

// NOTE: -pedantic forbids extern template, not ISO C++ :(
// see alternative in #else case below
//=============================================================================
#if 0
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
class packed_array_generic<pint_value_type, pint_value_type>;
extern template
class packed_array_generic<pint_value_type, pbool_value_type>;

extern template
class multikey_generator_generic<pint_value_type>;

#else

// Preferred method of template instantiation control, since -pedantic
// disables the GNU "extern template" extension:

// predefine these macros to suppress
// NB: the inclusion of this file MUST precede the (attempted)
// inclusion of the affected .tcc files for this to be effective.  
#define	EXTERN_TEMPLATE_UTIL_MULTIKEY
#define	EXTERN_TEMPLATE_UTIL_PACKED_ARRAY

#endif

//=============================================================================
// in this section, use specialization to suppress instantiation
// highly discourage this technique, because depends on the 
// compiler and linker charateristics with name mangling.  

// for gcc, will result in use of incomplete specialized types

//=============================================================================

}	// end namespace util

#endif	// __OBJECT_ART_OBJECT_EXTERN_TEMPLATES_CC__
#endif	// __OBJECT_ART_OBJECT_EXTERN_TEMPLATES_H__

