/**
	\file "Object/inst/int_instance_collection.hh"
	Class declarations for built-in and user-defined data instances
	and instance collections.  
	This file was "Object/art_object_instance_int.h"
		in a previous life.  
	$Id: int_instance_collection.hh,v 1.7 2006/11/07 06:34:53 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_INT_INSTANCE_COLLECTION_H__
#define	__HAC_OBJECT_INST_INT_INSTANCE_COLLECTION_H__

#include "Object/inst/datatype_instance_collection.hh"
#include "Object/inst/int_instance.hh"
#include "Object/inst/instance_collection.hh"
#include "Object/inst/instance_alias_info.hh"
#include "Object/inst/state_instance.hh"
#include "Object/inst/int_collection_type_manager.hh"

namespace HAC {
namespace entity {
//=============================================================================

ostream&
operator << (ostream&, const int_instance_alias_info&);

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

#endif	// __HAC_OBJECT_INST_INT_INSTANCE_COLLECTION_H__

