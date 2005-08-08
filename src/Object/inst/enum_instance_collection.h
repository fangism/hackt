/**
	\file "Object/inst/enum_instance_collection.h"
	Class declarations for built-in and user-defined data instances
	and instance collections.  
	This file was "Object/art_object_instance_enum.h"
		in a previous life.  
	$Id: enum_instance_collection.h,v 1.3 2005/08/08 16:51:08 fang Exp $
 */

#ifndef	__OBJECT_INST_ENUM_INSTANCE_COLLECTION_H__
#define	__OBJECT_INST_ENUM_INSTANCE_COLLECTION_H__

#include "Object/inst/datatype_instance_collection.h"
#include "Object/traits/enum_traits.h"
#include "Object/inst/instance_collection.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/state_instance.h"

namespace ART {
namespace entity {
//=============================================================================
/**
	An actual instantiated instance of an enum.
	These are not constructed until after unrolling.  
	A final pass is required to construct the instances.  
 */
class class_traits<enum_tag>::state_instance_base {
protected:
	int				state;
};	// end class state_instance_base

//=============================================================================
// convenient typedefs

typedef	instance_array<enum_tag, 0>	enum_scalar;
typedef	instance_array<enum_tag, 1>	enum_array_1D;
typedef	instance_array<enum_tag, 2>	enum_array_2D;
typedef	instance_array<enum_tag, 3>	enum_array_3D;
typedef	instance_array<enum_tag, 4>	enum_array_4D;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_ENUM_INSTANCE_COLLECTION_H__

