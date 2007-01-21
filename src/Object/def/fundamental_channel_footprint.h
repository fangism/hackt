/**
	\file "Object/def/fundamental_channel_footprint.h"
	NOTE: definitions will be in 
	"Object/type/canonical_fundamental_chan_type.cc"
	for ease of global pool construction ordering.  
	$Id: fundamental_channel_footprint.h,v 1.2 2007/01/21 05:58:40 fang Exp $
 */

#ifndef	__HAC_OBJECT_DEF_FUNDAMENTAL_CHANNEL_FOOTPRINT_H__
#define	__HAC_OBJECT_DEF_FUNDAMENTAL_CHANNEL_FOOTPRINT_H__

#include "util/size_t.h"
#include "Object/traits/classification_tags_fwd.h"
#include "util/memory/chunk_map_pool_fwd.h"

namespace HAC {
namespace entity {
//=============================================================================
/**
	Base class for containee of channel footprint.  
 */
template <class Tag>
class channel_footprint_base {
protected:
	/**
		This size is the total number of entries to reserve
		for this meta-type.  
		e.g. a chan(bool[2][2]) would have size 4-bools.
		This number is is computed by accumulating over the
		the channel-type's datatype_list.  
	 */
	size_t			_size;
public:
	channel_footprint_base() : _size(0) { }

};	// end class channel_footprint_base

//=============================================================================
/**
	Describes the data composition for a fundamental channel type.
	For now fundamental channel types consist only of int, bool, enum.  
	All this needs to do (for now) is track the *number* of each.
	e.g. chan(int[2][2], bool) -> 4 ints, 1 bool, 0 enums.  
 */
class fundamental_channel_footprint :
		public channel_footprint_base<bool_tag>,
		public channel_footprint_base<int_tag>,
		public channel_footprint_base<enum_tag> {
	typedef	fundamental_channel_footprint		this_type;
	typedef	channel_footprint_base<bool_tag>	channel_bool_base;
	typedef	channel_footprint_base<int_tag>		channel_int_base;
	typedef	channel_footprint_base<enum_tag>	channel_enum_base;
public:
	fundamental_channel_footprint() : 
		channel_bool_base(), 
		channel_int_base(), 
		channel_enum_base() { }
	~fundamental_channel_footprint() { }

	template <class Tag>
	size_t&
	size(void) {
		return channel_footprint_base<Tag>::_size;
	}

	template <class Tag>
	const size_t&
	size(void) const {
		return channel_footprint_base<Tag>::_size;
	}

public:
	// should be pool-allocated
	CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(32)

};	// end class fundamental_channel_footprint

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_DEF_FUNDAMENTAL_CHANNEL_FOOTPRINT_H__

