/**
	\file "sim/prsim/Rule.tcc"
	$Id: Rule.tcc,v 1.6 2008/12/01 20:27:37 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_RULE_TCC__
#define	__HAC_SIM_PRSIM_RULE_TCC__

#include <iostream>
#include "sim/prsim/Rule.h"

namespace HAC {
namespace SIM {
namespace PRSIM {
//=============================================================================
template <class Time>
Time
Rule<Time>::default_unspecified_delay = delay_policy<time_type>::default_delay;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Dump it.  
 */
template <class Time>
ostream&
Rule<Time>::dump(ostream& o) const {
	o << "after " << this->after;
#if PRSIM_AFTER_RANGE
	typedef	delay_policy<Time>		time_traits;
	const bool min = !time_traits::is_zero(this->after_min);
	const bool max = !time_traits::is_zero(this->after_max);
	if (min || max) {
		o << " [";
		if (min) o << this->after_min;
		else o << '0';
		o << ", ";
		if (max) o << this->after_max;
		else o << '*';
		o << ']';
	}
#endif
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

