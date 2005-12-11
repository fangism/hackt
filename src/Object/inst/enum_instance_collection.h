/**
	\file "Object/inst/enum_instance_collection.h"
	Class declarations for built-in and user-defined data instances
	and instance collections.  
	This file was "Object/art_object_instance_enum.h"
		in a previous life.  
	$Id: enum_instance_collection.h,v 1.4.20.1 2005/12/11 00:45:34 fang Exp $
 */

#ifndef	__OBJECT_INST_ENUM_INSTANCE_COLLECTION_H__
#define	__OBJECT_INST_ENUM_INSTANCE_COLLECTION_H__

#include "Object/inst/datatype_instance_collection.h"
#include "Object/inst/enum_instance.h"
#include "Object/inst/instance_collection.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/parameterless_collection_type_manager.h"

namespace HAC {
namespace entity {
//=============================================================================
// convenient typedefs

typedef	instance_array<enum_tag, 0>	enum_scalar;
typedef	instance_array<enum_tag, 1>	enum_array_1D;
typedef	instance_array<enum_tag, 2>	enum_array_2D;
typedef	instance_array<enum_tag, 3>	enum_array_3D;
typedef	instance_array<enum_tag, 4>	enum_array_4D;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __OBJECT_INST_ENUM_INSTANCE_COLLECTION_H__

