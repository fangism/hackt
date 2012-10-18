/**
	\file "Object/inst/struct_instance_collection.hh"
	Class declarations for built-in and user-defined data instances
	and instance collections.  
	This file was "Object/art_object_instance_struct.h"
		in a previous life.  
	$Id: struct_instance_collection.hh,v 1.6 2006/01/22 18:20:12 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_STRUCT_INSTANCE_COLLECTION_H__
#define	__HAC_OBJECT_INST_STRUCT_INSTANCE_COLLECTION_H__

#include "Object/inst/datatype_instance_collection.hh"
#include "Object/inst/struct_instance.hh"
#include "Object/inst/instance_collection.hh"
#include "Object/inst/instance_alias_info.hh"

namespace HAC {
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
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_STRUCT_INSTANCE_COLLECTION_H__

