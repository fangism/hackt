// "art_built_ins.h"
// Header for built-in features of the art language.  

#ifndef	__ART_BUILT_INS_H__
#define	__ART_BUILT_INS_H__

#include "art_object_definition.h"
#include "art_object_type_ref.h"
	// needed for static constructor of complete types

/*
	Consider exposing only never_ptrs to the built-in definitions, 
	and keeping the static objects private instead, to avoid
	confusion and the inconvenience of "taking the address of" &.  
*/

namespace ART {
namespace entity {
//=============================================================================
/**
	Pseudo-built-in namespace, superior to the global namespace.
**/
extern	name_space built_in_namespace;

/**
	Built-in parameter type definitions and references.
	Should we allocate with static const excl_const_ptr?
	Really, the defs aren't needed, only the type references.  
**/
extern	const built_in_param_def	pbool_def;
extern	const built_in_param_def	pint_def;

extern	const count_const_ptr<param_type_reference>	pbool_type_ptr;
extern	const count_const_ptr<param_type_reference>	pint_type_ptr;

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

