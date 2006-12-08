/**
	\file "sim/chpsim/Dependence.h"
	$Id: Dependence.h,v 1.1.2.1 2006/12/08 22:34:03 fang Exp $
 */

#ifndef	__HAC_SIM_CHPSIM_DEPENDENCE_H__
#define	__HAC_SIM_CHPSIM_DEPENDENCE_H__

namespace HAC {
namespace entity {
	class data_expr;
}
namespace SIM {
namespace CHPSIM {
using entity::data_expr;

/**
	Represents expressions that depend on a particular variable.  
	This captures the notion of variable fanout.  
	NOTE: we only need to add dependencies for expressions that
	may trigger events, i.e. guard expressions.  
	This is necessary for guards in selection statements or
	wait statements that may be blocked pending an expression
	becoming true.  
	Rvalue expressions need not apply because their uses are atomic.  
 */
class Dependence {
	/**
		Refers to a nonmeta expression (in CHP footprint)
	 */
	const data_expr&			guard_expression;
	// need a process context (footprint frame) to resolve
	//	local references.  

public:
	explicit
	Dependence(const data_expr& d) : guard_expression(d) { }
	
};	// end class Dependence

}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_CHPSIM_DEPENDENCE_H__


