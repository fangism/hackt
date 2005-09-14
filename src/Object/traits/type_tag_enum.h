/**
	\file "Object/traits/type_tag_enum.h"
	Global enumerations pertaining to meta types.  
	$Id: type_tag_enum.h,v 1.1.2.1 2005/09/14 00:17:11 fang Exp $
 */

#ifndef	__OBJECT_TRAITS_TYPE_TAG_ENUM_H__
#define	__OBJECT_TRAITS_TYPE_TAG_ENUM_H__

namespace ART {
namespace entity {

/**
	Oh, no!  Heaven forbid, I have to use an enum and switch-case
	in my code.  
	Use this only when absolutely unavoidable.  
 */
typedef	enum {
	NONE = 0,
	PROCESS = 1,
	CHANNEL = 2, 
	STRUCT = 3
}	parent_tag_enum;

}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_TRAITS_TYPE_TAG_ENUM_H__

