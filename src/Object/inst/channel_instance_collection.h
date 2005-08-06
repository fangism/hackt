/**
	\file "Object/inst/channel_instance_collection.h"
	Class declarations for channel instance and collections.  
	This file originated from "Object/art_object_instance_chan.h"
		in a previous life.  
	$Id: channel_instance_collection.h,v 1.2.4.2 2005/08/06 15:42:27 fang Exp $
 */

#ifndef	__OBJECT_INST_CHANNEL_INSTANCE_COLLECTION_H__
#define	__OBJECT_INST_CHANNEL_INSTANCE_COLLECTION_H__

#include "Object/inst/physical_instance_collection.h"
#include "Object/traits/chan_traits.h"
#include "Object/inst/instance_collection.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/general_collection_type_manager.h"
#include "Object/inst/state_instance.h"

namespace ART {
namespace entity {
//=============================================================================
#if 0
/**
	An actual instantiated instance of a channel.
	These are not constructed until after unrolling.  
	A final pass is required to construct the instances.  
 */
struct channel_instance {
	typedef	channel_instance		this_type;
	typedef	channel_instance_alias_info	alias_info_type;
	// need back-reference(s) to owner(s) or hierarchical keys?
	never_ptr<const alias_info_type>	back_ref;
public:
	channel_instance();

	explicit
	channel_instance(const alias_info_type&);

	~channel_instance();

#if 0
	PERSISTENT_METHODS_DECLARATIONS
#else
	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);
#endif
public:
	static
	instance_pool<this_type>		pool;
};	// end class chan_instance
#else
/**
	Channel state information.  
 */
class class_traits<channel_tag>::state_instance_base {
// state information, persistence...
};	// end class state_instance_base
#endif

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

