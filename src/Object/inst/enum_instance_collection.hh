/**
	\file "Object/inst/enum_instance_collection.hh"
	Class declarations for built-in and user-defined data instances
	and instance collections.  
	This file was "Object/art_object_instance_enum.h"
		in a previous life.  
	$Id: enum_instance_collection.hh,v 1.6 2006/01/22 18:20:04 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_ENUM_INSTANCE_COLLECTION_H__
#define	__HAC_OBJECT_INST_ENUM_INSTANCE_COLLECTION_H__

#include "Object/inst/datatype_instance_collection.hh"
#include "Object/inst/enum_instance.hh"
#include "Object/inst/instance_collection.hh"
#include "Object/inst/instance_alias_info.hh"
#include "Object/inst/parameterless_collection_type_manager.hh"

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

#endif	// __HAC_OBJECT_INST_ENUM_INSTANCE_COLLECTION_H__

