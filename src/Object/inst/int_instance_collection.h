/**
	\file "Object/inst/int_instance_collection.h"
	Class declarations for built-in and user-defined data instances
	and instance collections.  
	This file was "Object/art_object_instance_int.h"
		in a previous life.  
	$Id: int_instance_collection.h,v 1.4.20.1 2005/12/11 00:45:37 fang Exp $
 */

#ifndef	__OBJECT_INST_INT_INSTANCE_COLLECTION_H__
#define	__OBJECT_INST_INT_INSTANCE_COLLECTION_H__

#include "Object/inst/datatype_instance_collection.h"
#include "Object/inst/int_instance.h"
#include "Object/inst/instance_collection.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/state_instance.h"
#include "Object/inst/int_collection_type_manager.h"

namespace HAC {
namespace entity {
//=============================================================================

ostream&
operator << (ostream&, const int_instance_alias_base&);

//-----------------------------------------------------------------------------
// convenient typedefs

typedef	instance_array<int_tag, 0>	int_scalar;
typedef	instance_array<int_tag, 1>	int_array_1D;
typedef	instance_array<int_tag, 2>	int_array_2D;
typedef	instance_array<int_tag, 3>	int_array_3D;
typedef	instance_array<int_tag, 4>	int_array_4D;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __OBJECT_INST_INT_INSTANCE_COLLECTION_H__

