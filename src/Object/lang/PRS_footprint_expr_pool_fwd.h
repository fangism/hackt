/**
	\file "Object/lang/PRS_footprint_expr_pool_fwd.h"
	All this trouble for a measly forward declaration.  
	$Id: PRS_footprint_expr_pool_fwd.h,v 1.1.2.1 2005/12/24 02:33:34 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_PRS_FOOTPRINT_EXPR_POOL_FWD_H__
#define	__HAC_OBJECT_LANG_PRS_FOOTPRINT_EXPR_POOL_FWD_H__

#include "util/list_vector_fwd.h"

namespace util {
	template <class, int>	class offset_array;
}	// end namespace util

namespace HAC {
namespace entity {
namespace PRS {
class footprint_expr_node;

typedef util::offset_array<util::list_vector<footprint_expr_node>, 1>
				PRS_footprint_expr_pool_type;

}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC


#endif	// __HAC_OBJECT_LANG_PRS_FOOTPRINT_EXPR_POOL_FWD_H__

