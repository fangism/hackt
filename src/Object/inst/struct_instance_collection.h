/**
	\file "Object/inst/struct_instance_collection.h"
	Class declarations for built-in and user-defined data instances
	and instance collections.  
	This file was "Object/art_object_instance_struct.h"
		in a previous life.  
	$Id: struct_instance_collection.h,v 1.3 2005/08/08 16:51:10 fang Exp $
 */

#ifndef	__OBJECT_INST_STRUCT_INSTANCE_COLLECTION_H__
#define	__OBJECT_INST_STRUCT_INSTANCE_COLLECTION_H__

#include "Object/inst/datatype_instance_collection.h"
#include "Object/traits/struct_traits.h"
#include "Object/inst/instance_collection.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/state_instance.h"

namespace ART {
namespace entity {
//=============================================================================
/**
	An actual instantiated instance of a struct.
	These are not constructed until after unrolling.  
	A final pass is required to construct the instances.  
 */
class class_traits<datastruct_tag>::state_instance_base {
};	// end class state_instance_base

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

