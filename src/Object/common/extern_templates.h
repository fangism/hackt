/**
	\file "Object/common/extern_templates.h"
	Declarations to suppress the automatic instantiation of certain
	template classes.  
	Useful for common template classes that are shared all over the place, 
	explicitly or implicitly.  
	This file used to be "Object/common/extern_templates.h"
		in its previous life.  
	$Id: extern_templates.h,v 1.4 2006/01/22 18:19:19 fang Exp $
 */

#ifndef	__HAC_OBJECT_COMMON_EXTERN_TEMPLATES_H__
#define	__HAC_OBJECT_COMMON_EXTERN_TEMPLATES_H__

// never include this in the .cc file
#ifndef	__HAC_OBJECT_COMMON_EXTERN_TEMPLATES_CC__

#include "Object/expr/types.h"
#include "util/multikey_fwd.h"
#include "util/packed_array_fwd.h"

// two methods, fake specialization, or extern template

// namespace is just for convenience
namespace util {
using HAC::entity::pint_value_type;
using HAC::entity::pbool_value_type;

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
// #define	EXTERN_TEMPLATE_UTIL_PACKED_ARRAY

#endif

//=============================================================================
// in this section, use specialization to suppress instantiation
// highly discourage this technique, because depends on the 
// compiler and linker charateristics with name mangling.  

// for gcc, will result in use of incomplete specialized types

//=============================================================================

}	// end namespace util

#endif	// __HAC_OBJECT_COMMON_EXTERN_TEMPLATES_CC__
#endif	// __HAC_OBJECT_COMMON_EXTERN_TEMPLATES_H__

