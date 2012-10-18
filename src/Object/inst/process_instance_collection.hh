/**
	\file "Object/inst/process_instance_collection.hh"
	Class declarations for process instance and collections.  
	This file originated from "Object/art_object_instance_proc.h"
		in a previous life.
	$Id: process_instance_collection.hh,v 1.6 2006/01/22 18:20:11 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_PROCESS_INSTANCE_COLLECTION_H__
#define	__HAC_OBJECT_INST_PROCESS_INSTANCE_COLLECTION_H__

#include "Object/inst/physical_instance_collection.hh"
#include "Object/inst/process_instance.hh"
#include "Object/inst/instance_collection.hh"
#include "Object/inst/instance_alias_info.hh"

namespace HAC {
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
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_PROCESS_INSTANCE_COLLECTION_H__

