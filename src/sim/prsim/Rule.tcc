/**
	\file "sim/prsim/Rule.tcc"
	$Id: Rule.tcc,v 1.4 2008/05/13 23:06:00 fang Exp $
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
	if (this->is_unstable()) {
		o << ", unstable";
	}
	if (this->is_always_random()) {
	       o << ", always-random";
	}
	return o;
}

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_RULE_TCC__

