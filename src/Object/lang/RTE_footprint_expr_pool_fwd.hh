/**
	\file "Object/lang/RTE_footprint_expr_pool_fwd.hh"
	All this trouble for a measly forward declaration.  
	$Id: RTE_footprint_expr_pool_fwd.hh,v 1.3 2008/11/11 20:06:19 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_RTE_FOOTPRINT_EXPR_POOL_FWD_HH__
#define	__HAC_OBJECT_LANG_RTE_FOOTPRINT_EXPR_POOL_FWD_HH__

#include <vector>

namespace util {
	template <class, int>	class offset_array;
}	// end namespace util

namespace HAC {
namespace entity {
namespace RTE {
class footprint_expr_node;

typedef util::offset_array<std::vector<footprint_expr_node>, 1>
				RTE_footprint_expr_pool_type;

}	// end namespace RTE
}	// end namespace entity
}	// end namespace HAC


#endif	// __HAC_OBJECT_LANG_RTE_FOOTPRINT_EXPR_POOL_FWD_HH__

