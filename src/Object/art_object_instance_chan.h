/**
	\file "Object/art_object_instance_chan.h"
	Class declarations for channel instance and collections.  
	$Id: art_object_instance_chan.h,v 1.11 2005/05/10 04:51:17 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INSTANCE_CHAN_H__
#define	__OBJECT_ART_OBJECT_INSTANCE_CHAN_H__

#include "Object/art_object_instance.h"
#include "Object/art_object_classification_details.h"
#include "Object/art_object_instance_collection.h"
#include "Object/art_object_instance_alias.h"


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

#endif	// __OBJECT_ART_OBJECT_INSTANCE_CHAN_H__

