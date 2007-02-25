/**
	\file "Object/traits/type_tag_enum.h"
	Global enumerations pertaining to meta types.  
	$Id: type_tag_enum.h,v 1.6.18.1 2007/02/25 19:54:41 fang Exp $
 */

#ifndef	__HAC_OBJECT_TRAITS_TYPE_TAG_ENUM_H__
#define	__HAC_OBJECT_TRAITS_TYPE_TAG_ENUM_H__

#include "Object/traits/classification_tags_fwd.h"

namespace HAC {
namespace entity {
//=============================================================================

typedef	enum {
	META_TYPE_NONE = 0,
	META_TYPE_PROCESS = 1,
	META_TYPE_CHANNEL = 2, 
	META_TYPE_STRUCT = 3,
	META_TYPE_BOOL = 4,
	META_TYPE_INT = 5,
	META_TYPE_ENUM = 6,
	META_TYPES_PHYSICAL = 7,	// use as a MAX size
	// parameter types must follow physical types
	META_TYPE_PBOOL = 7,
	META_TYPE_PINT = 8,
	META_TYPE_PREAL = 9,
	META_TYPES_ALL = 10		// use as a MAX size
}	meta_type_tag_enum;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The following specializations also guarantee that the enumerations
	are unique, otherwise compiler would complain.  
	Each specialization uses an enum from meta_type_tag_enum.
 */
template <unsigned char>
struct meta_type_map;

template <>
struct meta_type_map<META_TYPE_PROCESS> { typedef	process_tag	type; };

template <>
struct meta_type_map<META_TYPE_CHANNEL> { typedef	channel_tag	type; };

template <>
struct meta_type_map<META_TYPE_STRUCT> { typedef	datastruct_tag	type; };

template <>
struct meta_type_map<META_TYPE_BOOL> { typedef	bool_tag	type; };

template <>
struct meta_type_map<META_TYPE_INT> { typedef	int_tag		type; };

template <>
struct meta_type_map<META_TYPE_ENUM> { typedef	enum_tag	type; };

template <>
struct meta_type_map<META_TYPE_PBOOL> { typedef	pbool_tag	type; };

template <>
struct meta_type_map<META_TYPE_PINT> { typedef	pint_tag	type; };

template <>
struct meta_type_map<META_TYPE_PREAL> { typedef	preal_tag	type; };

//-----------------------------------------------------------------------------
/**
	Oh, no!  Heaven forbid, I have to use an enum and switch-case
	in my code.  
	Use this only when absolutely unavoidable.  
 */
typedef	enum {
	PARENT_TYPE_NONE = META_TYPE_NONE,
	PARENT_TYPE_PROCESS = META_TYPE_PROCESS,
	PARENT_TYPE_CHANNEL = META_TYPE_CHANNEL, 
	PARENT_TYPE_STRUCT = META_TYPE_STRUCT 
}	parent_tag_enum;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_TRAITS_TYPE_TAG_ENUM_H__

