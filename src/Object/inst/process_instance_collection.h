/**
	\file "Object/inst/process_instance_collection.h"
	Class declarations for process instance and collections.  
	This file originated from "Object/art_object_instance_proc.h"
		in a previous life.
	$Id: process_instance_collection.h,v 1.3.4.1 2005/08/11 00:20:20 fang Exp $
 */

#ifndef	__OBJECT_INST_PROCESS_INSTANCE_COLLECTION_H__
#define	__OBJECT_INST_PROCESS_INSTANCE_COLLECTION_H__

#include "Object/inst/physical_instance_collection.h"
#include "Object/inst/process_instance.h"
#include "Object/inst/instance_collection.h"
#include "Object/inst/instance_alias_info.h"

namespace ART {
namespace entity {
//=============================================================================
// convenient typedefs

typedef	instance_array<process_tag, 0>	process_scalar;
typedef	instance_array<process_tag, 1>	process_array_1D;
typedef	instance_array<process_tag, 2>	process_array_2D;
typedef	instance_array<process_tag, 3>	process_array_3D;
typedef	instance_array<process_tag, 4>	process_array_4D;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_PROCESS_INSTANCE_COLLECTION_H__

