/**
	\file "sim/prsim/Reference.h"
	Interface to refrence-parsing.
	Consider making this a general function in main/main_funcs.
	$Id: Reference.h,v 1.2 2006/01/22 06:53:30 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_REFERENCE_H__
#define	__HAC_SIM_PRSIM_REFERENCE_H__

#include "sim/common.h"
#include "util/string_fwd.h"

namespace HAC {
namespace entity {
class module;
}
namespace SIM {
namespace PRSIM {

extern
node_index_type
parse_node_to_index(const std::string&, const entity::module&);


}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_REFERENCE_H__

