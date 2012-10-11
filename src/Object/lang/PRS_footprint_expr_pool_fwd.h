/**
	\file "Object/lang/PRS_footprint_expr_pool_fwd.h"
	All this trouble for a measly forward declaration.  
	$Id: PRS_footprint_expr_pool_fwd.h,v 1.3 2008/11/11 20:06:19 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_PRS_FOOTPRINT_EXPR_POOL_FWD_H__
#define	__HAC_OBJECT_LANG_PRS_FOOTPRINT_EXPR_POOL_FWD_H__

#include <vector>

namespace util {
	template <class, int>	class offset_array;
}	// end namespace util

namespace HAC {
namespace entity {
namespace PRS {
class footprint_expr_node;

typedef util::offset_array<std::vector<footprint_expr_node>, 1>
				PRS_footprint_expr_pool_type;

}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC


#endif	// __HAC_OBJECT_LANG_PRS_FOOTPRINT_EXPR_POOL_FWD_H__

