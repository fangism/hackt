/**
	\file "art_built_ins.h"
	Header for built-in features of the art language, 
	including primitive definitions and types.  
	$Id: art_built_ins.h,v 1.8 2005/01/13 05:28:27 fang Exp $
 */

#ifndef	__ART_BUILT_INS_H__
#define	__ART_BUILT_INS_H__

// don't need all forward declarations, just a few
// #include "art_object_fwd.h"
#include "memory/pointer_classes_fwd.h"

/***
	NOTE:
	Consider exposing only never_ptrs to the built-in definitions, 
	and keeping the static objects private instead, to avoid
	confusion and the inconvenience of "taking the address of" &.  
***/

namespace ART {
namespace entity {
using util::memory::count_ptr;

// forward declarations
class name_space;
class built_in_param_def;
class param_type_reference;
class built_in_datatype_def;

//=============================================================================
/**
	Pseudo-built-in namespace, external to the global namespace.
 */
extern	name_space built_in_namespace;

/**
	Built-in parameter type definitions and references.
	Really, these defs aren't needed, only the type references.  
**/
extern	const built_in_param_def	pbool_def;
extern	const built_in_param_def	pint_def;

extern	const count_ptr<const param_type_reference>	pbool_type_ptr;
extern	const count_ptr<const param_type_reference>	pint_type_ptr;

/**
	Built-in data types.  
**/
extern	const built_in_datatype_def	bool_def;
extern	const built_in_datatype_def	int_def;

#if 0
// not actually used
extern	const data_type_reference	bool_type;
// no int_type because is templated
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	//	__ART_BUILT_INS_H__

