/**
	\file "Object/inst/channel_instance_collection.h"
	Class declarations for channel instance and collections.  
	This file originated from "Object/art_object_instance_chan.h"
		in a previous life.  
	$Id: channel_instance_collection.h,v 1.6.34.1 2006/04/26 19:50:45 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_CHANNEL_INSTANCE_COLLECTION_H__
#define	__HAC_OBJECT_INST_CHANNEL_INSTANCE_COLLECTION_H__

#include "Object/inst/physical_instance_collection.h"
#include "Object/inst/channel_instance.h"
#include "Object/inst/instance_collection.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/general_collection_type_manager.h"
#include "Object/type/canonical_generic_chan_type.h"

namespace HAC {
namespace entity {

/**
	Need this because typedef not allowed in nested elaborated
	type specifiers in base classes.  (in this case, of
	the instance_collection)
 */
class class_traits<channel_tag>::collection_type_manager_parent_type
	: public general_collection_type_manager<channel_tag> {
};

//=============================================================================
// convenient typedefs

typedef	instance_array<channel_tag, 0>	channel_scalar;
typedef	instance_array<channel_tag, 1>	channel_array_1D;
typedef	instance_array<channel_tag, 2>	channel_array_2D;
typedef	instance_array<channel_tag, 3>	channel_array_3D;
typedef	instance_array<channel_tag, 4>	channel_array_4D;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_CHANNEL_INSTANCE_COLLECTION_H__

