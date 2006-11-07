/**
	\file "Object/inst/bool_instance_collection.h"
	Class declarations for built-in boolean data instances
	and instance collections.  
	This file was "Object/art_object_instance_bool.h" in a previous life.  
	$Id: bool_instance_collection.h,v 1.7 2006/11/07 06:34:35 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_BOOL_INSTANCE_COLLECTION_H__
#define	__HAC_OBJECT_INST_BOOL_INSTANCE_COLLECTION_H__

#include "Object/inst/datatype_instance_collection.h"
#include "Object/inst/bool_instance.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/alias_empty.h"
#include "Object/inst/instance_collection.h"
#include "Object/inst/null_collection_type_manager.h"

namespace HAC {
namespace entity {

//=============================================================================
// class datatype_instance_collection declared in "art_object_instance.h"

//=============================================================================

ostream&
operator << (ostream&, const bool_instance_alias_info&);

//-----------------------------------------------------------------------------
// convenient typedefs

typedef	instance_array<bool_tag, 0>	bool_scalar;
typedef	instance_array<bool_tag, 1>	bool_array_1D;
typedef	instance_array<bool_tag, 2>	bool_array_2D;
typedef	instance_array<bool_tag, 3>	bool_array_3D;
typedef	instance_array<bool_tag, 4>	bool_array_4D;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_BOOL_INSTANCE_COLLECTION_H__

