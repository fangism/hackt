/**
	\file "sim/prsim/Rule.tcc"
	$Id: Rule.tcc,v 1.2 2006/04/23 07:37:27 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_RULE_TCC__
#define	__HAC_SIM_PRSIM_RULE_TCC__

#include <iostream>
#include "sim/prsim/Rule.h"

namespace HAC {
namespace SIM {
namespace PRSIM {
//=============================================================================
/**
	Dump it.  
 */
template <class Time>
ostream&
Rule<Time>::dump(ostream& o) const {
	o << "after " << this->after;
	if (this->is_weak()) {
		o << ", weak";
	}
	return o;
}

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_RULE_TCC__

