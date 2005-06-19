/**
	\file "Object/art_built_ins.h"
	Header for built-in features of the art language, 
	including primitive definitions and types.  
	$Id: art_built_ins.h,v 1.10 2005/06/19 01:58:32 fang Exp $
 */

#ifndef	__OBJECT_ART_BUILT_INS_H__
#define	__OBJECT_ART_BUILT_INS_H__

#include "util/memory/pointer_classes_fwd.h"

/***
	NOTE:
	Consider exposing only never_ptrs to the built-in definitions, 
	and keeping the static objects private instead, to avoid
	confusion and the inconvenience of "taking the address of" &.  
***/

namespace ART {
namespace entity {
class data_type_reference;
// class param_type_reference;
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

#if 0
// replaced with class_traits<Tag>::built_in_type_ptr below
extern	const count_ptr<const param_type_reference>	pbool_type_ptr;
extern	const count_ptr<const param_type_reference>	pint_type_ptr;
#endif

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

extern	const count_ptr<const data_type_reference>	bool_type_ptr;
extern	const count_ptr<const data_type_reference>	int32_type_ptr;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	//	__OBJECT_ART_BUILT_INS_H__

