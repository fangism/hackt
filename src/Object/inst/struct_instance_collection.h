/**
	\file "Object/inst/struct_instance_collection.h"
	Class declarations for built-in and user-defined data instances
	and instance collections.  
	This file was "Object/art_object_instance_struct.h"
		in a previous life.  
	$Id: struct_instance_collection.h,v 1.3.4.1 2005/08/11 00:20:21 fang Exp $
 */

#ifndef	__OBJECT_INST_STRUCT_INSTANCE_COLLECTION_H__
#define	__OBJECT_INST_STRUCT_INSTANCE_COLLECTION_H__

#include "Object/inst/datatype_instance_collection.h"
#include "Object/inst/struct_instance.h"
#include "Object/inst/instance_collection.h"
#include "Object/inst/instance_alias_info.h"

namespace ART {
namespace entity {
//=============================================================================
// convenient typedefs

typedef	instance_array<datastruct_tag, 0>	struct_scalar;
typedef	instance_array<datastruct_tag, 1>	struct_array_1D;
typedef	instance_array<datastruct_tag, 2>	struct_array_2D;
typedef	instance_array<datastruct_tag, 3>	struct_array_3D;
typedef	instance_array<datastruct_tag, 4>	struct_array_4D;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_STRUCT_INSTANCE_COLLECTION_H__

