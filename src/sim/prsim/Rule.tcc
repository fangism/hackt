/**
	\file "sim/prsim/Rule.tcc"
	$Id: Rule.tcc,v 1.6 2008/12/01 20:27:37 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_RULE_TCC__
#define	__HAC_SIM_PRSIM_RULE_TCC__

#include <iostream>
#include "sim/prsim/Rule.hh"
#include "util/string.hh"		// for string_to_num

namespace HAC {
namespace SIM {
namespace PRSIM {
using util::strings::string_to_num;
#include "util/using_ostream.hh"

//=============================================================================
template <class Time>
Time
Rule<Time>::default_unspecified_delay = delay_policy<time_type>::default_delay;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Time>
bool
Rule<Time>::edit_property(const string& prop, const string& val) {
	time_type t;
	int i;
	const bool t_err = string_to_num(val, t);
	const bool i_err = string_to_num(val, i);
	if (prop == "unstab") {
		if (i_err) {
			cerr << "Error: value should be 0 or 1." << endl;
			return true;
		} else {
			if (i) {
				this->set_unstable();
			} else {
				this->clear_unstable();
			}
		}
	} else if (prop == "after") {
		if (t_err) {
			cerr << "Error: value should be floating-point." << endl;
			return true;
		} else {
			this->after = t;
		}
#if PRSIM_AFTER_RANGE
	} else if (prop == "after_min") {
		if (t_err) {
			cerr << "Error: value should be floating-point." << endl;
			return true;
		} else {
			this->after_min = t;
		}
	} else if (prop == "after_max") {
		if (t_err) {
			cerr << "Error: value should be floating-point." << endl;
			return true;
		} else {
			this->after_max = t;
		}
#endif
	}
	return false;
}

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

