/**
	\file "sim/chpsim/Variable.h"
	$Id: Variable.h,v 1.1.2.1 2006/12/04 09:56:04 fang Exp $
 */

#ifndef	__HAC_SIM_CHPSIM_VARIABLE_H__
#define	__HAC_SIM_CHPSIM_VARIABLE_H__

#include "Object/expr/types.h"
#include <valarray>
#include "sim/chpsim/Dependence.h"

namespace HAC {
namespace SIM {
namespace CHPSIM {
using std::valarray;
typedef	valarray<Dependence>			fanout_list_type;

//=============================================================================
/**
	Boolean (nonmeta) variable class.  
	These are allocated during creation of the simulator state.  
	NOTE: eventually, there bools (abstract data) will be differentiated
		from the physical (node) bools.  
 */
class BoolVariable {
	/**
		The value type.
	 */
	typedef	char				value_type;
	fanout_list_type			fanouts;
public:
};	// end class BoolVariable

//=============================================================================

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_CHPSIM_VARIABLE_H__

