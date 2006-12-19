/**
	\file "sim/chpsim/type_enum.h"
	$Id: type_enum.h,v 1.1.2.2 2006/12/19 23:44:13 fang Exp $
 */

#ifndef	__HAC_SIM_CHPSIM_TYPE_ENUM_H__
#define	__HAC_SIM_CHPSIM_TYPE_ENUM_H__

#include <utility>	// for std::pair
#include "util/size_t.h"
#include "sim/common.h"

namespace HAC {
namespace SIM {
namespace CHPSIM {

/**
	Enumeration for type indexed by step()'s return value.
	Keep these enumerations consistent with class_traits?
 */
enum {
	INSTANCE_TYPE_NULL = 0,
	INSTANCE_TYPE_BOOL,
	INSTANCE_TYPE_INT,
	INSTANCE_TYPE_CHANNEL
};
typedef	std::pair<size_t, node_index_type>
					instance_reference;
// typedef	instance_reference		step_return_type;

}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_CHPSIM_TYPE_ENUM_H__

