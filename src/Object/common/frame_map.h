/**
	\file "Object/common/frame_map.h"
	$Id: frame_map.h,v 1.2 2008/11/05 23:03:30 fang Exp $
 */

#ifndef	__HAC_OBJECT_COMMON_FRAME_MAP_H__
#define	__HAC_OBJECT_COMMON_FRAME_MAP_H__

#include <vector>
#include "util/size_t.h"

namespace HAC {
namespace entity {
class state_manager;
class footprint_frame;

template <class Tag>
struct  global_entry;

template <class Tag>
struct global_entry_pool;

/**
	Common type passed around for translating
	between local and global IDs.  
 */
typedef	std::vector<size_t>			footprint_frame_map_type;

}	// end namespace entity
}	// end namespace HAC
#endif	// __HAC_OBJECT_COMMON_FRAME_MAP_H__

