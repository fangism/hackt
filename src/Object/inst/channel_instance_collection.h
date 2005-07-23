/**
	\file "Object/inst/channel_instance_collection.h"
	Class declarations for channel instance and collections.  
	This file originated from "Object/art_object_instance_chan.h"
		in a previous life.  
	$Id: channel_instance_collection.h,v 1.2 2005/07/23 06:52:35 fang Exp $
 */

#ifndef	__OBJECT_INST_CHANNEL_INSTANCE_COLLECTION_H__
#define	__OBJECT_INST_CHANNEL_INSTANCE_COLLECTION_H__

#include "Object/inst/physical_instance_collection.h"
#include "Object/traits/chan_traits.h"
#include "Object/inst/instance_collection.h"
#include "Object/inst/instance_alias.h"
#include "Object/inst/general_collection_type_manager.h"

namespace ART {
namespace entity {
//=============================================================================
/**
	An actual instantiated instance of a channel.
	These are not constructed until after unrolling.  
	A final pass is required to construct the instances.  
 */
struct channel_instance : public persistent {
	// need back-reference(s) to owner(s) or hierarchical keys?
	never_ptr<const channel_instance_alias_base>	back_ref;
public:
	channel_instance();

	~channel_instance();

	PERSISTENT_METHODS_DECLARATIONS
};	// end class chan_instance

//=============================================================================
// convenient typedefs

typedef	instance_array<channel_tag, 0>	channel_scalar;
typedef	instance_array<channel_tag, 1>	channel_array_1D;
typedef	instance_array<channel_tag, 2>	channel_array_2D;
typedef	instance_array<channel_tag, 3>	channel_array_3D;
typedef	instance_array<channel_tag, 4>	channel_array_4D;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_CHANNEL_INSTANCE_COLLECTION_H__

