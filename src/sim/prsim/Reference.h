/**
	\file "sim/prsim/Reference.h"
	Interface to refrence-parsing.
	Consider making this a general function in main/main_funcs.
	$Id: Reference.h,v 1.1.2.1 2006/01/16 06:58:58 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_REFERENCE_H__
#define	__HAC_SIM_PRSIM_REFERENCE_H__

#include "sim/common.h"
#include "util/string_fwd.h"

namespace HAC {
namespace SIM {
namespace PRSIM {

extern
node_index_type
parse_node_to_index(const std::string&);


}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_REFERENCE_H__

